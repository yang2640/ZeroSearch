/* 
 * Copyright 2011-2012 Noah Snavely, Cornell University
 * (snavely@cs.cornell.edu).  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:

 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NOAH SNAVELY ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NOAH SNAVELY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * The views and conclusions contained in the software and
 * documentation are those of the authors and should not be
 * interpreted as representing official policies, either expressed or
 * implied, of Cornell University.
 *
 */

/* VocabMatch.cpp */
/* Read a database stored as a vocab tree and score a set of query images */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctime>

#include <string>
#include <algorithm>

#include "VocabTree.h"
#include "keys2.h"

#include "defines.h"
#include "qsort.h"

/* Read in a set of keys from a file 
 *
 * Inputs:
 *   keyfile      : file from which to read keys
 *   dim          : dimensionality of descriptors
 * 
 * Outputs:
 *   num_keys_out : number of keys read
 *
 * Return value   : pointer to array of descriptors.  The descriptors
 *                  are concatenated together in one big array of
 *                  length num_keys_out * dim 
 */
unsigned char *ReadKeys(const char *keyfile, int dim, int &num_keys_out)
{
    short int *keys;
    keypt_t *info = NULL;
    int num_keys = ReadKeyFile(keyfile, &keys, &info);
    
    unsigned char *keys_char = new unsigned char[num_keys * dim];
        
    for (int j = 0; j < num_keys * dim; j++) {
        keys_char[j] = (unsigned char) keys[j];
    }

    delete [] keys;

    if (info != NULL) 
        delete [] info;

    num_keys_out = num_keys;

    return keys_char;
}

int BasifyFilename(const char *filename, char *base)
{
    strcpy(base, filename);
    base[strlen(base) - 4] = 0;    

    return 0;
}

#if 0
/* Functions for outputting a webpage synopsis of the results */
void PrintHTMLHeader(FILE *f, int num_nns) 
{
    fprintf(f, "<html>\n"
            "<header>\n"
            "<title>Vocabulary tree results</title>\n"
            "</header>\n"
            "<body>\n"
            "<h1>Vocabulary tree results</h1>\n"
            "<hr>\n\n");

    fprintf(f, "<table border=2 align=center>\n<tr>\n<th>Query image</th>\n");
    for (int i = 0; i < num_nns; i++) {
        fprintf(f, "<th>Match %d</th>\n", i+1);
    }
    fprintf(f, "</tr>\n");
}

void PrintHTMLRow(FILE *f, const std::string &query, 
                  double *scores, int *perm, int num_nns,
                  const std::vector<std::string> &db_images)
{
    char q_base[512], q_thumb[512];
    BasifyFilename(query.c_str(), q_base);
    sprintf(q_thumb, "%s.thumb.jpg", q_base);

    fprintf(f, "<tr align=center>\n<td><img src=\"%s\"</td>\n", q_thumb);

    for (int i = 0; i < num_nns; i++) {
        char d_base[512], d_thumb[512];
        BasifyFilename(db_images[perm[i]].c_str(), d_base);
        sprintf(d_thumb, "%s.thumb.jpg", d_base);

        fprintf(f, "<td><img src=\"%s\"</td>\n", d_thumb);
    }
            
    fprintf(f, "</tr>\n<tr align=right>\n");

    fprintf(f, "<td></td>\n");
    for (int i = 0; i < num_nns; i++) 
        fprintf(f, "<td>%0.5f</td>\n", scores[i]);

    fprintf(f, "</tr>\n");
}

void PrintHTMLFooter(FILE *f) 
{
    fprintf(f, "</tr>\n"
            "</table>\n"
            "<hr>\n"
            "</body>\n"
            "</html>\n");
}
#endif

