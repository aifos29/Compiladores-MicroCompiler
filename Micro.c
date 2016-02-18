#include <stdio.h>
#include <ctype.h>
/*character classification macros */
//2.1,2.2.2.3 Complete Scanner Function for Micro
extern char token_buffer[];

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
			for (c= getchar();isalnum(c)||c=='_';c=getchar())
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
		else if (in_char==':')
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
		if (c=='_'){
			do 
				in_char=getchar();
			while (in_char!='\n');

		} else{
			ungetc(c,stdin);
			return MINUSOP;
		}else 
			lexical_error(in_char);
	}
}


/*Figure 2.7  Remaining Parsing Procedures for Macro*/
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

void id_list(void)
{
	/*<id list> ::= ID{ , ID} */
	match(ID);

	while (next_token() == COMMA) {
		match(COMMA);
		match(ID);
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
			math(INTLITERAL);
			break;

		default:
		syntax_error(tok);
		break;
	}
}

/*Figure 2.8 Semantic Records for Micro Grammar Symbols*/

#define MAXIDLEN
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


/**/

/*Is s in the symbol table?*/
extern int lookup(string s);

/*Put s unconditionally into symbol table*/
extern void enter(string s);

void check_id(string s)
{
	if(!lookup(s)){
		enter(s);
		generate("Declare",s,"Integer","");
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

/*Figure 2.10*/
void start(void)
{
	/*Semantic initializations,none needed. */
}

void finish(void)
{
	/*Generate code to finish program. */
	generate("Halt","","","");
}

void assign(expr_rec target, expr_rec source)
{
	/*Generate code for assigment. */
	generate("Store",extract(source), target.name,"");
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

expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
	expr_rec e_rec;
	/*An expr_rec with temp variant set.*/
	e_rec.kind = TEMPEXPR;

	/*Generate code for infix operation.
		Get result temp and set up semantic record for result*/
	strcpy(e_rec.name,get_temp());
	generate(extract(op), extract(e1), extract(e2), e_rec.name);
	return e_rec;
}

void read_id(expr_rec in_var)
{
	/*Generate code for read*/
	generate("Read",in_var.name,"Integer","");
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

expr_rec process_literal(void)
{
	expr_rec t;

	/*Convert literal to a numeric representation and build semantic record*/
	t.kind = LITERALEXPR;
	(void) sscanf(token_buffer,"%d", & t.val);
	return t;
}

void write_expr(expr_rec out_expr)
{
	generate("Write", extract(out_expr),"Integer","");
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
	*result = left_operand;
}