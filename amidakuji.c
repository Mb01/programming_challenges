#include "stdio.h"
#include "stdlib.h"
#include "string.h"/* for memset */

/* for solving how many permutations can be reached in an amidakuji with SIZE elements and DEPTH swaps */

/* number of swaps of adjacent numbers */
const int DEPTH = 10;
/* number of elements */
const int SIZE =7;

/* return map of factorials up to n */
int *factorial(int n);
/* lexicographically rank a permutation of (1 .. length) inclusive */
int lex_rank(const int *permutation);
/* inverse of lex_rank */
int* inv_lex_rank(unsigned int hash);
/* swap at position "at" and "at+1" */
void swap(int* arr, int at);
/* use these to solve */
int solve();
/* print an array of size SIZE */
void prar(int* arr);
/*test the inverse functions on each other */
void test_lex_rank();

/* we can use a hash table to implement a set of permutations */
/* then we compute the reachable permutations in each generation */
/* we use each computation to build the next */
/* if we count the reachable permutations in the last generation, that is the answer */

int solve(){
    /* need factorial to calculate size of tables */
	int* fact = factorial(SIZE);
    /* maximum size of a hash of size SIZE*/
	int max_index = fact[SIZE] - 1;
    /* tables to store hashes of permutations */
	char* table_now = calloc(max_index, sizeof(char));
	char* table_next = calloc(max_index, sizeof(char));
    /* table pointer for swapping */
	char* table_temp = NULL;
    /* table index */
	unsigned int ti;
    /* how deep we are */
	int depth;
    /* set the initial configuration, an ascending sort of consecutive integers 1 .. SIZE */
	table_now[0] = 1;
    /* perform all possible swaps from positions in "table_now" */
    /* record these swapped positions in table_next */
	for(depth = 0 ; depth < DEPTH; depth++){
		/* iterate over generation, build next generation */
		for(ti = 0; ti < max_index; ti++){
            /* if the position was reached last time */
			if(table_now[ti] == 1){
                /* rebuild the sequence from the hash */
				int* sequence = inv_lex_rank(ti);
				int swaps;
				for(swaps = 0; swaps < SIZE - 1; swaps++){
					/* swap it */
					swap(sequence, swaps);
                    /* set the resulting sequence in the next generation */
					table_next[lex_rank(sequence)] = 1;
					/* swap it back */
					swap(sequence, swaps);
				}/* end for (swaps) */
                /* we're done with this sequence */
				free(sequence);
			}/* end if */
		}/* end for (ti) */
		/* swap tables */
		table_temp = table_next;
		table_next = table_now;
		table_now = table_temp;
        /* clear out next */
		memset(table_next, 0, fact[SIZE] - 1);
	}/* end for (depth) */
	/* now count the set bits */
	int answer = 0;
	for(ti = 0; ti < max_index; ti++){
		if(table_now[ti] > 0){
			answer++;
		}
	}
	printf("%d\n", answer);
	free(table_now);
	free(table_next);
	return answer;
}

/* lexicographically rank a permutation of (1 .. length) inclusive */
int lex_rank(const int *permutation){
    int pos;
    unsigned int rank = 0;
    int mask[SIZE];
    /* get an array with a map of factorials */
    int* fact = factorial(SIZE);
    /* initialize mask */
    for(pos = 0; pos < SIZE; pos++){
        mask[pos] = 1;
    }
    /* add offset at each position to rank */
    /* note that the final position always produces a count of 0 so we skip it */
    for(pos = 0; pos < SIZE - 1; pos++){
        /* use reference and mask to count how many remaining integers are less than permutation[pos] */
        /* remove used integer from reference from mask */
        mask[ permutation[pos] ] = 0;
        /* count how many remaining integers are less than permuation[pos] */
        int x;
        int count = 0;
        for(x = 1; x < SIZE + 1; x++){ /* 1 .. length inclusive */
            if(x >= permutation[pos]){
                break;
            }
            if(mask[x] == 1){
                count++;
            }
        }/* end counting */
        /* use the count to determine how much offset there is */
        if(count == 0){
            continue;
        }
        /* add offset */
        /* (n! / size) * count == (n - 1)! * count */
        rank += fact[SIZE - pos - 1] * count;
    }
    return rank;
}

/* inverse of lex_rank, returns a malloced to be freed */
int* inv_lex_rank(unsigned int hash){
	int* answer = malloc(sizeof(int) * SIZE);
	int* fact = factorial(SIZE);
	int pos;
	int mask[SIZE];
    /* initialize mask i.e. set everything to unused */
    for(pos = 0; pos < SIZE; pos++){
        mask[pos] = 1;
    }
	int x;
	/* compute which compartments it is in */
	for(x = 0; x < SIZE - 1; x++){
		int compartment_size = fact[SIZE - x] / (SIZE - x);
		/* find out which compartment it's in */
		answer[x] = hash / compartment_size;
		/* subtract that from the offset */
		hash -= (answer[x] * compartment_size);
	}
	/* last compartment has size 0, there are no numbers left to skip over */
	answer[SIZE - 1] = 0;
    /* now answer should contain the number of unused numbers to skip over */
	/* pick these from the mask */
	/* replace the space in answer with that number */
	for(x = 0, pos = 0; x < SIZE; x++, pos = 0){ 
		/* pick the answer[x] number from the remaining numbers */
        do{
        pos++;
		/* bypass any numbers already used */
            if(mask[pos]){
                /* subtract from the number to be skipped */
                answer[x]--;
            }

        }while(answer[x] > -1);
		/* ...that's our answer */
		answer[x] = pos;
		/* mark the number as used */
		mask[pos] = 0;
	}
	return answer;
}

/* swap the position at and at+1*/
void swap(int* arr, int at){
	int t = arr[at];
	arr[at] = arr[at+1];
	arr[at+1] = t;
}

/* return an array mapped to factorial at index */
int *factorial(int n){
    static int* arr = NULL;
    static unsigned int size;
    /* move off zero based indexing */
    n = n + 1;
    if( n > size || arr == NULL){
        if(arr == NULL){
            /* size is where we are going to start calculating from */
            size = 1;
            arr = malloc(sizeof(int) * n);
            arr[0] = 1;
        }
        else{
            arr = realloc(arr, sizeof(int) * n);
        }
        /* note we are increasing size to equal n */
        for(;size < n; size++){
            /* set each position to it's index times the computed factorial at one less index */
            arr[size] = size * arr[size - 1];
        }
    }
    return arr;
}

/* entry point */
int main(int argc, char** argv){
    solve();
    return 0;
}

/* debug */

/* print an array of size SIZE */
void prar(int* arr){
	int x;
	for(x = 0; x < SIZE; x++){
		printf("%d ", arr[x]);	
	}
	printf("\n");
}

/*test the inverse functions on each other */
void test_lex_rank(){
    int x;
    for(x = 0; x < 5040; x++){
        if(lex_rank(inv_lex_rank(x)) != x){
            printf("%d doesn't work\n", x);
            prar(inv_lex_rank(x));
            fflush(stdout);
        }
    }
}
