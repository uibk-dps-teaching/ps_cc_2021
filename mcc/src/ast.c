#include "mcc/ast.h"

#include <assert.h>
#include <stdlib.h>

// ---------------------------------------------------------------- Expressions

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal)
{
	assert(literal);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	*expr = (struct mcc_ast_expression){
	    .type = MCC_AST_EXPRESSION_TYPE_LITERAL,
	    .literal = literal,
	};
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs)
{
	assert(lhs);
	assert(rhs);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	*expr = (struct mcc_ast_expression){
	    .type = MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	    .op = op,
	    .lhs = lhs,
	    .rhs = rhs,
	};
	return expr;
}

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression)
{
	assert(expression);

	struct mcc_ast_expression *expr = malloc(sizeof(*expr));
	if (!expr) {
		return NULL;
	}

	*expr = (struct mcc_ast_expression){
	    .type = MCC_AST_EXPRESSION_TYPE_PARENTH,
	    .expression = expression,
	};
	return expr;
}

void mcc_ast_delete_expression(struct mcc_ast_expression *expression)
{
	if (!expression) {
		return;
	}

	switch (expression->type) {
	case MCC_AST_EXPRESSION_TYPE_LITERAL:
		mcc_ast_delete_literal(expression->literal);
		break;

	case MCC_AST_EXPRESSION_TYPE_BINARY_OP:
		mcc_ast_delete_expression(expression->lhs);
		mcc_ast_delete_expression(expression->rhs);
		break;

	case MCC_AST_EXPRESSION_TYPE_PARENTH:
		mcc_ast_delete_expression(expression->expression);
		break;
	}

	free(expression);
}

// ------------------------------------------------------------------- Literals

struct mcc_ast_literal *mcc_ast_new_literal_int(long value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	*lit = (struct mcc_ast_literal){
	    .type = MCC_AST_LITERAL_TYPE_INT,
	    .i_value = value,
	};
	return lit;
}

struct mcc_ast_literal *mcc_ast_new_literal_float(double value)
{
	struct mcc_ast_literal *lit = malloc(sizeof(*lit));
	if (!lit) {
		return NULL;
	}

	*lit = (struct mcc_ast_literal){
	    .type = MCC_AST_LITERAL_TYPE_FLOAT,
	    .f_value = value,
	};
	return lit;
}

void mcc_ast_delete_literal(struct mcc_ast_literal *literal)
{
	free(literal);
}
