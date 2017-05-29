#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
int has_attribute(const xmlNode* node, const char* name)
{
    return xmlHasProp(node, (const xmlChar*)name) != NULL;
}

//  ---------------------------------------------------------------------------
int is_node(const xmlNode* node, const char* name)
{
    return xmlStrcmp(node->name, (const xmlChar*)name) == 0;
}

//  ---------------------------------------------------------------------------
void read_attribute(const xmlNode* node, const char* name, char** value)
{
    assert(*value == NULL);

    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* str_value = xmlGetProp(node, (const xmlChar*)name);
        int len = xmlStrlen(str_value);
        *value = malloc(sizeof(char*) * len + 1);
        strcpy(*value, (char *)str_value);
        xmlFree(str_value);
    }
}

//  ---------------------------------------------------------------------------
void read_int_attribute(const xmlNode* node, const char* name, int* value)
{
    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* str_value = xmlGetProp(node, (const xmlChar*)name);
        *value = strtol((char*)str_value, NULL, 10);
        xmlFree(str_value);
    }
}