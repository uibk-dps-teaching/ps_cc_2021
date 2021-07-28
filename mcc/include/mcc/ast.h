// Abstract Syntax Tree (AST)
//
// Here we define the AST data structure of the compiler. It mainly consists of
// *tagged unions* for node types and enums for operators.
//
// In addition to the node type specific members, each node features a common
// member `mmc_ast_node` which serves as a *base-class*. It holds data
// independent from the actual node type, like the source location.
//
// Also note that this makes excessive use of C11's *anonymous structs and
// unions* feature.

#ifndef MCC_AST_H
#define MCC_AST_H

#include "mcc/sloc.h"

// Forward Declarations
struct mcc_ast_expression;
struct mcc_ast_literal;

// ------------------------------------------------------------------- AST Node

struct mcc_ast_node {
	struct mcc_sloc sloc;
};

// ------------------------------------------------------------------ Operators

enum mcc_ast_binary_op {
	MCC_AST_BINARY_OP_ADD,
	MCC_AST_BINARY_OP_SUB,
	MCC_AST_BINARY_OP_MUL,
	MCC_AST_BINARY_OP_DIV,
};

// ---------------------------------------------------------------- Expressions

enum mcc_ast_expression_type {
	MCC_AST_EXPRESSION_TYPE_LITERAL,
	MCC_AST_EXPRESSION_TYPE_BINARY_OP,
	MCC_AST_EXPRESSION_TYPE_PARENTH,
};

struct mcc_ast_expression {
	struct mcc_ast_node node;

	enum mcc_ast_expression_type type;
	union {
		// MCC_AST_EXPRESSION_TYPE_LITERAL
		struct mcc_ast_literal *literal;

		// MCC_AST_EXPRESSION_TYPE_BINARY_OP
		struct {
			enum mcc_ast_binary_op op;
			struct mcc_ast_expression *lhs;
			struct mcc_ast_expression *rhs;
		};

		// MCC_AST_EXPRESSION_TYPE_PARENTH
		struct mcc_ast_expression *expression;
	};
};

struct mcc_ast_expression *mcc_ast_new_expression_literal(struct mcc_ast_literal *literal);

struct mcc_ast_expression *mcc_ast_new_expression_binary_op(enum mcc_ast_binary_op op,
                                                            struct mcc_ast_expression *lhs,
                                                            struct mcc_ast_expression *rhs);

struct mcc_ast_expression *mcc_ast_new_expression_parenth(struct mcc_ast_expression *expression);

void mcc_ast_delete_expression(struct mcc_ast_expression *expression);

// ------------------------------------------------------------------- Literals

enum mcc_ast_literal_type {
	MCC_AST_LITERAL_TYPE_INT,
	MCC_AST_LITERAL_TYPE_FLOAT,
};

struct mcc_ast_literal {
	struct mcc_ast_node node;

	enum mcc_ast_literal_type type;
	union {
		// MCC_AST_LITERAL_TYPE_INT
		long i_value;

		// MCC_AST_LITERAL_TYPE_FLOAT
		double f_value;
	};
};

struct mcc_ast_literal *mcc_ast_new_literal_int(long value);

struct mcc_ast_literal *mcc_ast_new_literal_float(double value);

void mcc_ast_delete_literal(struct mcc_ast_literal *literal);

#endif // MCC_AST_H
