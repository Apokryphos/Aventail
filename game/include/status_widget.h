#ifndef STATUS_WIDGET_HEADER_INCLUDED
#define STATUS_WIDGET_HEADER_INCLUDED

struct Actor;
struct GuiGaugeString;
struct GuiScreen;
struct Panel;

struct StatusWidget
{
    struct Panel* Panel;
    char* ActorName;
    struct Panel* NamePanel;
    struct Panel* HealthGaugePanel;
    struct GuiGaugeString* HealthGaugeString;
    struct Panel* CashPanel;
    struct GuiIntString* CashIntString;
};

struct StatusWidget* CreateStatusWidget(struct GuiScreen* guiScreen);
void SetStatusWidgetPosition(struct StatusWidget* widget, int x, int y);
void UpdateStatusWidget(struct StatusWidget* widget, struct Actor* actor);

#endif