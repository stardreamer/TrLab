#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE !TRUE
#define MEM_ERROR -2
#define UNABLE_TO_FIND_PATH -3

typedef struct{
	/*
		from - sender id
		where - receiver id
	*/	
	int from, where;
} path_node;

typedef struct{
	/*
		i0 - start point P
		j0 - start point Q
		N - number of sources
		M - number of recievers
		bplen - length of basic path
		plen - length of new path
		ulen -length of used
		cunode - current node
		used - array of freedom
		path - new path
		basicpath - basic path
	*/	
	int i0,j0;
	int M,N,bplen,plen,ulen;
	int *used;
	path_node curnode;
	path_node *path;
	path_node *basicpath;
} cycle;

int goH(cycle* cl, path_node node); //go vertical
int goV(cycle* cl, path_node node); //go horizontal
inline int reductor(int i,int j,int l) __attribute__((always_inline)); 
inline void loader(cycle *inputstruct, FILE *fstream) __attribute__((always_inline)); //load data from file
inline int addNode(cycle* cl, path_node *node); //resize and add
inline void path_to_file(path_node *n_ps_Pointer, int size, FILE *outfile) __attribute__((always_inline)); //write results to file


inline void loader(cycle *inputstruct, FILE *fstream){
	path_node *bufPath;
	fscanf(fstream, "%i %i", &(inputstruct->N), &(inputstruct->M));
	fscanf(fstream, "%i %i", &(inputstruct->i0), &(inputstruct->j0));
	
	inputstruct->i0--;
	inputstruct->j0--;
	inputstruct->curnode.from = inputstruct->i0;
	inputstruct->curnode.where = inputstruct->j0;
	inputstruct->plen = 0;
	inputstruct->bplen = (inputstruct->M + inputstruct->N - 1);
	inputstruct->ulen = inputstruct->M * inputstruct->N;
	inputstruct->basicpath = (path_node*)malloc(inputstruct->bplen*sizeof(path_node));
	inputstruct->used = (int*)calloc(inputstruct->ulen, sizeof(int));
	bufPath=inputstruct->basicpath;

	for(int i=0; i < inputstruct->bplen; ++i,bufPath++){
		fscanf(fstream, "%i %i", &(bufPath->from), &(bufPath->where));
		bufPath->from--;
		bufPath->where--;
		inputstruct->used[reductor(bufPath->from,bufPath->where,inputstruct->M)] = 1;
	}
	
	inputstruct->used[reductor(inputstruct->i0,inputstruct->j0,inputstruct->M)] = 1;

}

int goH(cycle* cl, path_node node){	
	cl->used[reductor(node.from, node.where, cl->M)] = 2;
	
	for(int j = 0;j < cl->M; ++j){
		if(j != node.where ){
			if (j == cl->j0 && node.from == cl->i0){
				addNode(cl, &(node));
				return TRUE;
			}
			
			if(cl->used[reductor(node.from,j,cl->M)] == 1){
				cl->curnode.where = j;
				if (goV(cl, cl->curnode)){
					addNode(cl, &(node));
					return TRUE;
				}
			}
		}
		
	}
	
	
	cl->used[reductor(node.from, node.where, cl->M)] = 1;
	cl->curnode = node;
	return FALSE;
}

int goV(cycle* cl, path_node node){
	cl->used[reductor(node.from, node.where, cl->M)] = 2;

	for(int i = 0;i < cl->N; ++i){
		if(i != node.from){
			cl->curnode.from = i;
			if (i == cl->i0 && node.where == cl->j0){
				addNode(cl, &(node));
				return TRUE;
			}
			if(cl->used[reductor(i,node.where,cl->M)] == 1){
				if (goH(cl, cl->curnode)){
					cl->curnode.from = i;
					addNode(cl, &(node));
					return TRUE;
				}
			}
		}
		
	}

	cl->used[reductor(node.from, node.where, cl->M)] = 1;

	cl->curnode = node;
	return FALSE;
}

inline int addNode(cycle* cl, path_node *node){
	
	if(cl->plen == 0){
		cl->path = (path_node*)malloc(sizeof(path_node));
		if (cl->path == NULL)
			return MEM_ERROR;
	}
	else{
		path_node* localpath = NULL;
	
		localpath = (path_node*)realloc(cl->path,(cl->plen+1)*sizeof(path_node));
		if (localpath!=NULL)
			cl->path=localpath;
		else
			return MEM_ERROR;
	}
	
	cl->path[cl->plen] = *node;
	
	cl->plen++;
	
	return cl->plen;
}

inline void path_to_file(path_node *n_ps_Pointer, int size, FILE *outfile){
	path_node* localpointer;

	localpointer = n_ps_Pointer;
	
	for(int i = 0; i < size; ++i,localpointer++)
		fprintf(outfile,"%i %i\n", localpointer->from+1, localpointer->where+1);
	
}

inline int reductor(int i,int j,int l){
	return (l*i+j);
}

int main(){
	
	FILE *infile, *outfile;
	cycle curcyc;
	
	infile = fopen ("input.txt","r");
	outfile = fopen ("output.txt","w");
	
	loader(&(curcyc), infile);
	if(!goH(&(curcyc), curcyc.curnode)){
		free(curcyc.basicpath);
		free(curcyc.path);
		free(curcyc.used);	
		fclose(infile);
		fclose(outfile);
		return UNABLE_TO_FIND_PATH;
	}
	else
		path_to_file(curcyc.path, curcyc.plen, outfile);
		
	free(curcyc.basicpath);
	free(curcyc.path);
	free(curcyc.used);	
	fclose(infile);
	fclose(outfile);
	
	return 0;
}