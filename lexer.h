#ifndef CXMLP_LEXER_H_
#define CXMLP_LEXER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "steel/ll.h"
#include "steel/string.h"

typedef struct Loc {
	char *filepath;
	size_t row;
	size_t col;
} Loc;

#define Line_Capacity 1024

void Cxmlp_Error(Loc loc, char *message);
void Cxmlp_Error_Fmt(Loc loc, char *format, ...);
Loc Cxmlp_Loc_Init(char *filepath, size_t row, size_t col);
	
void Cxmlp_Error(Loc loc, char *message) {
	printf("%s:%d:%d: error: %s\n", loc.filepath, loc.row, loc.col, message);
	exit(EXIT_FAILURE);
}

void Cxmlp_Error_Fmt(Loc loc, char *format, ...) {
	va_list args;
    va_start(args, format);
    printf("%s:%d:%d: error: ", loc.filepath, loc.row, loc.col);
    vprintf(format, args);
    printf("\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

Loc Cxmlp_Loc_Init(char *filepath, size_t row, size_t col) {
	return (Loc){filepath: strdup(filepath), row: row, col: col};
}

typedef enum {
	TokenKind_TagOpen,
	TokenKind_TagSlashOpen,
	TokenKind_TagClose,
	TokenKind_TagSlashClose,
	TokenKind_Iden,
	TokenKind_Eq,
	TokenKind_String,
} TokenKind;

char *Cxmlp_TokenKind_ToString(TokenKind kind);

char *Cxmlp_TokenKind_ToString(TokenKind kind) {
	switch (kind) {
	case TokenKind_TagOpen: return "TokenKind_TagOpen";
	case TokenKind_TagSlashOpen: return "TokenKind_TagOpen";
	case TokenKind_TagClose: return "TokenKind_TagOpen";
	case TokenKind_TagSlashClose: return "TokenKind_TagOpen";
	case TokenKind_Iden: return "TokenKind_TagOpen";
	case TokenKind_Eq: return "TokenKind_TagOpen";
	case TokenKind_String: return "TokenKind_TagOpen";
	}
}

typedef struct Token {
	TokenKind kind;
	char *text;
	Loc loc;
} Token;

#define Cxmlp_Tokens_Push(tokens, token) (Steel_LL_Push(tokens, token, sizeof(Token)))

void Cxmlp_Token_Print(Token token);
void Cxmlp_Tokens_Print(Steel_LL *tokens);
Steel_LL *Cxmpl_Tokenize(char *filepath);

void Cxmlp_Token_Print(Token token) {
	printf("%s:%d:%d: ", token.loc.filepath, token.loc.row, token.loc.col);
	switch (token.kind) {
	case TokenKind_TagOpen:
		printf("<\n");
		break;
	case TokenKind_TagSlashOpen:
		printf("</\n");
		break;
	case TokenKind_TagClose:
		printf(">\n");
		break;
	case TokenKind_TagSlashClose:
		printf("/>\n");
		break;
	case TokenKind_Iden:
		printf("%s\n", token.text);
		break;
	case TokenKind_Eq:
		printf("=\n");
		break;
	case TokenKind_String: 
		printf("\"%s\"\n", token.text);
		break;
	}
}

void Cxmlp_Tokens_Print(Steel_LL *tokens) {
	Steel_Node *itr = tokens->head;
	while (itr) {
		Cxmlp_Token_Print(*(Token *)itr->data);
		itr = itr->next;
	}
}

Steel_LL *Cxmlp_Tokenize(char *filepath) {
	FILE *file = fopen(filepath, "r");
    if (!file) Cxmlp_Error_Fmt((Loc){filepath: filepath, row: 0, col: 0},
						 "could not open file '%s'", filepath);
	char *line = (char *)malloc(sizeof(char)*Line_Capacity);
	size_t row = 0;
	Steel_LL *tokens = Steel_LL_Init();
    while (fgets(line, Line_Capacity, file)) {
		row++;
		size_t col = 1;
		while (*line) {
			col += Steel_String_Trim(&line);
			Token* token = (Token *)malloc(sizeof(Token));
			token->loc = Cxmlp_Loc_Init(filepath, row, col);
			if (Steel_String_StartsWith(line, "</")) {
				token->kind = TokenKind_TagSlashOpen;
				Cxmlp_Tokens_Push(tokens, token);
				col += 2;
				Steel_String_EatFront(&line, 2);
				continue;
			}
			if (Steel_String_StartsWith(line, "<")) {
				token->kind = TokenKind_TagOpen;
				Cxmlp_Tokens_Push(tokens, token);
				col += 1;
				Steel_String_EatFront(&line, 1);
				continue;
			}
			if (Steel_String_StartsWith(line, "/>")) {
				token->kind = TokenKind_TagSlashClose;
				Cxmlp_Tokens_Push(tokens, token);
				col += 2;
				Steel_String_EatFront(&line, 2);
				continue;
			}
			if (Steel_String_StartsWith(line, ">")) {
				token->kind = TokenKind_TagClose;
				Cxmlp_Tokens_Push(tokens, token);
				col += 1;
				Steel_String_EatFront(&line, 1);
				continue;
			}
			if (Steel_String_StartsWith(line, "=")) {
				token->kind = TokenKind_Eq;
				Cxmlp_Tokens_Push(tokens, token);
				col += 1;
				Steel_String_EatFront(&line, 1);
				continue;
			}
			if (isalnum(*line)) {
				token->text = Steel_String_FetchIf(line, isalnum);
				token->kind = TokenKind_Iden;
				Cxmlp_Tokens_Push(tokens, token);
				col += strlen(token->text);
				Steel_String_EatFront(&line, strlen(token->text));
				continue;
			}
			if (*line == '\"') {
				Steel_String_EatFront(&line, 1);
				token->text = Steel_String_FetchNEatUntil(&line, '\"');
				token->kind = TokenKind_String;
				Cxmlp_Tokens_Push(tokens, token);
				col += strlen(token->text)+2;
				Steel_String_EatFront(&line, 1);
				continue;
			}
			Cxmlp_Error_Fmt(token->loc, "invalid token '%s'", line);
		}
	}
	return tokens;
}

#endif // CXMLP_LEXER_H_