int main(int argc, char **argv) 
{
    const int dim = 128;

    if (argc != 5 && argc != 6 && argc != 7) {
        fprintf(stderr, "Usage: %s <vocab.db> <list.txt> <query_image> <num_nbrs> "
               "[distance_type:1] [normalize:1]\n", argv[0]);
        return 1;
    }
    
    /* a list of arguments */
    // dev/vocab.db
    char *tree_in = argv[1];
    // dev/list.in
    char *db_in = argv[2];
    // dev/query_image
    char *query_image = argv[3];
    // 4
    int num_nbrs = atoi(argv[4]);
    DistanceType distance_type = DistanceMin;
    bool normalize = true;


#if 0    
    if (argc >= 6)
        output_html = argv[5];
#endif

    if (argc >= 6)
        distance_type = (DistanceType) atoi(argv[5]);

    if (argc >= 7)
        normalize = (atoi(argv[6]) != 0);

    fprintf(stderr, "[VocabMatch] Using tree %s\n", tree_in);

    switch (distance_type) {
    case DistanceDot:
        fprintf(stderr, "[VocabMatch] Using distance Dot\n");
        break;        
    case DistanceMin:
        fprintf(stderr, "[VocabMatch] Using distance Min\n");
        break;
    default:
        fprintf(stderr, "[VocabMatch] Using no known distance!\n");
        break;
    }

    /* Read the tree */
    fprintf(stderr, "[VocabMatch] Reading tree...\n");

    VocabTree tree;
    tree.Read(tree_in);

#if 1
    tree.Flatten();
#endif

    tree.SetDistanceType(distance_type);
    tree.SetInteriorNodeWeight(0, 0.0);

    
    /* Read the database keyfiles */
    FILE *f = fopen(db_in, "r");
    
    std::vector<std::string> db_files;
    char buf[256];
    while (fgets(buf, 256, f)) {
        /* Remove trailing newline */
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;

        db_files.push_back(std::string(buf));
    }

    fclose(f);


    int num_db_images = db_files.size();

    fprintf(stderr, "[VocabMatch] Read %d database images\n", num_db_images);


#if 0
    FILE *f_html = fopen(output_html, "w");
    PrintHTMLHeader(f_html, num_nbrs);
#endif

    float *scores = new float[num_db_images];
    double *scores_d = new double[num_db_images];
    int *perm = new int[num_db_images];



    /* Clear scores */
    for (int j = 0; j < num_db_images; j++) 
        scores[j] = 0.0;

    unsigned char *keys;
    int num_keys;

    keys = ReadKeys(query_image, dim, num_keys);

    double mag = tree.ScoreQueryKeys(num_keys, normalize, keys, scores);

    /* Find the top scores */
    for (int j = 0; j < num_db_images; j++) {
        scores_d[j] = (double) scores[j];
    }

    qsort_descending();
    qsort_perm(num_db_images, scores_d, perm);        

    int top = MIN(num_nbrs, num_db_images);

    // generate the doc url
    std::string msg = "";
    for (int j = 0; j < top; j++) {
	std::string file_name = db_files[perm[j]];
	std::string host = getenv("HTTP_HOST");
	std::string acct = getenv("PATH_INFO");
	// strip the '/' for PATH_INFO
	std::size_t first = acct.find_first_not_of('/');
	std::size_t last = acct.find_last_not_of('/');
	acct = acct.substr(first, last-first+1);
	char doc_url[256] = "\0";
	sprintf(doc_url, "http://%s/api/%s/snakebin-api/%s\n", host.c_str(), acct.c_str(), file_name.c_str());
	msg += doc_url;
	msg += ",";
    }
    // remove the last added ',' charactor
    msg = msg.substr(0, msg.size()-1);
    // close the message with the ']'
    // generate the http_resp here and print to the stdout
    char http_resp[256] = "\0";
    sprintf(http_resp, "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s", msg.size(), msg.c_str());
    printf(http_resp);
    // ****************************************************
    
    fflush(stdout);

#if 0
    PrintHTMLRow(f_html, query_image, scores_d, 
                     perm, num_nbrs, db_files);
#endif

    delete [] keys;


#if 0
    PrintHTMLFooter(f_html);
    fclose(f_html);
#endif

    delete [] scores;
    delete [] scores_d;
    delete [] perm;

    return 0;
}
