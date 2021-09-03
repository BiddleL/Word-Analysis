// COMP2521 21T2 Assignment 1
// Dict.c ... implementation of the Dictionary ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dict.h"
#include "WFreq.h"

#define MAXWORD 100

// you may define your own structs here
typedef struct DictNode *Node;

struct DictNode {
	WFreq data;
	Node left;
	Node right;
	int height;
};

struct DictRep {
	Node tree;
};

// add function prototypes for your helper functions here

static Node insertTree(Node n, char *word);
static Node searchDict(Node n, char *w);
static int topN(Node n, WFreq *wfs, int s);

// helper
static int calculateHeight(Node n);
static int getHeight(Node n);
static Node rotateLeft(Node n);
static Node rotateRight(Node n);
static void freeTree(Node n);
static void viewTree(Node n);
static void sortList(WFreq *wfs, int n);



// Creates a new Dictionary
Dict DictNew(void) {
	Dict new = malloc(sizeof(Dict));
	if (new == NULL) {
		fprintf(stderr, "Insufficient memory for the NODE!\n"); 
        exit(EXIT_FAILURE);
	}
	new->tree = NULL;

	return new;
}

// Frees the given Dictionary
void DictFree(Dict d) {
	freeTree(d->tree);
	free(d);
}

// Inserts an occurrence of the given word into the Dictionary
void DictInsert(Dict d, char *word) {
	assert(d != NULL && word != NULL);
	Node record = searchDict(d->tree, word);
	if (record != NULL) {
		record->data.freq++;
	} else {
		d->tree = insertTree(d->tree, word);
		//d->nodes++;
	}
}

// Returns the occurrence count of the given word. Returns 0 if the word
// is not in the Dictionary.
int DictFind(Dict d, char *word) {
	assert(d != NULL && word != NULL);
	Node n = searchDict(d->tree, word);
	if (n == NULL) return 0;
	return n->data.freq;
}

// Finds  the top `n` frequently occurring words in the given Dictionary
// and stores them in the given  `wfs`  array  in  decreasing  order  of
// frequency,  and then in increasing lexicographic order for words with
// the same frequency. Returns the number of WFreq's stored in the given
// array (this will be min(`n`, #words in the Dictionary)) in  case  the
// Dictionary  does  not  contain enough words to fill the entire array.
// Assumes that the `wfs` array has size `n`.
int DictFindTopN(Dict d, WFreq *wfs, int n) {
	assert(d != NULL && n > 0);
	for (int i = 0; i < n; i++) {
		wfs[i].word = (char *) malloc(MAXWORD * sizeof(char));
		wfs[i].freq = 0;
	}
	int help = topN(d->tree, wfs, n);
	sortList(wfs, help);
	return help;
}

// Displays the given Dictionary. This is purely for debugging purposes,
// so  you  may  display the Dictionary in any format you want.  You may
// choose not to implement this.
void DictShow(Dict d) {
	viewTree(d->tree);
}

static void freeTree(Node n) {
	if (n == NULL) return;
	freeTree(n->left);
	freeTree(n->right);
	free(n->data.word);
	free(n);
}

// helper functions

// Rotates the tree left
static Node rotateLeft(Node n) {
	if (n == NULL) return NULL;
	Node n1 = n->right;
	if (n1 == NULL) return n;
	n->right = n1->left;
	n1->left = n;
	// updating heights here
	n1->height = calculateHeight(n1);
	n1->left->height = calculateHeight(n1->left);
	return n1;
}
// Rotates tree right
static Node rotateRight(Node n) {
	if (n == NULL) return NULL;
	Node n1 = n->left;
	if (n1 == NULL) return n;
	n->left = n1->right;
	n1->right = n;
	// updating heights here
	n1->height = calculateHeight(n1);
	n1->right->height = calculateHeight(n1->right);

	return n1;
}

// finds height of node
static int getHeight(Node n) {
	if (n == NULL)	return -1;
	return n->height;
}

// returns the correct value for the node n
static int calculateHeight(Node n) {
	if (n == NULL) return 0;
	int left = getHeight(n->left);
	int right = getHeight(n->right);
	return (left > right ? left : right) + 1;

}

// calculates the balance of the subtree with n being the head
static int getBalance(Node n) {
	if (n == NULL) return 0;
	return getHeight(n->left) - getHeight(n->right);
}

// create a newNode for the tree with string w
static Node newNode(char *word) {
	Node n = malloc(sizeof(struct DictNode));
	if (n == NULL) {
		fprintf(stderr, "Not enough memory for a new node\n");
		return NULL;
	} 
	n->data.freq = 1;
	n->data.word = malloc((strlen(word) + 1) * sizeof(char));
	strcpy(n->data.word, word);
	n->data.word[strlen(word)] = '\0';
	n->left = NULL;
	n->right = NULL;
	n->height = 0;
	return n;
}
// searches a dictionary for a n with a matching string and returns that node;
static Node searchDict(Node n, char *w) {
	if (w == NULL) exit(EXIT_FAILURE);
	if (n == NULL) return NULL;
	int cmp = strcmp(w, n->data.word);
	if (cmp == 0) {
		return n;
	} else if (cmp > 0) {
		return searchDict(n->left, w);
	} else {
		return searchDict(n->right, w);
	}
}

// inserts string w, into the correct position in the AVL tree
static Node insertTree(Node n, char *w) {
	if(n == NULL)
	{
		return newNode(w);
	}
	int balance = getBalance(n);
	int cmp = strcmp(w, n->data.word);
	/*if (cmp == 0) {
		n->data.freq++;
		return n;
	} else */
	if(cmp > 0)	
	{
		n->left = insertTree(n->left, w);
		if(balance > 0)
		{
			if(cmp < 0)	n->left = rotateLeft(n->left);
			n = rotateRight(n);
		}	
	}	
	else if(cmp < 0)	
	{		
		n->right = insertTree(n->right, w);
		if( balance > 0)
		{		
			if(cmp > 0)	n->right = rotateRight(n->right);
			n = rotateLeft(n);
		}
	}
	n->height = calculateHeight(n);
	return n;
}

static int topN(Node n, WFreq *wfs, int s) {
	if (n == NULL) return 0;
	
	int l = topN(n->right, wfs, s);
	int i = 0;
	while (n->data.freq <= wfs[i].freq && i < s) {
		i++;
	}
	// 
	for(int j = s - 1; j > i; j--) {
		wfs[j] = wfs[j - 1];
	}
	sortList(wfs, i - 1);

	wfs[i] = (n->data);
	int r = topN(n->left, wfs, s); 
	
	//printf("i = %d\n", i);
	int tmp = (l > r ? l : r);
	int top = (tmp > i ? tmp : i);
	return top;
}


static void viewTree(Node n) {
	if(n == NULL)	return;	
	viewTree(n->left);
	printf("(%s, %d, h:%d)\n", n->data.word, n->data.freq, n->height);
	viewTree(n->right);
}

static void sortList(WFreq *wfs, int n) {
   char *temp = NULL;
	for (int i = 0; i < n - 1; ++i) {
		for (int j = i + 1; j < n; ++j) {
			//printf("i %d, j %d\n", i, j);
			// swapping strings if they are not in the lexicographical order
			if (wfs[i].freq == wfs[j].freq) {
				if (strcmp(wfs[i].word, wfs[j].word) > 0) {
					temp = strdup(wfs[i].word);
					wfs[i].word = wfs[j].word;
					wfs[j].word = temp;
					/*
					strcpy(temp, wfs[i].word);
					strcpy(wfs[i].word, wfs[j].word);
					strcpy(wfs[j].word, temp);*/
				}
			}
		}	
	}
}