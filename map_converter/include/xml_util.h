#ifndef XML_HEADER_INCLUDED
#define XML_HEADER_INCLUDED

typedef struct _xmlNode xmlNode;

int HasAttribute(const xmlNode* node, const char* name);
int IsNode(const xmlNode* node, const char* name);
void ReadAttribute(const xmlNode* node, const char* name, char** value);
void ReadIntAttribute(const xmlNode* node, const char* name, int* value);

#endif