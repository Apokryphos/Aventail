#ifndef GUI_INT_STRING
#define GUI_INT_STRING

struct GuiIntString
{
    int value;
    char* label;
    char* string;
};

struct GuiIntString* create_gui_int_string();

void destroy_gui_int_string(struct GuiIntString** gui_string);

void set_gui_int_string(
    struct GuiIntString* gui_string,
    const char* label,
    int value);

#endif