#ifndef CXMLP_PARSER_H_
#define CXMLP_PARSER_H_

#include <string.h>
#include "node.h"
#include "lexer.h"
#include "steel/stack.h"

#define Line_Capacity 1024

XmlNode *Cxmlp_Parse(Steel_LL *tokens);

static Steel_Stack *stack;

static Token *Cxmlp_GetToken(Steel_LL *tokens) {
	return (Token *)(tokens->head->data);
}

static void Cxmlp_AssertTokenKind(Steel_LL *tokens, TokenKind kind) {
	if (Steel_LL_Empty(tokens)) {
		Cxmlp_Error_Fmt((Loc){filepath:"", row: 0, col: 0},
						"expected token of kind '%s'",
						Cxmlp_TokenKind_ToString(kind));
	}
	Token *token = Cxmlp_GetToken(tokens);
	if (token->kind != kind) {
		Cxmlp_Error_Fmt(token->loc, "expected token of kind '%s'; got '%s'",
						Cxmlp_TokenKind_ToString(kind), Cxmlp_TokenKind_ToString(token->kind));
	}
}

static XmlNode *Cxmlp_ParseTagOpen(Steel_LL *tokens) {
	XmlNode *node = Cxmlp_XmlNode_Init();
	Steel_LL_PopFront(tokens); // eat '<'
	Cxmlp_AssertTokenKind(tokens, TokenKind_Iden);
	node->name = strdup(Cxmlp_GetToken(tokens)->text);
	Steel_LL_PopFront(tokens); // eat 'iden'
	while (Cxmlp_GetToken(tokens)->kind != TokenKind_TagSlashClose
		   && Cxmlp_GetToken(tokens)->kind != TokenKind_TagClose) {
		Attribute *attribute = (Attribute *)malloc(sizeof(Attribute));
		Cxmlp_AssertTokenKind(tokens, TokenKind_Iden);
		attribute->name = strdup(Cxmlp_GetToken(tokens)->text);
		Steel_LL_PopFront(tokens); // eat 'iden'
		Cxmlp_AssertTokenKind(tokens, TokenKind_Eq);
		Steel_LL_PopFront(tokens); // eat '='
		Cxmlp_AssertTokenKind(tokens, TokenKind_String);
		attribute->value = strdup(Cxmlp_GetToken(tokens)->text);
		Steel_LL_PopFront(tokens); // eat 'string'
		Steel_LL_PushBack(node->attributes, attribute, sizeof(Attribute));
	}
	if (Cxmlp_GetToken(tokens)->kind == TokenKind_TagClose) {
		Steel_Stack_Push(stack, node, sizeof(XmlNode));
	} else {
		Steel_Node *top = Steel_Stack_Peek(stack);
		if (!top && !Steel_LL_Empty(tokens)) {
			Token *token = Cxmlp_GetToken(tokens);
			Cxmlp_Error_Fmt(token->loc, "expected EOF as the top level node "
							"is already closed; but got '%s'",
							Cxmlp_TokenKind_ToString(token->kind));
		}
		XmlNode *parent = (XmlNode *)top->data;
		Steel_LL_PushBack(parent->childs, node, sizeof(XmlNode));
		node->parent = parent;
	}
	Steel_LL_PopFront(tokens); // eat '>' or '/>'
	return node;
}

static void Cxmlp_ParseTagSlashOpen(Steel_LL *tokens) {
	Steel_LL_PopFront(tokens); // eat '</'
	Cxmlp_AssertTokenKind(tokens, TokenKind_Iden);
	Token *token = Cxmlp_GetToken(tokens);
	char *name = strdup(token->text);
	Steel_LL_PopFront(tokens); // eat 'iden'
	Cxmlp_AssertTokenKind(tokens, TokenKind_TagClose);
	Steel_LL_PopFront(tokens); // eat '>'
	Steel_Node *top = Steel_Stack_Peek(stack);
	XmlNode *node = (XmlNode *)top->data;
	if (!top) {
		Cxmlp_Error_Fmt(token->loc, "unexpected tag '</%s>'; nothing to close", name);
	}
	if (strcmp(node->name, name) != 0) {
		Cxmlp_Error_Fmt(token->loc, "tag name mismatch expected name '%s'; got '%s'",
						node->name, name);
	}
	Steel_Stack_PopNotFree(stack);
	top = Steel_Stack_Peek(stack);
	if (!top) {
		if (Steel_LL_Empty(tokens)) return;
		Token *token = Cxmlp_GetToken(tokens);
		Cxmlp_Error_Fmt(token->loc, "expected EOF as the top level node "
						"is already closed; but got '%s'",
						Cxmlp_TokenKind_ToString(token->kind));
	}
	XmlNode *parent = (XmlNode *)top->data;
	Steel_LL_PushBack(parent->childs, node, sizeof(XmlNode));	
	node->parent = parent;
}

XmlNode *Cxmlp_Parse(Steel_LL *tokens) {
	stack = Steel_Stack_Init();
	XmlNode *root = Cxmlp_ParseTagOpen(tokens);
	while (!Steel_LL_Empty(tokens)) {
		Token *token = Cxmlp_GetToken(tokens);
		switch (token->kind) {
		case TokenKind_TagOpen:
			Cxmlp_ParseTagOpen(tokens);
			break;
		case TokenKind_TagSlashOpen:
			Cxmlp_ParseTagSlashOpen(tokens);
			break;
		default:
			Cxmlp_Error_Fmt(token->loc, "unexpected token kind '%s'; expected '<' or '</'",
							Cxmlp_TokenKind_ToString(token->kind));
		}
	}
	return root;
}

#endif // CXMLP_PARSER_H_
