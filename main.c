#include "parser.h"

int main() {
	Steel_LL *tokens = Cxmlp_Tokenize("./voyage.xml");
    /* Cxmlp_Tokens_Print(tokens); */
	XmlNode *root = Cxmlp_Parse(tokens);
	Cxmlp_XmlNode_Print(root);
	return 0;
}
