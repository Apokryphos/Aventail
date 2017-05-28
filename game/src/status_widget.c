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
struct StatusWidget* create_status_widget(struct GuiScreen* guiScreen)
{
    assert(guiScreen != NULL);

    struct StatusWidget* widget = malloc(sizeof(struct StatusWidget));

    widget->Panel = create_panel("Status", PANEL_BORDER_STYLE_3);
    widget->Panel->width = 200;
    widget->Panel->height = 200;
    widget->Panel->background = 1;
    AddGuiScreenPanel(guiScreen, widget->Panel);

    widget->NamePanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->NamePanel->width = 8;
    widget->NamePanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->NamePanel);

    widget->HealthGaugePanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->HealthGaugePanel->width = 8;
    widget->HealthGaugePanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->HealthGaugePanel);

    widget->AttackPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->AttackPanel->width = 8;
    widget->AttackPanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->AttackPanel);

    widget->DefendPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->DefendPanel->width = 8;
    widget->DefendPanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->DefendPanel);

    widget->VitalityPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->VitalityPanel->width = 8;
    widget->VitalityPanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->VitalityPanel);

    widget->CashPanel = create_panel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->CashPanel->width = 8;
    widget->CashPanel->height = 16;
    AddGuiScreenPanel(guiScreen, widget->CashPanel);

    widget->ActorName = NULL;
    widget->HealthGaugeString = create_gui_gauge_string();
    widget->CashIntString = create_gui_int_string();
    widget->AttackIntString = create_gui_int_string();
    widget->DefendIntString = create_gui_int_string();
    widget->VitalityIntString = create_gui_int_string();

    return widget;
}

//  ---------------------------------------------------------------------------
void set_status_widget_position(struct StatusWidget* widget, int x, int y)
{
    assert(widget != NULL);

    widget->Panel->X = 100;
    widget->Panel->Y = 100;

    int left = widget->Panel->X + 16;

    widget->NamePanel->X = left;
    widget->NamePanel->Y = widget->Panel->Y + 16;

    widget->HealthGaugePanel->X = left;
    widget->HealthGaugePanel->Y = widget->NamePanel->Y + widget->NamePanel->height * 2;

    widget->AttackPanel->X = left;
    widget->AttackPanel->Y = widget->HealthGaugePanel->Y + widget->HealthGaugePanel->height * 2;

    widget->DefendPanel->X = left;
    widget->DefendPanel->Y = widget->AttackPanel->Y + widget->AttackPanel->height;

    widget->VitalityPanel->X = left;
    widget->VitalityPanel->Y = widget->DefendPanel->Y + widget->DefendPanel->height;

    widget->CashPanel->X = left;
    widget->CashPanel->Y = widget->VitalityPanel->Y + widget->VitalityPanel->height * 2;
}

//  ---------------------------------------------------------------------------
void update_status_widget(struct StatusWidget* widget, struct Actor* actor)
{
    assert(widget != NULL);

    const char* actorName = NULL;

    if (actor != NULL)
    {
        actorName = actor->name;
    }

    set_gui_string(&widget->ActorName, actorName);
    widget->NamePanel->text = widget->ActorName;

    set_gui_gauge_string(
        widget->HealthGaugeString,
        "Health",
        actor->health,
        actor->max_health);
    widget->HealthGaugePanel->text = widget->HealthGaugeString->string;

    set_gui_int_string(widget->CashIntString, "Cash", actor->cash);
    widget->CashPanel->text = widget->CashIntString->string;

    set_gui_int_string(widget->AttackIntString, "Attack", actor->stats.attack);
    widget->AttackPanel->text = widget->AttackIntString->string;

    set_gui_int_string(widget->DefendIntString, "Defend", actor->stats.defend);
    widget->DefendPanel->text = widget->DefendIntString->string;

    set_gui_int_string(widget->VitalityIntString, "Vitality", actor->stats.vitality);
    widget->VitalityPanel->text = widget->VitalityIntString->string;
}