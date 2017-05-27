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
struct StatusWidget* CreateStatusWidget(struct GuiScreen* guiScreen)
{
    assert(guiScreen != NULL);

    struct StatusWidget* widget = malloc(sizeof(struct StatusWidget));

    widget->Panel = CreatePanel("Status", PANEL_BORDER_STYLE_3);
    widget->Panel->Width = 200;
    widget->Panel->Height = 200;
    widget->Panel->Background = 1;
    AddGuiScreenPanel(guiScreen, widget->Panel);

    widget->NamePanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->NamePanel->Width = 8;
    widget->NamePanel->Height = 8;
    AddGuiScreenPanel(guiScreen, widget->NamePanel);

    widget->HealthGaugePanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->HealthGaugePanel->Width = 8;
    widget->HealthGaugePanel->Height = 8;
    AddGuiScreenPanel(guiScreen, widget->HealthGaugePanel);

    widget->CashPanel = CreatePanel(NULL, PANEL_BORDER_STYLE_NONE);
    widget->CashPanel->Width = 8;
    widget->CashPanel->Height = 8;
    AddGuiScreenPanel(guiScreen, widget->CashPanel);

    widget->ActorName = NULL;
    widget->HealthGaugeString = CreateGuiGaugeString();
    widget->CashIntString = CreateGuiIntString();

    return widget;
}

//  ---------------------------------------------------------------------------
void SetStatusWidgetPosition(struct StatusWidget* widget, int x, int y)
{
    assert(widget != NULL);

    widget->Panel->X = 100;
    widget->Panel->Y = 100;

    int left = widget->Panel->X + 16;

    widget->NamePanel->X = left;
    widget->NamePanel->Y = widget->Panel->Y + 16;

    widget->HealthGaugePanel->X = left;
    widget->HealthGaugePanel->Y = widget->NamePanel->Y + widget->NamePanel->Height + 8;

    widget->CashPanel->X = left;
    widget->CashPanel->Y = widget->HealthGaugePanel->Y + widget->HealthGaugePanel->Height + 8;
}

//  ---------------------------------------------------------------------------
void UpdateStatusWidget(struct StatusWidget* widget, struct Actor* actor)
{
    assert(widget != NULL);

    const char* actorName = NULL;

    if (actor != NULL)
    {
        actorName = actor->Name;
    }

    SetGuiString(&widget->ActorName, actorName);
    widget->NamePanel->Text = widget->ActorName;

    SetGuiGaugeString(
        widget->HealthGaugeString,
        "Health",
        actor->Health,
        actor->MaxHealth);
    widget->HealthGaugePanel->Text = widget->HealthGaugeString->String;

    SetGuiIntString(widget->CashIntString, "Cash", actor->Cash);
    widget->CashPanel->Text = widget->CashIntString->String;
}