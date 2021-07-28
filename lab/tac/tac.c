#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------- TAC Definition

enum tac_variable_type {
	TAC_VARIABLE_TYPE_VOID = 0,
	TAC_VARIABLE_TYPE_INT,
	TAC_VARIABLE_TYPE_FLOAT,
};

const char *tac_variable_type_to_string(enum tac_variable_type type)
{
	switch (type) {
	case TAC_VARIABLE_TYPE_VOID:
		return "void";
	case TAC_VARIABLE_TYPE_INT:
		return "int";
	case TAC_VARIABLE_TYPE_FLOAT:
		return "float";
	}

	assert(false);
	return "INVALID";
}

struct tac_variable {
	enum tac_variable_type type;
	uint64_t identifier;
};

void tac_variable_print(const struct tac_variable *variable)
{
	assert(variable);
	if (variable->type == TAC_VARIABLE_TYPE_VOID) {
		printf("unused");
	} else {
		printf("v%ld", variable->identifier);
	}
}

enum tac_instruction_op {
	TAC_INSTRUCTION_OP_CONST,

	TAC_INSTRUCTION_OP_ASSIGN,

	// Arithmetic operations
	TAC_INSTRUCTION_OP_ADD,
	// ...

	// Control flow
	TAC_INSTRUCTION_OP_LABEL,
	TAC_INSTRUCTION_OP_JUMP,
	TAC_INSTRUCTION_OP_JUMP_IF,

	// Function calls
	TAC_INSTRUCTION_OP_CALL,
	TAC_INSTRUCTION_OP_RETURN,
	TAC_INSTRUCTION_OP_PUSH,
	TAC_INSTRUCTION_OP_POP,
};

const char *tac_instruction_op_to_string(enum tac_instruction_op op)
{
	switch (op) {
	case TAC_INSTRUCTION_OP_CONST:
		return "CONST";
	case TAC_INSTRUCTION_OP_ASSIGN:
		return "ASSIGN";
	case TAC_INSTRUCTION_OP_ADD:
		return "ADD";
	case TAC_INSTRUCTION_OP_LABEL:
		return "LABEL";
	case TAC_INSTRUCTION_OP_JUMP:
		return "JUMP";
	case TAC_INSTRUCTION_OP_JUMP_IF:
		return "JUMP_IF";
	case TAC_INSTRUCTION_OP_CALL:
		return "CALL";
	case TAC_INSTRUCTION_OP_RETURN:
		return "RETURN";
	case TAC_INSTRUCTION_OP_PUSH:
		return "PUSH";
	case TAC_INSTRUCTION_OP_POP:
		return "POP";
	}

	assert(false);
	return "INVALID";
}

struct tac_function {
	const char name[256];

	// Using a singly linked list here for convenience, although it is super
	// inefficient.
	struct tac_instruction *start;
};

struct tac_instruction {
	enum tac_instruction_op op;
	struct tac_variable arg1, arg2, result;

	union {
		long int_constant;
		double float_constant;
	};

	uint64_t label;
	const char *function;

	struct tac_instruction *next;
};

// -------------------------------------------------------------------- TAC Generator

struct tac_variable gen_variable(enum tac_variable_type type)
{
	static uint64_t variable_index = 1;

	return (struct tac_variable){
	    .type = type,
	    .identifier = variable_index++,
	};
}

struct tac_instruction *gen_label(void)
{
	static uint64_t label_index = 1;

	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_LABEL,
	    .label = label_index++,
	};
	return result;
}

struct tac_instruction *gen_const_int(long value)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_CONST,
	    .result = gen_variable(TAC_VARIABLE_TYPE_INT),
	    .int_constant = value,
	};
	return result;
}

struct tac_instruction *gen_const_float(double value)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_CONST,
	    .result = gen_variable(TAC_VARIABLE_TYPE_FLOAT),
	    .float_constant = value,
	};
	return result;
}

struct tac_instruction *gen_assign(struct tac_variable lhs, struct tac_variable rhs)
{
	assert(lhs.type == rhs.type);

	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_ASSIGN,
	    .arg1 = rhs,
	    .result = lhs,
	};
	return result;
}

struct tac_instruction *gen_add(struct tac_variable lhs, struct tac_variable rhs)
{
	assert(lhs.type == rhs.type);

	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_ADD,
	    .arg1 = lhs,
	    .arg2 = rhs,
	    .result = gen_variable(lhs.type),
	};
	return result;
}

struct tac_instruction *gen_jump(uint64_t label)
{
	assert(label != 0);

	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_JUMP,
	    .label = label,
	};
	return result;
}

struct tac_instruction *gen_jump_if(uint64_t label, struct tac_variable condition)
{
	assert(label != 0);
	assert(condition.type == TAC_VARIABLE_TYPE_INT);

	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_JUMP_IF,
	    .arg1 = condition,
	    .label = label,
	};
	return result;
}

struct tac_instruction *gen_call(const char *function, enum tac_variable_type result_type)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_CALL,
	    .result = gen_variable(result_type),
	    .function = function,
	};
	return result;
}

struct tac_instruction *gen_return()
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_RETURN,
	};
	return result;
}

