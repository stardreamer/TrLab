#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#define TRUE 42
#define FALSE !TRUE
#define MEM_ERROR -2
#define UNABLE_TO_FIND_PATH -3

typedef struct{
	/*
		from - sender id
		where - receiver id
	*/	
	int from, where, amount;
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

int goH(cycle* cl, path_node node); //go horizontal
int goV(cycle* cl, path_node node); //go vertical
inline int reductor(int i,int j,int l) __attribute__((always_inline)); 
inline int balancer(cycle* cl) __attribute__((always_inline)); 
inline void loader(cycle *inputstruct, FILE *fstream) __attribute__((always_inline)); //load data from file
inline int addNode(cycle* cl, path_node *node); //resize and add
inline void path_to_file(cycle *cl, FILE *outfile) __attribute__((always_inline)); //write results to file
inline path_node* findnode(path_node *cl, int len, int from, int where) __attribute__((always_inline));

inline void loader(cycle *inputstruct, FILE *fstream){
	path_node *bufPath;
	fscanf(fstream, "%i %i", &(inputstruct->N), &(inputstruct->M));
	fscanf(fstream, "%i %i", &(inputstruct->i0), &(inputstruct->j0));
	
	inputstruct->i0--;
	inputstruct->j0--;
	inputstruct->curnode.from = inputstruct->i0;
	inputstruct->curnode.where = inputstruct->j0;
	inputstruct->curnode.amount = 0;
	inputstruct->plen = 0;
	inputstruct->bplen = (inputstruct->M + inputstruct->N - 1);
	inputstruct->ulen = inputstruct->M * inputstruct->N;
	inputstruct->basicpath = (path_node*)malloc(inputstruct->bplen*sizeof(path_node));
	inputstruct->used = (int*)calloc(inputstruct->ulen, sizeof(int));
	bufPath=inputstruct->basicpath;

	for(int i=0; i < inputstruct->bplen; ++i,bufPath++){
		fscanf(fstream, "%i %i %i", &(bufPath->from), &(bufPath->where), &(bufPath->amount));
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
				cl->curnode = *findnode(cl->basicpath, cl->bplen, node.from, j);
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
			if (i == cl->i0 && node.where == cl->j0){
				addNode(cl, &(node));
				return TRUE;
			}
			if(cl->used[reductor(i,node.where,cl->M)] == 1){
				cl->curnode = *findnode(cl->basicpath, cl->bplen, i, node.where);
				if (goH(cl, cl->curnode)){
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

inline void path_to_file(cycle *cl, FILE *outfile){
	path_node* local;	

	for(int i = 0; i< cl->ulen; ++i){
		if(i%cl->M==0 && i!=0)
			fprintf(outfile,"\n");
		if(cl->used[i]==2){
			local = findnode(cl->path,cl->plen,i/cl->M,i%cl->M);
			if(local != NULL)
				fprintf(outfile,"%i ", (*local).amount);
			else
				fprintf(outfile,"%i ", 5);
			
		}
		else	
			fprintf(outfile,"%i ", 0);
	}
	
}

inline int reductor(int i,int j,int l){
	return (l*i+j);
}

inline path_node* findnode(path_node *cl, int len, int from, int where) {

	for(int i=0;i<len;++i){
		if(cl[i].from == from && cl[i].where == where)
			return cl+i;
	}

	return NULL;
}

inline int balancer(cycle* cl){
	int minimum = INT_MAX;

	for(int i=cl->plen-1, j=0; i>=0, j<cl->plen; i--, j++){
		if(j%2!=0)
			if(cl->path[i].amount < minimum)
				minimum = cl->path[i].amount;

	}

	for(int i=cl->plen-1, j=0; i>=0, j<cl->plen; i--, j++){
		if(j%2==0)
			cl->path[i].amount+= minimum;
		else
			cl->path[i].amount-= minimum;
	}
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

	balancer(&(curcyc));
	path_to_file(&curcyc, outfile);
		
	free(curcyc.basicpath);
	free(curcyc.path);
	free(curcyc.used);	
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
