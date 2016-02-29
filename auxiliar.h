#include <stdio.h>


#define MAXIDLEN 33
typedef char string[MAXIDLEN];


char *table[100];
int dim=0;
int i;


int lookup(string s){
	int ret=0;
	for (i=0;i<dim;i++){
    	
		if (strcmp(s,table[i])==0){

    		return 1;
    	}
    	else{
    		return 0;
    	}
    }
}

void enter(string s){

	table[dim]=strdup(s);
	dim=dim +1;

}

void imprimir(){

	for (i=0;i<dim;i++){

		printf("%s",table[i]);
	}
}
