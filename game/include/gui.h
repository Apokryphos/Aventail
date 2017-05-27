#ifndef GUI_HEADER_INCLUDED
#define GUI_HEADER_INCLUDED

struct Game;
struct GuiScreen;

void ActivateGui();
void AddGuiScreen(struct GuiScreen* screen);
void DeactivateGui();
void EnableCursor(int enabled);
void GuiDraw(struct Game* game);
void GuiUpdate(struct Game* game);
void SetCursorPosition(int x, int y);

#endif