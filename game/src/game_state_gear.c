#include "actor.h"
#include "game.h"
#include "gui.h"
#include "gui_screen.h"
#include "input_device.h"
#include "inventory.h"
#include "inventory_widget.h"
#include "item.h"
#include "item_slot_widget.h"
#include "panel.h"
#include "player.h"
#include "render.h"
#include "world.h"
#include <assert.h>
#include <SDL2/SDL.h>

enum GuiState
{
    GUI_STATE_SELECT_GEAR_SLOT,
    GUI_STATE_SELECT_INVENTORY_ITEM_SLOT
};

struct GearSlotWidget
{
    int Index;
    struct ItemSlotWidget* ItemSlotWidget;
    struct Panel* SlotPanel;
};

static const int GearSlotCount = 4;

static int SelectedGearSlot = 0;
static enum GuiState GearGuiState = GUI_STATE_SELECT_GEAR_SLOT;

static struct GuiScreen* GearGuiScreen = NULL;
static struct GearSlotWidget WeaponSlotWidget;
static struct GearSlotWidget ArmorSlotWidget;
static struct GearSlotWidget ShieldSlotWidget;
static struct GearSlotWidget AccessorySlotWidget;
static struct InventoryWidget* InventoryWidget = NULL;

//  ---------------------------------------------------------------------------
static enum ItemType GetSelectedGearSlotItemType()
{
    switch (SelectedGearSlot)
    {
        default:
        case 0:
            return ITEM_TYPE_WEAPON;
        case 1:
            return ITEM_TYPE_ARMOR;
        case 2:
            return ITEM_TYPE_SHIELD;
        case 3:
            return ITEM_TYPE_ACCESSORY;
    }
}

//  ---------------------------------------------------------------------------
static void ExitGearGameState(
    struct Game* game,
    enum GameState gameState)
{
    SelectedGearSlot = 0;
    InventoryWidget->SelectedItemIndex = 0;
    game->State = gameState;
    DeactivateGui();
    GearGuiScreen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void ProcessSelectGearSlotStateInput(struct Game* game)
{
    assert(GearGuiState == GUI_STATE_SELECT_GEAR_SLOT);

    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Remove)
    {
        struct Actor* actor = game->World->Player.actor;
        enum ItemType itemType = GetSelectedGearSlotItemType();
        remove_item_from_gear(actor, itemType);
        UpdateInventoryWidget(InventoryWidget, actor->inventory);
    }
    else if (inputDevice->Accept)
    {
        if (InventoryWidget->ItemCount > 0)
        {
            GearGuiState = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            InventoryWidget->SelectedItemIndex = 0;
        }
    }
    else if (inputDevice->Cancel)
    {
        ExitGearGameState(game, GAME_STATE_LEVEL);
    }
    else if (inputDevice->MoveDirection == DIRECTION_DOWN)
    {
        ++SelectedGearSlot;
        if (SelectedGearSlot > GearSlotCount - 1)
        {
            SelectedGearSlot = 0;
        }
    }
    else if (inputDevice->MoveDirection == DIRECTION_UP)
    {
        --SelectedGearSlot;
        if (SelectedGearSlot < 0)
        {
            SelectedGearSlot = GearSlotCount - 1;
        }
    }
}

//  ---------------------------------------------------------------------------
static void ProcessSelectInventoryItemSlotStateInput(struct Game* game)
{
    assert(GearGuiState == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT);

    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Cancel)
    {
        InventoryWidget->SelectedItemIndex = 0;
        GearGuiState = GUI_STATE_SELECT_GEAR_SLOT;
    }

    if (inputDevice->Accept)
    {
        if (InventoryWidget->ItemCount > 0)
        {
            struct Actor* actor = game->World->Player.actor;
            struct Item* item = InventoryWidget->Items[InventoryWidget->SelectedItemIndex];
            equip_item(actor, item);
            remove_item_from_inventory(actor->inventory, item);
            UpdateInventoryWidget(InventoryWidget, actor->inventory);

            InventoryWidget->SelectedItemIndex = 0;
            GearGuiState = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }

    if (inputDevice->MoveDirection == DIRECTION_DOWN)
    {
        SelectNextInventoryWidgetItemSlot(InventoryWidget);
    }

    if (inputDevice->MoveDirection == DIRECTION_UP)
    {
        SelectPreviousInventoryWidgetItemSlot(InventoryWidget);
    }
}

//  ---------------------------------------------------------------------------
static void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* inputDevice = game->InputDevice;

    if (inputDevice->Gear)
    {
        ExitGearGameState(game, GAME_STATE_LEVEL);
    }
    else if (inputDevice->Inventory)
    {
        ExitGearGameState(game, GAME_STATE_INVENTORY);
    }
    else if (inputDevice->Status)
    {
        ExitGearGameState(game, GAME_STATE_STATUS);
    }
    else
    {
        switch (GearGuiState)
        {
            case GUI_STATE_SELECT_GEAR_SLOT:
                ProcessSelectGearSlotStateInput(game);
                break;
            case GUI_STATE_SELECT_INVENTORY_ITEM_SLOT:
                ProcessSelectInventoryItemSlotStateInput(game);
                break;
        }
    }
}

