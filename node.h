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

XmlNode *Cxmlp_XmlNode_Init() {
	XmlNode *node = (XmlNode *)malloc(sizeof(XmlNode));
	node->attributes = Steel_LL_Init();
	node->childs = Steel_LL_Init();
	node->parent=NULL;
	return node;
}

#endif // CXMLP_NODE_H_
