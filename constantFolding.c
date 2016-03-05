void expression(expr_rec *result)
{
	expr_rec left_operand, right_operand;
	op_rec op;
	int valor=0;

	primary(& left_operand);
	while(next_token() == PLUSOP || next_token() == MINUSOP)
	{
		add_op(& op);
		primary(& right_operand);
		if (left_operand.kind==LITERALEXPR && right_operand.kind==LITERALEXPR){
			if(op.operator==MINUS){
				if(valor==0){
					valor = left_operand.val-right_operand.val;
				}
				else{
					valor = valor-right_operand.val;
				}
			}
			else{
				if(valor==0){
					valor = left_operand.val+right_operand.val;;
				}
				else{
					valor = valor+right_operand.val;
				}
			}
		}
		left_operand = gen_infix(left_operand, op, right_operand);
	}
	*result= left_operand;
}