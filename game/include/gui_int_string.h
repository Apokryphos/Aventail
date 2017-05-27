#ifndef GUI_INT_STRING
#define GUI_INT_STRING

struct GuiIntString
{
    int Value;
    char* Label;
    char* String;
};

struct GuiIntString* CreateGuiIntString();

void DestroyGuiIntString(struct GuiIntString** intString);

void SetGuiIntString(
    struct GuiIntString* intString,
    const char* label,
    int value);

#endif