struct tac_instruction *gen_return_variable(struct tac_variable arg1)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_RETURN,
	    .arg1 = arg1,
	};
	return result;
}

struct tac_instruction *gen_push(struct tac_variable variable)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_PUSH,
	    .arg1 = variable,
	};
	return result;
}

struct tac_instruction *gen_pop(enum tac_variable_type result_type)
{
	struct tac_instruction *result = malloc(sizeof(*result));
	assert(result);

	*result = (struct tac_instruction){
	    .op = TAC_INSTRUCTION_OP_POP,
	    .result = gen_variable(result_type),
	};
	return result;
}

void append_instruction(struct tac_function *function, struct tac_instruction *instruction)
{
	assert(function);
	assert(instruction);

	struct tac_instruction **end = &function->start;
	while (*end) {
		end = &(*end)->next;
	}

	*end = instruction;
}

void clear_instructions(struct tac_function *function)
{
	assert(function);

	struct tac_instruction *current = function->start;
	while (current) {
		struct tac_instruction *to_free = current;
		current = current->next;
		free(to_free);
	}
}

void print_instructions(struct tac_function *function)
{
	assert(function);

	printf("==== %s ====\n", function->name);

	for (struct tac_instruction *instruction = function->start; instruction; instruction = instruction->next) {
		printf("%s\t", tac_instruction_op_to_string(instruction->op));
		if (instruction->result.type != TAC_VARIABLE_TYPE_VOID) {
			tac_variable_print(&instruction->result);
			printf("\t%s\t", tac_variable_type_to_string(instruction->result.type));
		} else {
			printf("\t\t");
		}

		switch (instruction->op) {
		case TAC_INSTRUCTION_OP_CONST:
			if (instruction->result.type == TAC_VARIABLE_TYPE_INT) {
				printf("%ld", instruction->int_constant);
			} else {
				printf("%f", instruction->float_constant);
			}
			break;

		case TAC_INSTRUCTION_OP_ADD:
			tac_variable_print(&instruction->arg1);
			printf("\t");
			tac_variable_print(&instruction->arg2);
			break;

		case TAC_INSTRUCTION_OP_LABEL:
		case TAC_INSTRUCTION_OP_JUMP:
			printf("L_%ld", instruction->label);
			break;

		case TAC_INSTRUCTION_OP_JUMP_IF:
			printf("L_%ld\t", instruction->label);
			tac_variable_print(&instruction->arg1);
			break;

		case TAC_INSTRUCTION_OP_CALL:
			printf("%s", instruction->function);
			break;

		case TAC_INSTRUCTION_OP_RETURN:
			if (instruction->arg1.type != TAC_VARIABLE_TYPE_VOID) {
				tac_variable_print(&instruction->arg1);
			}
			break;

		case TAC_INSTRUCTION_OP_ASSIGN:
		case TAC_INSTRUCTION_OP_PUSH:
			tac_variable_print(&instruction->arg1);
			break;

		case TAC_INSTRUCTION_OP_POP:
			break;

		default:
			assert(false);
			break;
		}

		printf("\n");
	}
}

// --------------------------------------------------------------------

int main(void)
{
	struct tac_function my_function = {
	    .name = "My TAC Function",
	};

	struct tac_instruction *inst1 = gen_pop(TAC_VARIABLE_TYPE_INT);
	append_instruction(&my_function, inst1);

	struct tac_instruction *c1 = gen_const_int(1);
	append_instruction(&my_function, c1);

	struct tac_instruction *c2 = gen_const_int(2);
	append_instruction(&my_function, c2);

	struct tac_instruction *inst2 = gen_add(c1->result, c2->result);
	append_instruction(&my_function, inst2);

	struct tac_instruction *inst3 = gen_add(inst2->result, inst1->result);
	append_instruction(&my_function, inst3);

	struct tac_instruction *l1 = gen_label();
	struct tac_instruction *inst4 = gen_jump_if(l1->label, inst3->result);
	append_instruction(&my_function, inst4);

	struct tac_instruction *inst5 = gen_add(inst3->result, c1->result);
	append_instruction(&my_function, inst5);
	append_instruction(&my_function, gen_assign(inst3->result, inst5->result));

	struct tac_instruction *l2 = gen_label();
	struct tac_instruction *inst7 = gen_jump(l2->label);
	append_instruction(&my_function, inst7);

	append_instruction(&my_function, l1);

	struct tac_instruction *inst8 = gen_add(inst3->result, c2->result);
	append_instruction(&my_function, inst8);
	append_instruction(&my_function, gen_assign(inst3->result, inst8->result));

	append_instruction(&my_function, l2);

	append_instruction(&my_function, gen_push(c2->result));
	append_instruction(&my_function, gen_push(c1->result));
	append_instruction(&my_function, gen_push(inst3->result));

	struct tac_instruction *inst9 = gen_call("some_function", TAC_VARIABLE_TYPE_FLOAT);
	append_instruction(&my_function, inst9);

	append_instruction(&my_function, gen_return_variable(inst9->result));

	print_instructions(&my_function);

	clear_instructions(&my_function);
	return EXIT_SUCCESS;
}
