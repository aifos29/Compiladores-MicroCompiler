#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char token_buffer[200];
FILE *archivo;
FILE *salida;

typedef enum token_types{
	ADD,SUB,DECLARE,STORE,MOVE,HALT,FIN,VAR
} token;

//Variable para saber el temporal
int val_temp;
//Tablas con variables
char * variables[200];
int con_var=0;

void clear_buffer(void){
	//Borra el buffer de token buffer.
	memset(token_buffer,'\0',strlen(token_buffer));
}

void buffer_char(int x){
	//Convierte el entero a caracter y lo agrega al token_buffer
	if (strlen(token_buffer)==0){
		char dato=(char)x;
		char auxiliar[]={(char)x,'\0'};
		strcat(token_buffer,auxiliar);
	}
	else{
	char dato=(char)x;
	 char auxiliar[]={dato,'\0'};
 	 strcat(token_buffer,auxiliar);
	}
}

token check_reserved(){
	//Revise el token_buffer y si este es una palabra reservada retorna el token al que pertenece

	if (strcmp(token_buffer,"Add")==0){
		return ADD;}
	if (strcmp(token_buffer,"Sub")==0){
		return SUB;}
	if (strcmp(token_buffer,"Declare")==0){
		return DECLARE;}
	if (strcmp(token_buffer,"Store")==0){
		return STORE;
	}
	if (strcmp(token_buffer,"Move")==0){
		return MOVE;
	}
	else{
	 return VAR;
  }
}



token scanner(void)
{
	int in_char,c;

	clear_buffer();
	if (feof(archivo))
		return FIN;

	while (feof(archivo)==0){
		in_char=fgetc(archivo);
		if (isspace(in_char))
			continue; /*do nothing */
		else if (isalpha(in_char)){
			buffer_char(in_char);
			for (c= fgetc(archivo);isalnum(c)||c=='-'||c=='&';c=fgetc(archivo))
				if (c!=','){
					if (c=='&'){
				    	c=fgetc(archivo);
						val_temp = c;
						return VAR;
					}
				buffer_char(c);
			  }
				else{
					return check_reserved();
				}
				//ungetc(c,stdin);
				return check_reserved();

		}else if (isdigit(in_char)){
			buffer_char(in_char);
			for (c=fgetc(archivo);isdigit(c)||c==',';c=fgetc(archivo))
			 	if (c!=','){
				buffer_char(c);
			}
				else{
					return check_reserved();
				}

}
   }
}

int buscar_temp(){
	/*
		Revisa el token buffer en busca de temporales, asigna 0 si es Temp o 1 si no lo es
	*/
		if (strcmp(token_buffer,"Temp")==0){
			return 0;
		}
		else{
			return 1;
		}
	
}

void crear_temporal(char * x){
	if (buscar_temp()==0){
		char num[10];
		char registro[10];
		sprintf(registro,"%d",val_temp);
		sprintf(num,"$t%s",registro);
		strcpy(x,num);
    }
	else{
		strcpy(x,token_buffer);
	}

}

void operacion(int op){
	
	char operando1[33];
	scanner();
	crear_temporal(&operando1);
	//strcpy(operando1,token_buffer);
	char operando2[33];
	scanner();
	crear_temporal(&operando2);
	char registro[33];
	scanner();
	crear_temporal(&registro);
	

	printf("estoy entrando a escribir");
	fflush(stdout);

	if (op==0){
		printf("estoy escribiendo");
					fflush(stdout);
		fprintf(salida,"%s %s %s %s\n","sub",registro,operando1,operando2);
		printf("termine de escribri");
			fflush(stdout);


	}
		else{
			fprintf(salida,"%s %s %s %s\n","add",registro,operando1,operando2 );
			;
           }
     fflush(stdout);
}
void declare(){
	//Llama al scanner, revisa el token buffer, si no es Temp lo mete en una lista
	scanner();
	if (strcmp(token_buffer,"Temp")!=0){
		variables[con_var] = strdup(token_buffer);
		fflush(stdout);
		con_var = con_var +1;
	}


	
}
void main(){
	archivo=fopen("res.txt","r+");
	
	
	salida=fopen("prueba.txt","w+");
	token palabras = scanner();
	
	while (palabras!=FIN){
		switch(palabras){
			case DECLARE:
				declare();
				break;
			case SUB:
				operacion(0);
				break;
			case ADD:
				operacion(1);
				break;
			case HALT:
				break;
			case MOVE:
				break;
			case STORE:
				break;
			default:
				break;

		}
		palabras=scanner();


	}
}








