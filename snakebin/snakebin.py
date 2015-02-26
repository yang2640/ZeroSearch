import base64
import hashlib
import json
import os
import random
import sqlite3
import string
import sys
import urlparse
import re



def http_resp(code, reason, content_type='message/http', msg='',
              extra_headers=None):
    if extra_headers is None:
        extra_header_text = ''
    else:
        extra_header_text = '\r\n'.join(
            ['%s: %s' % (k, v) for k, v in extra_headers.items()]
        )
        extra_header_text += '\r\n'

    resp = """HTTP/1.1 %(code)s %(reason)s\r
%(extra_headers)sContent-Type: %(content_type)s\r
Content-Length: %(msg_len)s\r
\r
%(msg)s"""
    resp %= dict(code=code, reason=reason, content_type=content_type,
                 msg_len=len(msg), msg=msg, extra_headers=extra_header_text)
    sys.stdout.write(resp)


class Job(object):

    def __init__(self, name, args, exe=None):
        self.name = name
        self.args = args
	if exe is not None:
	    self.execuation = exe
	else:
	    self.execuation = 'file://python2.7:python'


        self.devices = [
            {'name': 'python2.7'},
            {'name': 'stdout', 'content_type': 'message/http'},
            {'name': 'image', 'path': 'swift://~/snakebin-app/snakebin.zapp'},
        ]
        self.env = {}

    def add_device(self, name, content_type=None, path=None):
        dev = {'name': name}
        if content_type is not None:
            dev['content_type'] = content_type
        if path is not None:
            dev['path'] = path
        self.devices.append(dev)

    def set_envvar(self, key, value):
        self.env[key] = value

    def to_json(self):
        return json.dumps([self.to_dict()])

    def to_dict(self):
        return {
            'name': self.name,
            'exec': {
                'path': self.execuation,
                'args': self.args,
                'env': self.env,
            },
            'devices': self.devices,
        }


def _object_exists(name):
    """Check the local container (mapped to ) to see if it contains
    an object with the given name. /dev/input is expected to be a sqlite
    database.
    """
    conn = sqlite3.connect('/dev/input')
    try:
        cur = conn.cursor()
        sql = 'SELECT ROWID FROM object WHERE name=? AND deleted=0'
        cur.execute(sql, (name, ))
        result = cur.fetchall()
        return len(result) > 0
    finally:
        conn.close()




# HTTP request
# GET /snakebin-api                   Generate a template index.html to reply back
# GET /snakebin-api/:script/search    create a job chain for VocabMatch
# GET /snakebin-api/:script           create a job chain for get_file.py

def get():
    path_info = os.environ.get('PATH_INFO')
    file_name = None
    search = None
    file_name, search = re.match('.*/snakebin-api/?(\w+)?(/search)?', path_info).groups()

    if not file_name:
	# Get empty form page:
	with open('index.html') as fp:
	    index_page = fp.read()
	http_resp(200, 'OK', content_type='text/html; charset=utf-8',
		  msg=index_page)
    elif _object_exists('%s.jpg.pgm.sift' % file_name):
	# The client has requested a real document.
	# Spawn a job to go and retrieve it:
	if search:
	    private_file_path = 'swift://~/snakebin-store/%s.jpg.pgm.sift' % file_name

	    job = Job('VocabMatch', 'vocab.db  list.txt  /dev/input  10', exe='swift://~/snakebin-api/VocabMatch.nexe')
	    job.add_device('input', path=private_file_path)
	    job.add_device('stderr', path='swift://~/snakebin-api/err.txt')
	    job.set_envvar('HTTP_ACCEPT', os.environ.get('HTTP_ACCEPT'))

	    http_resp(200, 'OK', content_type='application/json', msg=job.to_json(), extra_headers={'X-Zerovm-Execute': '1.0'})
	else:
	    private_file_path = 'swift://~/snakebin-store/%s.jpg' % file_name
	    job = Job('snakebin-get-file', 'get_file.py')
	    job.add_device('input', path=private_file_path)
	    job.set_envvar('HTTP_ACCEPT', os.environ.get('HTTP_ACCEPT'))
	    http_resp(200, 'OK', content_type='application/json', msg=job.to_json(), extra_headers={'X-Zerovm-Execute': '1.0'})
    else:
	http_resp(404, 'Not Found')

if __name__ == '__main__':
    request_method = os.environ.get('REQUEST_METHOD')

    if request_method == 'POST':
        post()
    elif request_method == 'GET':
        get()
    else:
        http_resp(405, 'Method Not Allowed')
