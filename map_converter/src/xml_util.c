#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
int HasAttribute(const xmlNode* node, const char* name)
{
    return xmlHasProp(node, (const xmlChar*)name) != NULL;
}

//  ---------------------------------------------------------------------------
int IsNode(const xmlNode* node, const char* name)
{
    return xmlStrcmp(node->name, (const xmlChar*)name) == 0;
}

//  ---------------------------------------------------------------------------
void ReadAttribute(const xmlNode* node, const char* name, char** value)
{
    assert(*value == NULL);

    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* strValue = xmlGetProp(node, (const xmlChar*)name);
        int len = xmlStrlen(strValue);
        *value = malloc(sizeof(char*) * len + 1);
        strcpy(*value, (char *)strValue);
        xmlFree(strValue);
    }
}

//  ---------------------------------------------------------------------------
void ReadIntAttribute(const xmlNode* node, const char* name, int* value)
{
    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* strValue = xmlGetProp(node, (const xmlChar*)name);
        *value = strtol((char*)strValue, NULL, 10);
        xmlFree(strValue);
    }
}