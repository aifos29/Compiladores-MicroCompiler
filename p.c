//Scanner funcionando.No tocar:clear buffer,buffer_char,lexical error o check reserved.
//Parsear match existe, falta next_token(le toca)
//Para compilar gcc p.c
//Le tira un note y un warning. Deje asi,si no es error si compila
//Arraste el archivo a.out que se crea a la terminal y asi se ejecuta
//Be happy. Lo he hecho va comentado


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
//#include <vector.h>


/*character classification macros */
//2.1,2.2.2.3 Complete Scanner Function for Micro
typedef enum token_types{
	BEGIN,END,READ,WRITE,ID,INTLITERAL,
	LPAREN,RPAREN,SEMICOLON,COMMA,ASSIGNOP,
	PLUSOP,MINUSOP,SCANEOF
} token;

char token_buffer[2000];

/*Descrito por el libro, la idea es guardar el token actual*/
token current_token=SCANEOF;

					
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
void lexical_error(int x){
	//Funcion sumamente importante. Muestra error.
	printf("Caracter no encontrado %d\n",x);
}

token check_reserved(){
	//Revise el token_buffer y si este es una palabra reservada retorna el token al que pertenece

	if ((strcmp(token_buffer,"READ")==0) || (strcmp(token_buffer,"read")==0)){
		return READ;}
	if ((strcmp(token_buffer,"WRITE")==0) || (strcmp(token_buffer,"write")==0)){
		return WRITE;}
	if ((strcmp(token_buffer,"BEGIN")==0) || (strcmp(token_buffer,"begin")==0)){
		return BEGIN;}
	if ((strcmp(token_buffer,"END")==0) || (strcmp(token_buffer,"END")==0)){
		return END;
	}
	else{
	 return ID;
  }
}


	



token scanner(void)
{
	int in_char,c;

	clear_buffer();
	if (feof(stdin))
		return SCANEOF;

	while ((in_char = getchar())!= EOF){
		if (isspace(in_char))
			continue; /*do nothing */
		else if (isalpha(in_char)){
			/*
				ID::=LETTER | ID LETTER
							| ID DIGIT
							| ID UNDERSCORE


			*/
			buffer_char(in_char);
			for (c= getchar();isalnum(c)||c=='-';c=getchar())
				buffer_char(c);
				ungetc(c,stdin);
				return check_reserved();

		}else if (isdigit(in_char)){
			/*
				INTLITERAL :: = DIGIT |
								INTLITERAL DIGIT
			*/
			buffer_char(in_char);
			for (c=getchar();isdigit(c);c=getchar())
				buffer_char(c);
			ungetc(c,stdin);
			return INTLITERAL;
		}else if (in_char== '(')
			return LPAREN;
		else if (in_char==')')
			return RPAREN;
		else if (in_char==';')
			return SEMICOLON ;
		else if (in_char==',')
			return COMMA ;
		else if (in_char=='+')
			return  PLUSOP;
		else if (in_char==':'){
			/*looking for ":="*/
			c=getchar();
			if (c=='=')
				return ASSIGNOP ;
			else{
				ungetc(c,stdin);
				lexical_error(in_char);
			}		
}
	
	else if (in_char=='-'){
		/*is it --;comment start */
		c=getchar();
		if (c=='-'){
			do 
				in_char=getchar();
			while (in_char!='\n');

		} else{
			ungetc(c,stdin);
			return MINUSOP;
		}
		}else 
			lexical_error(in_char);
	}
}

void syntax_error(token t){

}

void match(token t){
//Crea un token auxiliar que llama al scanner.Compara el auxiliar con lo que recibe. Si son iguales, lo almacena en current_token
    token auxiliar=scanner();
    if (auxiliar==t){
    	current_token=t;
    }
    else{
    	printf("%s\n","Error de sintaxis");
    }

}

token next_token(){

}

void id_list(void)
{
	/*<id list> ::= ID{ , ID} */
	match(ID);

	while (next_token() == COMMA) {
		match(COMMA);
		match(ID);
	}
}

void add_op(void)
{
	token tok = next_token();

	/* <addop> ::= PLUSOP | MINUSOP*/
	if (tok == PLUSOP || tok == MINUSOP)
		match(tok);
	else
		syntax_error(tok);
}

void primary(void)
{
	token tok = next_token();

	switch (tok){
		case LPAREN:
			/*<primary> ::= (<expression>) */
			match(LPAREN); expression();
			match(RPAREN);
			break;

		case ID:
			/*<primary> ::= ID*/
			match(ID);
			break;

		case INTLITERAL:
			/*<primary> ::= INTLITERAL*/
			match(INTLITERAL);
			break;

		default:
		syntax_error(tok);
		break;
	}
}


void expression(void)
{
	token t;
	/*<expression> ::= <primary>
						{<add op><primary>} */
	primary();
	for (t = next_token(); t == PLUSOP || t == MINUSOP; t = next_token())
	{
		add_op();
		primary();
	}
}





/*Figure 2.7  Remaining Parsing Procedures for Macro*/

void expr_list(void)
{
	/*<expr list> ::= <expression> { , <expression> }*/
	expression();

	while(next_token() == COMMA)
	{
		match(COMMA);
		expression();
	}
}


void statement(void)
{
	token tok = next_token();

	switch (tok){
		case ID:
			/*<statement> ::= ID := <expression>; */
			match(ID); match(ASSIGNOP);
			expression(); match(SEMICOLON);
			break;

		case READ:
			/*<statement> ::= READ (<id list>); */
			match(READ); match(LPAREN);
			id_list(); match(RPAREN);
			match(SEMICOLON);
			break;

		case WRITE:
			/*<statement> ::= WRITE (<expr list>); */
			match(WRITE); match(LPAREN);
			expr_list(); match(RPAREN);
			match(SEMICOLON);
			break;

		default:
			syntax_error(tok);
			break;
	}
}

void statement_list(void){
	/*
		<statement list> ::=<statement>
								{<statement>}
	*/

	statement();
	while (1){
		switch (next_token()){
			case ID:
			case READ:
			case WRITE:
				statement();
				break;
			default:
				return;
			
		}
    }
}



void program(void){
	/*<program> ::= BEGIN <statement list> END*/
	match(BEGIN);
	statement_list();
	match(END);
}



void system_goal(void){
	/*<sytem goal> := <program> SCANEOF */
	program();
	match(SCANEOF);
}





int main(int argc, char const *argv[])
{
	//Esta aquí para no tirar error
	return 0;
}