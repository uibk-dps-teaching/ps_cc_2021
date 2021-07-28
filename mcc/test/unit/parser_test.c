#include <CuTest.h>

#include "mcc/ast.h"
#include "mcc/parser.h"

// Threshold for floating point comparisions.
static const double EPS = 1e-3;

void BinaryOp_1(CuTest *tc)
{
	const char input[] = "192 + 3.14";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_NONE, result.error);

	struct mcc_ast_expression *expr = result.expression;

	// root
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, expr->op);

	// root -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	CuAssertIntEquals(tc, 192, expr->lhs->literal->i_value);

	// root -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);

	// root -> rhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_FLOAT, expr->rhs->literal->type);
	CuAssertDblEquals(tc, 3.14, expr->rhs->literal->f_value, EPS);

	mcc_ast_delete_expression(expr);
}

void NestedExpression_1(CuTest *tc)
{
	const char input[] = "42 * (192 + 3.14)";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_NONE, result.error);

	struct mcc_ast_expression *expr = result.expression;

	// root
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_MUL, expr->op);

	// root -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->type);

	// root -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->lhs->literal->type);
	CuAssertIntEquals(tc, 42, expr->lhs->literal->i_value);

	// root -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_PARENTH, expr->rhs->type);

	struct mcc_ast_expression *subexpr = expr->rhs->expression;

	// subexpr
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, subexpr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, subexpr->op);

	// subexpr -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->lhs->type);

	// subexpr -> lhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, subexpr->lhs->literal->type);
	CuAssertIntEquals(tc, 192, subexpr->lhs->literal->i_value);

	// subexpr -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, subexpr->rhs->type);

	// subexpr -> rhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_FLOAT, subexpr->rhs->literal->type);
	CuAssertIntEquals(tc, 3.14, subexpr->rhs->literal->f_value);

	mcc_ast_delete_expression(expr);
}

void NestedExpression_2(CuTest *tc)
{
	const char input[] = "(42) + 21";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_NONE, result.error);

	struct mcc_ast_expression *expr = result.expression;

	// root
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, expr->op);

	// root -> lhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_PARENTH, expr->lhs->type);

	// root -> lhs -> expression
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->expression->type);

	// root -> lhs -> expression -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->lhs->expression->literal->type);
	CuAssertIntEquals(tc, 42, expr->lhs->expression->literal->i_value);

	// root -> rhs
	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->rhs->type);

	// root -> rhs -> literal
	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->rhs->literal->type);
	CuAssertIntEquals(tc, 21, expr->rhs->literal->i_value);

	mcc_ast_delete_expression(expr);
}

void MissingClosingParenthesis_1(CuTest *tc)
{
	const char input[] = "(42";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_PARSE_ERROR, result.error);
	CuAssertTrue(tc, NULL == result.expression);
}

void SourceLocation_SingleLineColumn(CuTest *tc)
{
	const char input[] = "(42 + 192)";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_NONE, result.error);

	struct mcc_ast_expression *expr = result.expression;

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_PARENTH, expr->type);
	CuAssertIntEquals(tc, 1, expr->node.sloc.column);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->expression->type);
	CuAssertIntEquals(tc, 2, expr->expression->node.sloc.column);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->expression->lhs->type);
	CuAssertIntEquals(tc, 2, expr->expression->lhs->node.sloc.column);

	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->expression->lhs->literal->type);
	CuAssertIntEquals(tc, 2, expr->expression->lhs->literal->node.sloc.column);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->expression->rhs->type);
	CuAssertIntEquals(tc, 7, expr->expression->rhs->node.sloc.column);

	CuAssertIntEquals(tc, MCC_AST_LITERAL_TYPE_INT, expr->expression->rhs->literal->type);
	CuAssertIntEquals(tc, 7, expr->expression->rhs->literal->node.sloc.column);

	mcc_ast_delete_expression(expr);
}

void Precedence_1(CuTest *tc)
{
	const char input[] = "1 + 2 * 3 + 4";
	struct mcc_parser_result result = mcc_parse_string(input);

	CuAssertIntEquals(tc, MCC_PARSER_ERROR_NONE, result.error);

	struct mcc_ast_expression *expr = result.expression;

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, expr->op);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->lhs->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_ADD, expr->lhs->op);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_BINARY_OP, expr->lhs->rhs->type);
	CuAssertIntEquals(tc, MCC_AST_BINARY_OP_MUL, expr->lhs->rhs->op);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->rhs->lhs->type);
	CuAssertIntEquals(tc, 2, expr->lhs->rhs->lhs->literal->i_value);

	CuAssertIntEquals(tc, MCC_AST_EXPRESSION_TYPE_LITERAL, expr->lhs->rhs->rhs->type);
	CuAssertIntEquals(tc, 3, expr->lhs->rhs->rhs->literal->i_value);

	mcc_ast_delete_expression(expr);
}

#define TESTS \
	TEST(BinaryOp_1) \
	TEST(NestedExpression_1) \
	TEST(NestedExpression_2) \
	TEST(MissingClosingParenthesis_1) \
	TEST(SourceLocation_SingleLineColumn) \
	TEST(Precedence_1)

#include "main_stub.inc"
