// COMP2521 21T2 Assignment 1
// tw.c ... compute top N most frequent words in file F
// Usage: ./tw [Nwords] File

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "stemmer.h"
#include "WFreq.h"

#define MAXLINE 1000
#define MAXWORD 100

#define START "*** START OF"
#define END "*** END OF"

#define stopfile "stopwords"

#define isWordChar(c) (isalnum(c) || (c) == '\'' || (c) == '-')

// add function prototypes for your own functions here

int main(int argc, char *argv[]) {
	int   nWords;    // number of top frequency words to show
	char *fileName;  // name of file containing book text

	FILE *in;

	Dict stopwords;
	Dict book;

	char line[MAXLINE] = "";
	char word[MAXWORD] = "";

	// process command-line args
	switch (argc) {
		case 2:
			nWords = 10;
			fileName = argv[1];
			break;
		case 3:
			nWords = atoi(argv[1]);
			if (nWords < 10) nWords = 10;
			fileName = argv[2];
			break;
		default:
			fprintf(stderr,"Usage: %s [Nwords] File\n", argv[0]);
			exit(EXIT_FAILURE);
	}
	// deal with stopwords

	in = fopen(stopfile, "r");
	
	if(in == NULL) { 
		// given failure condition
		fprintf(stderr, "Can't open stopwords\n");
		return EXIT_FAILURE;
	}
	// creating a dictionary with stopwords

	stopwords = DictNew();
	while(fgets(line, MAXWORD, in) != NULL) {
		int length = strlen(line);
		for(int i = 0; i < length - 1; i++){
			word[i] = line[i];
		}
		DictInsert(stopwords, word);
		memset(word, '\0', sizeof(char) * MAXWORD);
	}
	fclose(in);
	//remove when done >>
	
	
	// deal with book file
	in = fopen(fileName, "r");
	if (in == NULL) { // given failure condition 
      fprintf(stderr, "Can't open %s\n",fileName);
      return EXIT_FAILURE;
   } 
   // skipping to start of the readable text
   while (fgets(line, MAXWORD, in) != NULL) {
      if (strncmp(line, START, 12) == 0) {
         break;
      }
   }
   // printf("checking for gutenberg\n");
   if (feof(in)) { // "*** START OF" line failure condition
      fprintf(stderr, "Not a Project Gutenberg book\n");
      return EXIT_FAILURE;
   }
   // DictShow(stopwords);
   
   //int highest = 0;
   //WFreq highP;
   book = DictNew();
	// start ingesting words into dictionary
	while(fgets(line, MAXLINE, in) != NULL) {
		if(line[0] != '\n') {
			if (!strncmp(line, END, strlen(END))) break;
			int i = 0;
			char c;
			for(int j = 0; line[j]; j++) {
				// sorting through words
				c = line[j];
				int wChar = isWordChar(c);
				// if the word is valid character add it to the curWord
				if (wChar) {
					word[i] = tolower(c);
					i++;
				// if the next character isn't a valid character then we move to add	
				} else if (!wChar) {
					// checking if word is a stopword
					if(DictFind(stopwords, word) == 0) {
						// stemming word and 
						//printf("stemming %s\n", curWord);
						stem(word, 0, strlen(word) - 1);
						// checking word length
						if(strlen(word) > 1) {
							
							DictInsert(book, word);
							//int num = DictFind(book, word);
							//if (num > highest) highest = num;
						}
					}
					// resetng word variable
					memset(word, 0, MAXWORD);
					i = 0;
				}
			}
		}
	}
	WFreq *results = malloc(nWords * sizeof(*results));
	// results[0] = highP;


	nWords = DictFindTopN(book, results, nWords);

	for(int i = 0; i < nWords; i++) {
		if(results[i].freq == 0) break;
		
		fprintf(stdout, "%d %s\n", results[i].freq, results[i].word);
	}

	DictFree(stopwords);
	DictFree(book);
	return EXIT_SUCCESS;

	// calculate most calculated words

}

// add your own functions here

