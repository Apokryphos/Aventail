#ifndef GUI_GAUGE_STRING
#define GUI_GAUGE_STRING

struct GuiGaugeString
{
    int Value;
    int MaxValue;
    char* Label;
    char* String;
};

struct GuiGaugeString* CreateGuiGaugeString();

void DestroyGuiGaugeString(struct GuiGaugeString** gaugeString);

void SetGuiGaugeString(
    struct GuiGaugeString* gaugeString,
    const char* label,
    int value,
    int maxValue);

#endif