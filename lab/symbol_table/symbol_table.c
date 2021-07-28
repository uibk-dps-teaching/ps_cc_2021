#include <stdlib.h>

// ------------------------------------------------------------------- Type

enum mcc_type_kind {
	MCC_TYPE_BOOL,
	MCC_TYPE_INT,
	MCC_TYPE_FLOAT,
	MCC_TYPE_STRING,
	MCC_TYPE_BOOL_ARRAY,
	MCC_TYPE_INT_ARRAY,
	MCC_TYPE_FLOAT_ARRAY,
	MCC_TYPE_STRING_ARRAY,
};

struct mcc_type {
	enum mcc_type_kind kind;
	size_t array_size;
};

// ------------------------------------------------------------------- AST Declaration

enum mcc_ast_statement_type {
	MCC_AST_STATEMENT_TYPE_DECLARATION,
};

struct mcc_ast_statement {
	enum mcc_ast_statement_type type;
	union {
		// MCC_AST_STATEMENT_TYPE_DECLARATION
		struct {
			const char *identifier;
			struct mcc_type identifier_type;
		};

        // ...
	};
};

// ------------------------------------------------------------------- Symbol Table

struct mcc_symbol_table_entry {
	const char *identifier;
	struct mcc_ast_statement *declaration;
};

struct mcc_symbol_table {
	struct mcc_symbol_table_entry *entries;
	size_t entries_count;
	size_t entries_capacity;
};

// Creation: Traverse the AST using the visitor pattern, looking for
// declarations. Each decleration yields a new entry in our symbol table.

// Supporting scopes:
// - Add scope enter/exit visitor callbacks
// - Add parent pointer to symbol table
