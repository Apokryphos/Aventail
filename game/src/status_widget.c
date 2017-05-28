#include "actor.h"
#include "gui_screen.h"
#include "gui_gauge_string.h"
#include "gui_int_string.h"
#include "gui_string.h"
#include "panel.h"
#include "status_widget.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

//  ---------------------------------------------------------------------------
struct StatusWidget* create_status_widget(struct GuiScreen* gui_screen)
{
    assert(gui_screen != NULL);

    struct StatusWidget* widget = malloc(sizeof(struct StatusWidget));

    widget->panel = create_panel("Status", PANEL_BORDER_STYLE_3);
    widget->panel->width = 200;
    widget->panel->height = 200;
    widget->panel->background = 1;
    add_panel_to_gui_screen(gui_screen, widget->panel);

    widget->name_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->name_panel->width = 8;
    widget->name_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->name_panel);

    widget->health_gauge_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->health_gauge_panel->width = 8;
    widget->health_gauge_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->health_gauge_panel);

    widget->attack_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->attack_panel->width = 8;
    widget->attack_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->attack_panel);

    widget->defend_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->defend_panel->width = 8;
    widget->defend_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->defend_panel);

    widget->vitality_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->vitality_panel->width = 8;
    widget->vitality_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->vitality_panel);

    widget->cash_panel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->cash_panel->width = 8;
    widget->cash_panel->height = 16;
    add_panel_to_gui_screen(gui_screen, widget->cash_panel);

    widget->actor_name = NULL;
    widget->health_gauge_string = create_gui_gauge_string();
    widget->cash_int_string = create_gui_int_string();
    widget->attack_int_string = create_gui_int_string();
    widget->defend_int_string = create_gui_int_string();
    widget->vitality_int_string = create_gui_int_string();

    return widget;
}

//  ---------------------------------------------------------------------------
void set_status_widget_position(struct StatusWidget* widget, int x, int y)
{
    assert(widget != NULL);

    widget->panel->X = 100;
    widget->panel->Y = 100;

    int left = widget->panel->X + 16;

    widget->name_panel->X = left;
    widget->name_panel->Y = widget->panel->Y + 16;

    widget->health_gauge_panel->X = left;
    widget->health_gauge_panel->Y = widget->name_panel->Y + widget->name_panel->height * 2;

    widget->attack_panel->X = left;
    widget->attack_panel->Y = widget->health_gauge_panel->Y + widget->health_gauge_panel->height * 2;

    widget->defend_panel->X = left;
    widget->defend_panel->Y = widget->attack_panel->Y + widget->attack_panel->height;

    widget->vitality_panel->X = left;
    widget->vitality_panel->Y = widget->defend_panel->Y + widget->defend_panel->height;

    widget->cash_panel->X = left;
    widget->cash_panel->Y = widget->vitality_panel->Y + widget->vitality_panel->height * 2;
}

//  ---------------------------------------------------------------------------
void update_status_widget(struct StatusWidget* widget, struct Actor* actor)
{
    assert(widget != NULL);

    const char* actor_name = NULL;

    if (actor != NULL)
    {
        actor_name = actor->name;
    }

    set_gui_string(&widget->actor_name, actor_name);
    widget->name_panel->text = widget->actor_name;

    set_gui_gauge_string(
        widget->health_gauge_string,
        "Health",
        actor->health,
        actor->max_health);
    widget->health_gauge_panel->text = widget->health_gauge_string->string;

    set_gui_int_string(widget->cash_int_string, "Cash", actor->cash);
    widget->cash_panel->text = widget->cash_int_string->string;

    set_gui_int_string(widget->attack_int_string, "Attack", actor->stats.attack);
    widget->attack_panel->text = widget->attack_int_string->string;

    set_gui_int_string(widget->defend_int_string, "Defend", actor->stats.defend);
    widget->defend_panel->text = widget->defend_int_string->string;

    set_gui_int_string(widget->vitality_int_string, "Vitality", actor->stats.vitality);
    widget->vitality_panel->text = widget->vitality_int_string->string;
}