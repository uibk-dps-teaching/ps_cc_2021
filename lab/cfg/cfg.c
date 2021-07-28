#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------- TAC Definition

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

bool tac_is_jump(enum tac_instruction_op op)
{
	return op == TAC_INSTRUCTION_OP_JUMP || op == TAC_INSTRUCTION_OP_JUMP_IF;
}

struct tac_function {
	char name[256];
	struct tac_instruction *start;
};

struct tac_instruction {
	enum tac_instruction_op op;
	uint64_t label;
	struct tac_instruction *next;
};

void append_instruction(struct tac_function *function, struct tac_instruction instruction)
{
	assert(function);

	struct tac_instruction **end = &function->start;
	while (*end) {
		end = &(*end)->next;
	}

	*end = malloc(sizeof(**end));
	assert(*end);

	**end = instruction;
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

struct tac_function example_function()
{
	struct tac_function function = {
	    .name = "example",
	};

	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_POP});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_CONST});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_CONST});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ADD});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ADD});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_JUMP_IF, .label = 1});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ADD});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ASSIGN});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_JUMP, .label = 2});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_LABEL, .label = 1});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ADD});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_ASSIGN});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_LABEL, .label = 2});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_PUSH});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_PUSH});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_PUSH});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_CALL});
	append_instruction(&function, (struct tac_instruction){.op = TAC_INSTRUCTION_OP_RETURN});

	return function;
}

// -------------------------------------------------------------------- CFG Definition

struct cfg_basic_block_instruction {
	struct tac_instruction *instruction;
	struct cfg_basic_block_instruction *next;
};

struct cfg_basic_block {
	struct cfg_basic_block_instruction *instructions;
	struct cfg_basic_block *next;
};

struct cfg_edge {
	struct cfg_basic_block *from, *to;
	struct cfg_edge *next;
};

struct cfg_graph {
	struct cfg_basic_block *first_basic_block;
	struct cfg_edge *first_edge;
};

void add_instruction(struct cfg_basic_block *block, struct tac_instruction *instruction)
{
	assert(block);
	assert(instruction);

	struct cfg_basic_block_instruction **end = &block->instructions;
	while (*end) {
		end = &(*end)->next;
	}

	*end = malloc(sizeof(**end));
	assert(*end);

	**end = (struct cfg_basic_block_instruction){.instruction = instruction};
}

void clear_instructions_from_basic_block(struct cfg_basic_block *block)
{
	assert(block);
	struct cfg_basic_block_instruction *current = block->instructions;
	while (current) {
		struct cfg_basic_block_instruction *to_free = current;
		current = current->next;
		free(to_free);
	}
}

struct cfg_basic_block *add_basic_block(struct cfg_graph *graph)
{
	assert(graph);

	struct cfg_basic_block **end = &graph->first_basic_block;
	while (*end) {
		end = &(*end)->next;
	}

	*end = malloc(sizeof(**end));
	**end = (struct cfg_basic_block){};
	return *end;
}

void add_edge(struct cfg_graph *graph, struct cfg_basic_block *from, struct cfg_basic_block *to)
{
	assert(graph);
	assert(from);
	assert(to);

	struct cfg_edge **end = &graph->first_edge;
	while (*end) {
		end = &(*end)->next;
	}

	*end = malloc(sizeof(**end));
	assert(*end);

	**end = (struct cfg_edge){.from = from, .to = to};
}

void clear_graph(struct cfg_graph *graph)
{
	assert(graph);

	{
		struct cfg_basic_block *current = graph->first_basic_block;
		while (current) {
			struct cfg_basic_block *to_free = current;
			current = current->next;
			clear_instructions_from_basic_block(to_free);
			free(to_free);
		}
	}

	{
		struct cfg_edge *current = graph->first_edge;
		while (current) {
			struct cfg_edge *to_free = current;
			current = current->next;
			free(to_free);
		}
	}
}

// -------------------------------------------------------------------- CFG Generation

struct cfg_graph gen_graph(struct tac_function *function)
{
	assert(function);

	struct cfg_graph graph = {};

	struct cfg_basic_block *block = add_basic_block(&graph);

	struct cfg_basic_block *jump_targets[256] = {};

	for (struct tac_instruction *instruction = function->start; instruction; instruction = instruction->next) {
		if (instruction->op == TAC_INSTRUCTION_OP_LABEL) {
			// only split block if empty
			if (block->instructions) {
				struct cfg_basic_block *next_block = add_basic_block(&graph);
				add_edge(&graph, block, next_block);
				block = next_block;
			}

			// register as jump target
			jump_targets[instruction->label] = block;
		}

		add_instruction(block, instruction);

		if (instruction->op == TAC_INSTRUCTION_OP_JUMP_IF) {
			struct cfg_basic_block *next_block = add_basic_block(&graph);
			add_edge(&graph, block, next_block);
			block = next_block;
		}

		else if (instruction->op == TAC_INSTRUCTION_OP_JUMP) {
			block = add_basic_block(&graph);
		}
	}

	// wire up jumps and labels
	for (struct cfg_basic_block *block = graph.first_basic_block; block; block = block->next) {
		for (struct cfg_basic_block_instruction *inst = block->instructions; inst; inst = inst->next) {
			if (tac_is_jump(inst->instruction->op)) {
				add_edge(&graph, block, jump_targets[inst->instruction->label]);
			}
		}
	}

	return graph;
}

// -------------------------------------------------------------------- CFG Print

void cfg_print(struct cfg_graph *graph)
{
	assert(graph);

	puts("digraph \"CFG\" {");
	puts("\tnodesep=0.6\n");

	for (struct cfg_basic_block *block = graph->first_basic_block; block; block = block->next) {
		printf("\t\"%p\" [shape=box, label=\"", block);

		for (struct cfg_basic_block_instruction *inst = block->instructions; inst; inst = inst->next) {
			printf("%s\\n", tac_instruction_op_to_string(inst->instruction->op));
		}

		puts("\"];");
	}

	for (struct cfg_edge *edge = graph->first_edge; edge; edge = edge->next) {
		printf("\t\"%p\" -> \"%p\";\n", edge->from, edge->to);
	}

	puts("}");
}

// --------------------------------------------------------------------

int main(void)
{
	struct tac_function example = example_function();
	struct cfg_graph graph = gen_graph(&example);

	cfg_print(&graph);

	clear_graph(&graph);
	clear_instructions(&example);

	return EXIT_SUCCESS;
}
