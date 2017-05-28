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
    InventoryWidget->selected_item_index = 0;
    game->state = gameState;
    deactivate_gui();
    GearGuiScreen->Enabled = 0;
}

//  ---------------------------------------------------------------------------
static void ProcessSelectGearSlotStateInput(struct Game* game)
{
    assert(GearGuiState == GUI_STATE_SELECT_GEAR_SLOT);

    struct InputDevice* input_device = game->input_device;

    if (input_device->remove)
    {
        struct Actor* actor = game->world->player.actor;
        enum ItemType itemType = GetSelectedGearSlotItemType();
        remove_item_from_gear(actor, itemType);
        update_inventory_widget(InventoryWidget, actor->inventory);
    }
    else if (input_device->accept)
    {
        if (InventoryWidget->item_count > 0)
        {
            GearGuiState = GUI_STATE_SELECT_INVENTORY_ITEM_SLOT;
            InventoryWidget->selected_item_index = 0;
        }
    }
    else if (input_device->cancel)
    {
        ExitGearGameState(game, GAME_STATE_LEVEL);
    }
    else if (input_device->move_direction == DIRECTION_DOWN)
    {
        ++SelectedGearSlot;
        if (SelectedGearSlot > GearSlotCount - 1)
        {
            SelectedGearSlot = 0;
        }
    }
    else if (input_device->move_direction == DIRECTION_UP)
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

    struct InputDevice* input_device = game->input_device;

    if (input_device->cancel)
    {
        InventoryWidget->selected_item_index = 0;
        GearGuiState = GUI_STATE_SELECT_GEAR_SLOT;
    }

    if (input_device->accept)
    {
        if (InventoryWidget->item_count > 0)
        {
            struct Actor* actor = game->world->player.actor;
            struct Item* item = InventoryWidget->items[InventoryWidget->selected_item_index];
            equip_item(actor, item);
            remove_item_from_inventory(actor->inventory, item);
            update_inventory_widget(InventoryWidget, actor->inventory);

            InventoryWidget->selected_item_index = 0;
            GearGuiState = GUI_STATE_SELECT_GEAR_SLOT;
        }
    }

    if (input_device->move_direction == DIRECTION_DOWN)
    {
        select_next_inventory_widget_item_slot(InventoryWidget);
    }

    if (input_device->move_direction == DIRECTION_UP)
    {
        select_previous_inventory_widget_item_slot(InventoryWidget);
    }
}

//  ---------------------------------------------------------------------------
static void ProcessInventoryGameStateInput(struct Game* game)
{
    struct InputDevice* input_device = game->input_device;

    if (input_device->gear)
    {
        ExitGearGameState(game, GAME_STATE_LEVEL);
    }
    else if (input_device->inventory)
    {
        ExitGearGameState(game, GAME_STATE_INVENTORY);
    }
    else if (input_device->status)
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
    draw_map(game->renderer, game->world->map, game->world->actors);
    draw_gui(game);
}

//  ---------------------------------------------------------------------------
static void InitGearGuiScreen(SDL_Renderer* renderer)
{
    GearGuiScreen = CreateGuiScreen();

    WeaponSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Weapon", 0);
    ArmorSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Armor", 1);
    ShieldSlotWidget = CreateGearSlotWidget(GearGuiScreen, "Shield", 2);
    AccessorySlotWidget = CreateGearSlotWidget(GearGuiScreen, "Accessory", 3);

    InventoryWidget = create_inventory_widget(GearGuiScreen);

    SDL_Rect viewport;
    SDL_RenderGetViewport(renderer, &viewport);
    set_inventory_widget_position(
        InventoryWidget,
        viewport.w - InventoryWidget->panel->Width - 16,
        32);

    add_gui_screen(GearGuiScreen);
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

        set_gui_cursor_position(
            selectedGearSlot->ItemSlotWidget->ItemIconPanel->X + 4,
            selectedGearSlot->ItemSlotWidget->ItemIconPanel->Y + 4);
    }
    else if (GearGuiState == GUI_STATE_SELECT_INVENTORY_ITEM_SLOT)
    {
        assert(InventoryWidget->selected_item_index < MAX_INVENTORY_ITEMS);

        assert(InventoryWidget->selected_item_slot_widget != NULL);
        assert(InventoryWidget->selected_item_slot_widget->ItemIconPanel != NULL);
        set_gui_cursor_position(
            InventoryWidget->selected_item_slot_widget->ItemIconPanel->X + 4,
            InventoryWidget->selected_item_slot_widget->ItemIconPanel->Y + 4);
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
        InitGearGuiScreen(game->renderer);
    }

    GearGuiScreen->Enabled = 1;

    assert(SelectedGearSlot >= 0 && SelectedGearSlot < 4);
    if (InventoryWidget->selected_item_index > InventoryWidget->item_count -1)
    {
        InventoryWidget->selected_item_index = InventoryWidget->item_count -1;
    }
    if (InventoryWidget->selected_item_index < 0)
    {
        InventoryWidget->selected_item_index = 0;
    }

    activate_gui();
    enable_gui_cursor(1);
    ProcessInventoryGameStateInput(game);

    struct Actor* actor = game->world->player.actor;
    UpdateGearSlotWidget(&ArmorSlotWidget, actor->gear.armor);
    UpdateGearSlotWidget(&ShieldSlotWidget, actor->gear.shield);
    UpdateGearSlotWidget(&WeaponSlotWidget, actor->gear.weapon);
    UpdateGearSlotWidget(&AccessorySlotWidget, actor->gear.accessory);

    InventoryWidget->item_type = GetSelectedGearSlotItemType();
    update_inventory_widget(InventoryWidget, actor->inventory);

    UpdateCursor();
}