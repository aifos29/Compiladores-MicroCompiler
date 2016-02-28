#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliar.h"



FILE *fp;
 	
 	
 	
#define MAXIDLEN 33
typedef char string[MAXIDLEN];

typedef struct operator
{
	/* for operators */
	enum op{ PLUS, MINUS } operator;
}op_rec;

/*expression types*/
enum expr { IDEXPR, LITERALEXPR, TEMPEXPR};

/* for <primary> and <expression>*/
typedef struct expression
{
	enum expr kind;
	union{
		string name; /*for IDEXPR,TEMPEXPR*/
		int val; /*for LITERALEXPR*/
	};
}expr_rec;

/*character classification macros */
//2.1,2.2.2.3 Complete Scanner Function for Micro
typedef enum token_types{
	BEGIN,END,READ,WRITE,ID,INTLITERAL,
	LPAREN,RPAREN,SEMICOLON,COMMA,ASSIGNOP,
	PLUSOP,MINUSOP,SCANEOF
} token;

char token_buffer[200];
FILE *archivo;
/*Descrito por el libro, la idea es guardar el token actual*/
token current_token=SCANEOF;
token next;
int flag=0; //0=falso
int flag_next_token=0;
					
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
	if ((strcmp(token_buffer,"END")==0) || (strcmp(token_buffer,"end")==0)){
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
	if (feof(archivo))
		return SCANEOF;

	while (feof(archivo)==0){
		in_char=fgetc(archivo);
		if (isspace(in_char))
			continue; /*do nothing */
		else if (isalpha(in_char)){
			/*
				ID::=LETTER | ID LETTER
							| ID DIGIT
							| ID UNDERSCORE


			*/
			buffer_char(in_char);
			for (c= fgetc(archivo);isalnum(c)||c=='-';c=fgetc(archivo))
				buffer_char(c);
				//ungetc(c,stdin);
				return check_reserved();

		}else if (isdigit(in_char)){
			/*
				INTLITERAL :: = DIGIT |
								INTLITERAL DIGIT
			*/
			buffer_char(in_char);
			for (c=fgetc(archivo);isdigit(c);c=fgetc(archivo))
				buffer_char(c);
			//ungetc(c,stdin);
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
			c=fgetc(archivo);
			if (c=='=')
				return ASSIGNOP ;
			else{
				//ungetc(c,stdin);
				lexical_error(in_char);
			}		
}
	
	else if (in_char=='-'){
		/*is it --;comment start */
		c=fgetc(archivo);
		if (c=='-'){
			do 
				in_char=fgetc(archivo);
			while (in_char!='\n');

		} else{
			//ungetc(c,stdin);
			return MINUSOP;
		}
		}else 
			lexical_error(in_char);
	}
}

void syntax_error(token t){
	printf("%s","Error de sintaxis");
}

void match(token t){
//Crea un token auxiliar que llama al scanner.Compara el auxiliar con lo que recibe. Si son iguales, lo almacena en current_token
	
	if (flag_next_token==0){
	token auxiliar=scanner();
    if (auxiliar==t){
    	current_token=t;
    	flag_next_token=0;
    	flag=1; //Cambio la bandera
    }
    else{
    	flag=0;
    }
}
else{
	if (next==t){
		current_token=t;
		flag_next_token=0;
		flag=1;
	}
	else{
		flag=0;
	}

}

}

