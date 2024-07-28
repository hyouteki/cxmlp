#ifndef CXMLP_NODE_H_
#define CXMLP_NODE_H_

#include <stdlib.h>
#include "steel/ll.h"

typedef struct Attribute {
	char *name;
	char *value;
} Attribute;

typedef struct XmlNode {
	char *name;
	Steel_LL *attributes;
	Steel_LL *childs;
	struct XmlNode *parent;
} XmlNode;

XmlNode *Cxmlp_XmlNode_Init();
void Cxmlp_XmlNode_Print(XmlNode *node);

static void Cxmlp_XmlNode_PrintIndent(size_t indent) {
	for (size_t i = 0; i < indent; ++i) printf("  ");
}

static void Cxmlp_XmlNode_Print_(XmlNode *node, size_t indent) {
	if (!node) return;
	Cxmlp_XmlNode_PrintIndent(indent);
	printf("<%s", node->name);
	Steel_Node *itr = Steel_LL_Head(node->attributes);
	while (itr) {
		Attribute *attribute = (Attribute *)itr->data;
		printf(" %s=\"%s\"", attribute->name, attribute->value);
		Steel_Node_Next(itr);
	}
	if (Steel_LL_Empty(node->childs)) {
		printf("/>\n");
		return;
	}
	printf(">\n");
	itr = Steel_LL_Head(node->childs);
	while (itr) {
		XmlNode *child = (XmlNode *)itr->data;
		Cxmlp_XmlNode_Print_(child, indent+1);
		Steel_Node_Next(itr);
	}
	Cxmlp_XmlNode_PrintIndent(indent);
	printf("</%s>\n", node->name);
}

XmlNode *Cxmlp_XmlNode_Init() {
	XmlNode *node = (XmlNode *)malloc(sizeof(XmlNode));
	node->attributes = Steel_LL_Init();
	node->childs = Steel_LL_Init();
	node->parent=NULL;
	return node;
}

void Cxmlp_XmlNode_Print(XmlNode *node) {
	Cxmlp_XmlNode_Print_(node, 0);
}


#endif // CXMLP_NODE_H_
