#ifndef XML_HEADER_INCLUDED
#define XML_HEADER_INCLUDED

typedef struct _xmlNode xmlNode;

int has_attribute(const xmlNode* node, const char* name);
int is_node(const xmlNode* node, const char* name);
void read_attribute(const xmlNode* node, const char* name, char** value);
void read_bool_attribute(const xmlNode* node, const char* name, int* value);
void read_int_attribute(const xmlNode* node, const char* name, int* value);
void read_unsigned_int_attribute(const xmlNode* node, const char* name, unsigned int* value);

#endif