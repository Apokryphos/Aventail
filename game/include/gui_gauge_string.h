#ifndef GUI_GAUGE_STRING
#define GUI_GAUGE_STRING

struct GuiGaugeString
{
    int value;
    int max_value;
    char* label;
    char* string;
};

struct GuiGaugeString* create_gui_gauge_string();

void destroy_gui_gauge_string(struct GuiGaugeString** gauge_string);

void set_gui_gauge_string(
    struct GuiGaugeString* gauge_string,
    const char* label,
    int value,
    int max_value);

#endif