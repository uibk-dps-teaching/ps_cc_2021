// AST Visitor Infrastructure
//
// This module defines a visitor utility for traversing the AST.
//
// Instantiate the `mcc_ast_visitor` struct with the desired configuration and
// callbacks. Use this instance with the functions declared below. Each
// callback is optional, just set it to NULL.

#ifndef MCC_AST_VISIT_H
#define MCC_AST_VISIT_H

#include "mcc/ast.h"

enum mcc_ast_visit_traversal {
	MCC_AST_VISIT_DEPTH_FIRST,
	// TODO: MCC_AST_VISIT_BREADTH_FIRST,
};

enum mcc_ast_visit_order {
	MCC_AST_VISIT_PRE_ORDER,
	MCC_AST_VISIT_POST_ORDER,
};

// Callbacks
typedef void (*mcc_ast_visit_expression_cb)(struct mcc_ast_expression *, void *userdata);
typedef void (*mcc_ast_visit_literal_cb)(struct mcc_ast_literal *, void *userdata);

struct mcc_ast_visitor {
	enum mcc_ast_visit_traversal traversal;
	enum mcc_ast_visit_order order;

	// This will be passed to every callback along with the corresponding AST
	// node. Use it to share data while traversing the tree.
	void *userdata;

	mcc_ast_visit_expression_cb expression;
	mcc_ast_visit_expression_cb expression_literal;
	mcc_ast_visit_expression_cb expression_binary_op;
	mcc_ast_visit_expression_cb expression_parenth;

	mcc_ast_visit_literal_cb literal;
	mcc_ast_visit_literal_cb literal_int;
	mcc_ast_visit_literal_cb literal_float;
};

void mcc_ast_visit_expression(struct mcc_ast_expression *expression, struct mcc_ast_visitor *visitor);

void mcc_ast_visit_literal(struct mcc_ast_literal *literal, struct mcc_ast_visitor *visitor);

#endif // MCC_AST_VISIT_H
