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
void read_bool_attribute(const xmlNode* node, const char* name, int* value)
{
    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* str_value = xmlGetProp(node, (const xmlChar*)name);

        if (xmlStrcmp((const xmlChar*)"true", str_value) == 0)
        {
            *value = 1;
        }
        else if (xmlStrcmp((const xmlChar*)"false", str_value) == 0)
        {
            *value = 0;
        }

        xmlFree(str_value);
    }
}

//  ---------------------------------------------------------------------------
void read_double_attribute(const xmlNode* node, const char* name, double* value)
{
    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* str_value = xmlGetProp(node, (const xmlChar*)name);
        *value = strtod((char*)str_value, NULL);
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

//  ---------------------------------------------------------------------------
void read_unsigned_int_attribute(const xmlNode* node, const char* name, unsigned int* value)
{
    if (xmlHasProp(node, (const xmlChar*)name) != NULL)
    {
        xmlChar* str_value = xmlGetProp(node, (const xmlChar*)name);
        *value = strtoul((char*)str_value, NULL, 10);
        xmlFree(str_value);
    }
}