token next_token(){
	token aux;

	if (flag==1){
		flag_next_token=1;
		flag=0; // No se ha hecho match
		next=scanner();
	}
	
	return next;
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

op_rec process_op(void)
{
	/*Produce operator descriptor. */
	op_rec o;

	if (current_token == PLUSOP)
		o.operator = PLUS;
	else
		o.operator = MINUS;
	return o;
}

void add_op(op_rec * rec)
{
	token tok = next_token();

	/* <addop> ::= PLUSOP | MINUSOP*/
	if (tok == PLUSOP || tok == MINUSOP){
		match(tok);
		//op_rec x = process_op();
		*rec = process_op();
	}
	else{
		syntax_error(tok);
}}

expr_rec process_literal(void)
{
	expr_rec t;
	check_reserved(token_buffer);

	/*Convert literal to a numeric representation and build semantic record*/
	t.kind = LITERALEXPR;
	(void) sscanf(token_buffer,"%d", & t.val);
	return t;
}
expr_rec process_id(void)
{
	expr_rec t;
	/*Declare ID and built a corresponding semantic record*/
	check_id(token_buffer);
	t.kind = IDEXPR;
	strcpy(t.name, token_buffer);
	return t;
}

void primary(expr_rec * x)
{
	token tok = next_token();

	switch (tok){
		case LPAREN:
			/*<primary> ::= (<expression>) */
			match(LPAREN); 
            
			expression(x);
			match(RPAREN);
			break;

		case ID:
			/*<primary> ::= ID*/
			match(ID);
			expr_rec p;
			p=process_id();
			*x=p;
			break;

		case INTLITERAL:
			/*<primary> ::= INTLITERAL*/
			match(INTLITERAL);
			expr_rec y;
			y=process_literal();
			*x=y;
			break;

		default:
		syntax_error(tok);
		break;
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


char * extract2(expr_rec x){
	if (x.kind==LITERALEXPR){
		char * valor[MAXIDLEN];
		sprintf(*valor,"%d",x.val);
        return * valor;
	}
	else{
		char  * val[MAXIDLEN];

		strcpy(*val,x.name);
		return * val;
	}

}

void assign(expr_rec target, expr_rec source)
{
	/*Generate code for assigment. */
	if (source.kind==LITERALEXPR){
		generate("Store",extract2(source), target.name,"");
	}
	else{
		generate("Store",source.name, target.name,"");
	}



}

void statement(void)
{
	token tok = next_token();
	expr_rec source,target;

	switch (tok){
		case ID:
			/*<statement> ::= ID := <expression>; */
			match(ID);
			target = process_id();
			match(ASSIGNOP);
			expression(& source); 
			match(SEMICOLON);
			assign(target,source);
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


void finish(void)
{
	/*Generate code to finish program. */
	generate("Halt","","","");
}


void program(void){
	/*<program> ::= BEGIN <statement list> END*/
	match(BEGIN);
	statement_list();
	match(END);
	finish();
	finish();
}



void system_goal(void){
	/*<sytem goal> := <program> SCANEOF */
	program();
	match(SCANEOF);
}






int main(int argc, char const *argv[])
{
	printf("%s\n","hola");
	archivo=fopen("archivo.txt","r");
	
	system_goal();

	return 0;
}
//Código nuevo del translate
/*Figure 2.8 Semantic Records for Micro Grammar Symbols*/



/**/

/*Is s in the symbol table?*/
extern int lookup(string s);

/*Put s unconditionally into symbol table*/
extern void enter(string s);
	


void generate(string dec,string s,string tipo,string espacio){
	printf("%s\n ","");
	printf("%s ",dec);
 	printf("%s ",s);
 	printf("%s ",tipo);
 	printf("%s ",espacio);

 

}


char * extract(op_rec x){
	if (x.operator == PLUS){
		char *ret[MAXIDLEN];
		*ret ="add";
		return * ret;
	}
	else{
		char * ret[MAXIDLEN];
		*ret="sub";
		return * ret;
	}


}



char *get_temp(void)
{
	/*max temporary allocated so far*/
	static int max_temp = 0;
	static char tempname[MAXIDLEN];

	max_temp++;
	sprintf(tempname,"Temp&%d",max_temp);
	check_id(tempname);
	return tempname;
}

expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
	expr_rec e_rec;
	/*An expr_rec with temp variant set.*/
	e_rec.kind = TEMPEXPR;

	/*Generate code for infix operation.
		Get result temp and set up semantic record for result*/
	strcpy(e_rec.name,get_temp());

	char uno[33];

	strcpy(uno,extract2(e2));
    generate(extract(op), extract2(e1),uno, e_rec.name);
	return e_rec;
}





void check_id(string s)
{
	if(lookup(s)==0){
		enter(s);
		generate("Declare",s,"Integer","");
	}
}



/*Figure 2.10*/
void start(void)
{
	/*Semantic initializations,none needed. */
}










void read_id(expr_rec in_var)
{
	/*Generate code for read*/
	generate("Read",in_var.name,"Integer","");
}





void write_expr(expr_rec out_expr)
{
	generate("Write", extract2(out_expr),"Integer","");
}

/*Figure 2.11 A Parsing Procedure Including Semanntic Processing*/
void expression(expr_rec *result)
{
	expr_rec left_operand, right_operand;
	op_rec op;

	primary(& left_operand);
	while(next_token() == PLUSOP || next_token() == MINUSOP)
	{
		add_op(& op);
		primary(& right_operand);
		left_operand = gen_infix(left_operand, op, right_operand);
	}

	*result= left_operand;



}
