#include <stdio.h>
#include <stdlib.h>

#define WRONG_PATH -1;

typedef struct{
	/*
		N - number of sources
		M - number of recievers
		P - output amount
		Q - input amount
	*/	
	int N, M;
	int *P, *Q;
} indata;

typedef struct{
	/*
		from - sender id
		where - receiver id
		amount - amount
	*/	
	int from, where, amount;
} path_node;

inline void loader(indata *inputstruct, FILE *fstream) __attribute__((always_inline));
inline int guider(path_node **n_p_Pointer, indata *inputstruct) __attribute__((always_inline));
inline int min(int a, int b) __attribute__((always_inline));
inline void path_to_file(path_node *n_ps_Pointer, int size, FILE *outfile) __attribute__((always_inline));


inline void loader(indata *inputstruct, FILE *fstream){
	int *bufP, *bufQ;
	
	fscanf(fstream, "%i %i", &(inputstruct->N), &(inputstruct->M));
	
	inputstruct->P = (int*)malloc(inputstruct->N*sizeof(int));
	inputstruct->Q = (int*)malloc(inputstruct->M*sizeof(int));
	
	bufP=inputstruct->P;
	bufQ=inputstruct->Q;
	
	for(int i=0; i<inputstruct->N; ++i,bufP++)
		fscanf(fstream, "%i", bufP);

	for(int i=0; i<inputstruct->M; ++i,bufQ++)
		fscanf(fstream, "%i", bufQ);
}

inline int guider(path_node **n_ps_Pointer, indata *inputstruct){
	int ressize = 0;
	int ressizecopy = 0;
	int *bufP, *bufQ, minimum;
	path_node* localpointer;
	
	ressize = inputstruct->N + inputstruct->M - 1;
	ressizecopy=ressize;

	*n_ps_Pointer = (path_node*)malloc(ressize*sizeof(path_node));
	localpointer = *n_ps_Pointer;
	
	bufP = inputstruct->P;
	bufQ = inputstruct->Q;
	
	for(int i=0; i< inputstruct->N; ++i,bufP++){
		for(int j=0; j<inputstruct->M; ++j,bufQ++){
			minimum = min((*bufP),(*bufQ));
			if(minimum != 0){
				(*bufP)-=minimum;
				(*bufQ)-=minimum;
				if(ressizecopy != 0){
					localpointer->from = i+1;
					localpointer->where = j+1;
					localpointer->amount = minimum;
					if(ressizecopy != 1)
						localpointer++;
					ressizecopy--;
				}
				else return WRONG_PATH;
			}
		}
		bufQ = inputstruct->Q;
	}
	
	return ressize;
}

inline int min(int a, int b){
	return a < b ? a : b;
}

inline void path_to_file(path_node *n_ps_Pointer, int size, FILE *outfile){
	path_node* localpointer;

	localpointer = n_ps_Pointer;
	
	for(int i = 0; i < size; ++i,localpointer++)
		fprintf(outfile,"%i %i %i\n", localpointer->from, localpointer->where, localpointer->amount);
	
}

int main(){
	/*
		infile - pointer to infile
		outfile - pointer to outfile
	*/
	FILE *infile, *outfile;
	indata Bdata;
	path_node *nodes;

	
	infile = NULL;
	outfile = NULL;
	nodes=NULL;
	
	infile = fopen ("input.txt","r");
	outfile = fopen ("output.txt","w");
	
	
	loader(&Bdata, infile);
	int size = guider(&nodes, &Bdata);

	
	path_to_file(nodes, size, outfile);
	
	fclose(infile);
	fclose(outfile);
}