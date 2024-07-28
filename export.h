#ifndef CXMLP_EXPORT_H_
#define CXMLP_EXPORT_H_

#include "lexer.h"
#include "parser.h"

XmlNode *Cxmlp_ParseFile(char *filepath) {
	Steel_LL *tokens = Cxmlp_Tokenize(filepath);
	/* Cxmlp_Tokens_Print(tokens); */
	return Cxmlp_Parse(tokens);
}

#endif // CXMLP_EXPORT_H_