//  ---------------------------------------------------------------------------
static struct GearSlotWidget CreateGearSlotWidget(
    struct GuiScreen* screen,
    const char* slotName,
    int slotIndex)
{
    struct Panel* slotPanel = CreatePanel(slotName, PANEL_BORDER_STYLE_2);
    slotPanel->Width = 164;
    slotPanel->Height = 56;
    slotPanel->X = 16;
    slotPanel->Y = slotIndex * (slotPanel->Height + 32) + 32;
    slotPanel->Background = 1;
    AddGuiScreenPanel(screen, slotPanel);

    struct GearSlotWidget gearSlotWidget =
    {
        .Index = slotIndex,
        .SlotPanel = slotPanel,
        .ItemSlotWidget = CreateItemSlotWidget(screen),
    };

    SetItemSlotWidgetPosition(
        gearSlotWidget.ItemSlotWidget,
        slotPanel->X,
        slotPanel->Y);

    return gearSlotWidget;
}

//  ---------------------------------------------------------------------------
void GearGameStateDraw(struct Game* game, int inTransition)
{
    DrawMap(game->Renderer, game->World->Map, game->World->Actors);
    GuiDraw(game);
}

//  ---------------------------------------------------------------------------
static void InitGearGuiScreen(SDL_Renderer* renderer)
{
    GearGuiScreen = CreateGuiScreen();

    WeaponSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Weapon", 0);
    ArmorSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Armor", 1);
    ShieldSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Shield", 2);
    AccessorySlotWidget = CreateGearSlotWidget(GearGuiScreen, "Accessory", 3);

    InventoryWidget = CreateInventoryWidget(GearGuiScreen);

    SDL_Rect viewport;
    SDL_RenderGetViewport(renderer, &viewport);
    SetInventoryWidgetPosition(
        InventoryWidget,
        viewport.w - InventoryWidget->Panel->Width - 16,
        32);

    AddGuiScreen(GearGuiScreen);
}

//  ---------------------------------------------------------------------------
static struct GearSlotWidget* GetSelectedGearSlotGui()
{
    assert(SelectedGearSlot < 4);

    switch (SelectedGearSlot)
    {
        default:
        case 0:
            return &WeaponSlotWidget;
        case 1:
            return &ArmorSlotWidget;
        case 2:
            return &ShieldSlotWidget;
        case 3:
            return &AccessorySlotWidget;
    }
}

//  ---------------------------------------------------------------------------
static void UpdateCursor()
{
    if (GearGuiState == GUI_STATE_SELECT_GEAR_SLOT)
    {
        assert(SelectedGearSlot < 4);

        struct GearSlotWidget* selectedGearSlot = GetSelectedGearSlotGui();

        SetCursorPosition(
            selectedGearSlot->ItemSlotWidget->ItemIconPanel->X + 4,
            selectedGearSlot->ItemSlotWidget->ItemIconPanel->Y + 4);
    }
    else if (GearGuiState == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(InventoryWidget->SelectedItemIndex < MAX_INVENTORY_ITEMS);

        assert(InventoryWidget->SelectedItemSlotWidget != NULL);
        assert(InventoryWidget->SelectedItemSlotWidget->ItemIconPanel != NULL);
        SetCursorPosition(
            InventoryWidget->SelectedItemSlotWidget->ItemIconPanel->X + 4,
            InventoryWidget->SelectedItemSlotWidget->ItemIconPanel->Y + 4);
    }
}

//  ---------------------------------------------------------------------------
static void UpdateGearSlotWidget(
    struct GearSlotWidget* gearSlotWidget,
     struct Item* item)
{
    UpdateItemSlotWidget(gearSlotWidget->ItemSlotWidget, item);
}

//  ---------------------------------------------------------------------------
void GearGameStateUpdate(struct Game* game)
{
    if (GearGuiScreen == NULL)
    {
        InitGearGuiScreen(game->Renderer);
    }

    GearGuiScreen->Enabled = 1;

    assert(SelectedGearSlot >= 0 && SelectedGearSlot < 4);
    if (InventoryWidget->SelectedItemIndex > InventoryWidget->ItemCount -1)
    {
        InventoryWidget->SelectedItemIndex = InventoryWidget->ItemCount -1;
    }
    if (InventoryWidget->SelectedItemIndex < 0)
    {
        InventoryWidget->SelectedItemIndex = 0;
    }

    ActivateGui();
    EnableCursor(1);
    ProcessInventoryGameStateInput(game);

    struct Actor* actor = game->World->Player.actor;
    UpdateGearSlotWidget(&ArmorSlotWidget, actor->gear.armor);
    UpdateGearSlotWidget(&ShieldSlotWidget, actor->gear.shield);
    UpdateGearSlotWidget(&WeaponSlotWidget, actor->gear.weapon);
    UpdateGearSlotWidget(&AccessorySlotWidget, actor->gear.accessory);

    InventoryWidget->ItemType = GetSelectedGearSlotItemType();
    UpdateInventoryWidget(InventoryWidget, actor->inventory);

    UpdateCursor();
}