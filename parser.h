#ifndef CXMLP_PARSER_H_
#define CXMLP_PARSER_H_

#include "node.h"
#include "lexer.h"
#include "steel/stack.h"

#define Line_Capacity 1024

Token *Cxmlp_GetToken(Steel_LL *tokens) {
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

static Steel_Stack *stack;

static XmlNode *Cxmlp_ParseXmlNode(Steel_LL *tokens) {
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
		Steel_LL_PopFront(tokens); // eat 'iden'
		Steel_LL_Push(node->attributes, attribute, sizeof(Attribute));
	}
	if (Cxmlp_GetToken(tokens)->kind == TokenKind_TagClose) {
		Steel_Stack_Push(stack, node, sizeof(XmlNode));
	} else {
		Steel_Node *top = Steel_Stack_Top(stack);
		if (!top) {
			// TODO: raise error
		}
		XmlNode *parent = (XmlNode *)top->data;
		Steel_LL_Push(parent->childs, node, sizeof(XmlNode));
		node->parent = parent;
	}
	return node;
}

XmlNode *Cxmlp_Parse(Steel_LL *tokens);

XmlNode *Cxmlp_Parse(Steel_LL *tokens) {
	stack = Steel_Stack_Init();
	XmlNode *root = Cxmlp_ParseXmlNode(tokens);
	return root;
}

#endif // CXMLP_PARSER_H_
