#ifndef STATUS_WIDGET_HEADER_INCLUDED
#define STATUS_WIDGET_HEADER_INCLUDED

struct Actor;
struct GuiGaugeString;
struct GuiScreen;
struct Panel;

struct StatusWidget
{
    struct Panel* panel;
    char* actor_name;
    struct Panel* name_panel;
    struct Panel* health_gauge_panel;
    struct GuiGaugeString* health_gauge_string;
    struct Panel* cash_panel;
    struct GuiIntString* cash_int_string;
    struct Panel* attack_panel;
    struct GuiIntString* attack_int_string;
    struct Panel* defend_panel;
    struct GuiIntString* defend_int_string;
    struct Panel* vitality_panel;
    struct GuiIntString* vitality_int_string;
};

struct StatusWidget* create_status_widget(struct GuiScreen* gui_screen);
void set_status_widget_position(
    struct StatusWidget* widget,
    const int x,
    const int y);
void update_status_widget(
    struct StatusWidget* widget,
    const struct Actor* actor);

#endif