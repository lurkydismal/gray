#include "stdafx.h"
#include "UIActorMenu.h"
#include "../../xrUI/UIXmlInit.h"
#include "../../xrUI/xrUIXmlParser.h"
#include "UICharacterInfo.h"
#include "UIDragDropListEx.h"
#include "UIDragDropReferenceList.h"
#include "UIActorStateInfo.h"
#include "UIItemInfo.h"
#include "../../xrUI/Widgets/UIFrameLineWnd.h"
#include "UIMessageBoxEx.h"
#include "../../xrUI/Widgets/UIPropertiesBox.h"
#include "../../xrUI/Widgets/UI3tButton.h"

#include "UIInventoryUpgradeWnd.h"
#include "UIInvUpgradeInfo.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "object_broker.h"
#include "../../xrUI/Widgets/UIWndCallback.h"
#include "../UIHelperGame.h"
#include "../../xrUI/Widgets/UIProgressBar.h"
#include "../../xrUI/ui_base.h"
#include "../../xrEngine/string_table.h"

CUIActorMenu::CUIActorMenu()
{
    m_currMenuMode                    = mmUndefined;
    m_trade_partner_inventory_state = 0;
    Construct                        ();
}

CUIActorMenu::~CUIActorMenu()
{
    xr_delete            (m_message_box_yes_no);
    xr_delete            (m_message_box_ok);
    xr_delete            (m_UIPropertiesBox);
    xr_delete            (m_hint_wnd);
    xr_delete            (m_ItemInfo);

    ClearAllLists        ();
}

void CUIActorMenu::Construct()
{
    CUIXml                                uiXml;
    uiXml.Load                            (CONFIG_PATH, UI_PATH, "actor_menu.xml");

    CUIXmlInit                            xml_init;

    xml_init.InitWindow                    (uiXml, "main", 0, this);
    m_hint_wnd = UIHelper::CreateHint    (uiXml, "hint_wnd");

    m_LeftBackground                    = new CUIStatic();
    m_LeftBackground->SetAutoDelete        (true);
    AttachChild                            (m_LeftBackground);
    xml_init.InitStatic                    (uiXml, "left_background", 0, m_LeftBackground);

    m_pUpgradeWnd                        = new CUIInventoryUpgradeWnd(); 
    AttachChild                            (m_pUpgradeWnd);
    m_pUpgradeWnd->SetAutoDelete        (true);
    m_pUpgradeWnd->Init                    ();

    m_ActorCharacterInfo                = new CUICharacterInfo();
    m_ActorCharacterInfo->SetAutoDelete    (true);
    AttachChild                            (m_ActorCharacterInfo);
    m_ActorCharacterInfo->InitCharacterInfo(&uiXml, "actor_ch_info");

    m_PartnerCharacterInfo                = new CUICharacterInfo();
    m_PartnerCharacterInfo->SetAutoDelete(true);
    AttachChild                            (m_PartnerCharacterInfo);
    m_PartnerCharacterInfo->InitCharacterInfo( &uiXml, "partner_ch_info" );
    
    m_RightDelimiter            = UIHelper::CreateStatic(uiXml, "right_delimiter", this);
//    m_ActorTradeCaption            = UIHelper::CreateTextWnd(uiXml, "right_delimiter:trade_caption", m_RightDelimiter);
    m_ActorTradePrice            = UIHelper::CreateTextWnd(uiXml, "right_delimiter:trade_price", m_RightDelimiter);
    m_ActorTradeWeightMax        = UIHelper::CreateTextWnd(uiXml, "right_delimiter:trade_weight_max", m_RightDelimiter);
//    m_ActorTradeCaption->AdjustWidthToText();
    
    m_LeftDelimiter                = UIHelper::CreateStatic(uiXml, "left_delimiter", this);
//    m_PartnerTradeCaption        = UIHelper::CreateTextWnd(uiXml, "left_delimiter:trade_caption", m_LeftDelimiter);
    m_PartnerTradePrice            = UIHelper::CreateTextWnd(uiXml, "left_delimiter:trade_price", m_LeftDelimiter);
    m_PartnerTradeWeightMax        = UIHelper::CreateTextWnd(uiXml, "left_delimiter:trade_weight_max", m_LeftDelimiter);
//    m_PartnerTradeCaption->AdjustWidthToText();

    m_ActorBottomInfo            = UIHelper::CreateStatic(uiXml, "actor_weight_caption", this);
    m_ActorWeight                = UIHelper::CreateTextWnd(uiXml, "actor_weight", this);
    m_ActorWeightMax            = UIHelper::CreateTextWnd(uiXml, "actor_weight_max", this);
    m_ActorBottomInfo->AdjustWidthToText();

    m_PartnerBottomInfo            = UIHelper::CreateStatic(uiXml, "partner_weight_caption", this);
    m_PartnerWeight                = UIHelper::CreateTextWnd(uiXml, "partner_weight", this);
    m_PartnerBottomInfo->AdjustWidthToText();
    m_PartnerWeight_end_x        = m_PartnerWeight->GetWndPos().x;

    m_QuickSlotsHighlight[0]    = UIHelper::CreateStatic(uiXml, "quick_slot_highlight", this);
    m_QuickSlotsHighlight[0]    ->Show(false);

    for (u8 i = 0; i <= LAST_SLOT; ++i)
    {
        m_pInvList[i] = nullptr;
        m_pInvSlotHighlight[i] = nullptr;
        m_pInvSlotProgress[i] = nullptr;
    }

    XML_NODE* stored_root = uiXml.GetLocalRoot();
    XML_NODE* node = uiXml.NavigateToNode("inventory_slot_wnd", 0);
    uiXml.SetLocalRoot(node);

    m_slot_count = (u8)uiXml.GetNodesNum(node, "slot");
    for (u8 i = 1; i <= m_slot_count; ++i)
    {
        uiXml.SetLocalRoot(node);
        XML_NODE* slot_node = uiXml.NavigateToNode("slot", i - 1);
        uiXml.SetLocalRoot(slot_node);

        if (uiXml.GetNodesNum(slot_node, "slot_dragdrop") == 0)
            continue;

        m_pInvList[i] = new CUIDragDropListEx();
        AttachChild(m_pInvList[i]);
        CUIXmlInitGame::InitDragDropListEx(uiXml, "slot_dragdrop", 0, m_pInvList[i]);
        m_pInvList[i]->SetAutoDelete(true);

        BindDragDropListEvents(m_pInvList[i]);

        m_pInvSlotHighlight[i] = new CUIStatic();
        AttachChild(m_pInvSlotHighlight[i]);
        CUIXmlInit::InitStatic(uiXml, "slot_highlight", 0, m_pInvSlotHighlight[i]);
        m_pInvSlotHighlight[i]->SetAutoDelete(true);
        m_pInvSlotHighlight[i]->Show(false);

        if (uiXml.GetNodesNum(slot_node, "slot_progress") == 0)
            continue;

        m_pInvSlotProgress[i] = new CUIProgressBar();
        AttachChild(m_pInvSlotProgress[i]);
        CUIXmlInit::InitProgressBar(uiXml, "slot_progress", 0, m_pInvSlotProgress[i]);
        m_pInvSlotProgress[i]->SetAutoDelete(true);
    }
    uiXml.SetLocalRoot(stored_root);

    m_pInventoryBagList            = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_bag", this);
    m_pInventoryBeltList        = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_belt", this);

    m_pTradeActorBagList        = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_actor_trade_bag", this);
    m_pTradeActorList            = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_actor_trade", this);
    m_pTradePartnerBagList        = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_partner_bag", this);
    m_pTradePartnerList            = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_partner_trade", this);
    m_pDeadBodyBagList            = UIHelperGame::CreateDragDropListEx(uiXml, "dragdrop_deadbody_bag", this);
    m_pQuickSlot                = UIHelperGame::CreateDragDropReferenceList(uiXml, "dragdrop_quick_slots", this);
    m_pQuickSlot->Initialize    ();

    Fvector2 pos;
    pos                                = m_QuickSlotsHighlight[0]->GetWndPos();
    float dx                        = uiXml.ReadAttribFlt("quick_slot_highlight", 0, "dx", 24.0f);
    float dy                        = uiXml.ReadAttribFlt("quick_slot_highlight", 0, "dy", 24.0f);

    for(u8 i=1;i<4;i++)
    {
        pos.x                        += dx;
        m_QuickSlotsHighlight[i]    = UIHelper::CreateStatic(uiXml, "quick_slot_highlight", this);
        m_QuickSlotsHighlight[i]    ->SetWndPos(pos);
        m_QuickSlotsHighlight[i]    ->Show(false);
    }

    int cols = m_pInventoryBeltList->CellsCapacity().x;
    int rows = m_pInventoryBeltList->CellsCapacity().y;
    int counter = 1;

    for (u8 i = 0; i < rows; ++i)
    {
        for (u8 j = 0; j < cols; ++j)
        {
            if (i == 0 && j == 0)
            {
                m_ArtefactSlotsHighlight[0] = UIHelper::CreateStatic(uiXml, "artefact_slot_highlight", this);
                pos = m_ArtefactSlotsHighlight[0]->GetWndPos();
                m_ArtefactSlotsHighlight[0]->Show(false);
                dx = uiXml.ReadAttribFlt("artefact_slot_highlight", 0, "dx", 24.0f);
                dy = uiXml.ReadAttribFlt("artefact_slot_highlight", 0, "dy", 24.0f);
            }
            else
            {
                if (j != 0)
                    pos.x += dx;

                m_ArtefactSlotsHighlight[counter] = UIHelper::CreateStatic(uiXml, "artefact_slot_highlight", this);
                m_ArtefactSlotsHighlight[counter]->SetWndPos(pos);
                m_ArtefactSlotsHighlight[counter]->Show(false);
                counter++;
            }
        }

        pos.x = m_ArtefactSlotsHighlight[0]->GetWndPos().x;
        pos.y += dy;
    }

    m_pTrashList                = UIHelperGame::CreateDragDropListEx        (uiXml, "dragdrop_trash", this);
    m_pTrashList->m_f_item_drop    = CUIDragDropListEx::DRAG_CELL_EVENT    (this,&CUIActorMenu::OnItemDrop);
    m_pTrashList->m_f_drag_event= CUIDragDropListEx::DRAG_ITEM_EVENT    (this,&CUIActorMenu::OnDragItemOnTrash);

    counter = 1;

    for (u8 i = 0; i < rows; ++i)
    {
        for (u8 j = 0; j < cols; ++j)
        {
            if (i == 0 && j == 0)
            {
                m_belt_list_over[0] = UIHelper::CreateStatic(uiXml, "belt_list_over", this);
                pos = m_belt_list_over[0]->GetWndPos();
                dx = uiXml.ReadAttribFlt("belt_list_over", 0, "dx", 10.0f);
                dy = uiXml.ReadAttribFlt("belt_list_over", 0, "dy", 10.0f);
            }
            else
            {
                if (j != 0)
                    pos.x += dx;

                m_belt_list_over[counter] = UIHelper::CreateStatic(uiXml, "belt_list_over", this);
                m_belt_list_over[counter]->SetWndPos(pos);
                counter++;
            }
        }

        pos.x = m_belt_list_over[0]->GetWndPos().x;
        pos.y += dy;
    }

    m_HelmetOver = UIHelper::CreateStatic(uiXml, "helmet_over", this);
    m_HelmetOver->Show            (false);

    m_ActorMoney    = UIHelper::CreateTextWnd(uiXml, "actor_money_static", this);
    m_PartnerMoney    = UIHelper::CreateTextWnd(uiXml, "partner_money_static", this);
    m_QuickSlot1    = UIHelper::CreateTextWnd(uiXml, "quick_slot1_text", this);
    m_QuickSlot2    = UIHelper::CreateTextWnd(uiXml, "quick_slot2_text", this);
    m_QuickSlot3    = UIHelper::CreateTextWnd(uiXml, "quick_slot3_text", this);
    m_QuickSlot4    = UIHelper::CreateTextWnd(uiXml, "quick_slot4_text", this);

    m_trade_buy_button    = UIHelper::Create3tButton(uiXml, "trade_buy_button", this);
    m_trade_sell_button    = UIHelper::Create3tButton(uiXml, "trade_sell_button", this);
    m_takeall_button    = UIHelper::Create3tButton(uiXml, "takeall_button", this);
    m_putall_button        = UIHelper::Create3tButton(uiXml, "putall_button", this);
    m_exit_button        = UIHelper::Create3tButton(uiXml, "exit_button", this);

//    m_clock_value                        = UIHelper::CreateStatic(uiXml, "clock_value", this);

/*
    m_pDeadBodyBagList                    = new CUIDragDropListEx(); 
    AttachChild                            (m_pDeadBodyBagList);
    m_pDeadBodyBagList->SetAutoDelete    (true);
    xml_init.InitDragDropListEx            (uiXml, "dragdrop_deadbody_bag", 0, m_pDeadBodyBagList);
*/
    m_ActorStateInfo                    = new ui_actor_state_wnd();
    m_ActorStateInfo->init_from_xml        (uiXml, "actor_state_info");
    m_ActorStateInfo->SetAutoDelete        (true);
    AttachChild                            (m_ActorStateInfo); 

    stored_root                            = uiXml.GetLocalRoot    ();
    uiXml.SetLocalRoot                    (uiXml.NavigateToNode    ("action_sounds",0));
    ::Sound->create                        (sounds[eSndOpen],        uiXml.Read("snd_open",            0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eSndClose],        uiXml.Read("snd_close",            0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eItemToSlot],    uiXml.Read("snd_item_to_slot",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eItemToBelt],    uiXml.Read("snd_item_to_belt",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eItemToRuck],    uiXml.Read("snd_item_to_ruck",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eProperties],    uiXml.Read("snd_properties",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eDropItem],        uiXml.Read("snd_drop_item",        0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eAttachAddon],    uiXml.Read("snd_attach_addon",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eDetachAddon],    uiXml.Read("snd_detach_addon",    0,    nullptr),st_Effect,sg_SourceType);
    ::Sound->create                        (sounds[eItemUse],        uiXml.Read("snd_item_use",        0,    nullptr),st_Effect,sg_SourceType);
    uiXml.SetLocalRoot                    (stored_root);

    m_ItemInfo                            = new CUIItemInfo();
//-    m_ItemInfo->SetAutoDelete            (true);
//-    AttachChild                            (m_ItemInfo);
    m_ItemInfo->InitItemInfo            ("actor_menu_item.xml");

    m_upgrade_info                        = nullptr;
    if ( ai().get_alife() )
    {
        m_upgrade_info                        = new UIInvUpgradeInfo();
        m_upgrade_info->SetAutoDelete        (true);
        AttachChild                            (m_upgrade_info);
        m_upgrade_info->init_from_xml        ("actor_menu_item.xml");
    }

    m_message_box_yes_no                = new CUIMessageBoxEx();    
    m_message_box_yes_no->InitMessageBox( "message_box_yes_no" );
    m_message_box_yes_no->SetAutoDelete    (true);
    m_message_box_yes_no->SetText        ( "" );

    m_message_box_ok                    = new CUIMessageBoxEx();    
    m_message_box_ok->InitMessageBox    ( "message_box_ok" );
    m_message_box_ok->SetAutoDelete        (true);
    m_message_box_ok->SetText            ( "" );

    m_UIPropertiesBox                    = new CUIPropertiesBox();
    m_UIPropertiesBox->InitPropertiesBox(Fvector2().set(0,0),Fvector2().set(300,300));
    AttachChild                            (m_UIPropertiesBox);
    m_UIPropertiesBox->Hide                ();
    m_UIPropertiesBox->SetWindowName    ( "property_box" );

    InitCallbacks                        ();

    BindDragDropListEvents(m_pInventoryBeltList);
    BindDragDropListEvents(m_pInventoryBagList);
    BindDragDropListEvents(m_pTradeActorBagList);
    BindDragDropListEvents(m_pTradeActorList);
    BindDragDropListEvents(m_pTradePartnerBagList);
    BindDragDropListEvents(m_pTradePartnerList);
    BindDragDropListEvents(m_pDeadBodyBagList);
    BindDragDropListEvents(m_pQuickSlot);

    m_allowed_drops[iTrashSlot].push_back(iActorBag);
    m_allowed_drops[iTrashSlot].push_back(iActorSlot);
    m_allowed_drops[iTrashSlot].push_back(iActorBelt);
    m_allowed_drops[iTrashSlot].push_back(iQuickSlot);

    m_allowed_drops[iActorSlot].push_back(iActorBag);
    m_allowed_drops[iActorSlot].push_back(iActorSlot);
    m_allowed_drops[iActorSlot].push_back(iActorTrade);
    m_allowed_drops[iActorSlot].push_back(iDeadBodyBag);

    m_allowed_drops[iActorBag].push_back(iActorSlot);
    m_allowed_drops[iActorBag].push_back(iActorBelt);
    m_allowed_drops[iActorBag].push_back(iActorTrade);
    m_allowed_drops[iActorBag].push_back(iDeadBodyBag);
    m_allowed_drops[iActorBag].push_back(iActorBag);
    m_allowed_drops[iActorBag].push_back(iQuickSlot);
    
    m_allowed_drops[iActorBelt].push_back(iActorBag);
    m_allowed_drops[iActorBelt].push_back(iActorTrade);
    m_allowed_drops[iActorBelt].push_back(iDeadBodyBag);
    m_allowed_drops[iActorBelt].push_back(iActorBelt);

    m_allowed_drops[iActorTrade].push_back(iActorSlot);
    m_allowed_drops[iActorTrade].push_back(iActorBag);
    m_allowed_drops[iActorTrade].push_back(iActorBelt);
    m_allowed_drops[iActorTrade].push_back(iActorTrade);
    m_allowed_drops[iActorTrade].push_back(iQuickSlot);

    m_allowed_drops[iPartnerTradeBag].push_back(iPartnerTrade);
    m_allowed_drops[iPartnerTradeBag].push_back(iPartnerTradeBag);
    m_allowed_drops[iPartnerTrade].push_back(iPartnerTradeBag);
    m_allowed_drops[iPartnerTrade].push_back(iPartnerTrade);

    m_allowed_drops[iDeadBodyBag].push_back(iActorSlot);
    m_allowed_drops[iDeadBodyBag].push_back(iActorBag);
    m_allowed_drops[iDeadBodyBag].push_back(iActorBelt);
    m_allowed_drops[iDeadBodyBag].push_back(iDeadBodyBag);

    m_allowed_drops[iQuickSlot].push_back(iActorBag);
    m_allowed_drops[iQuickSlot].push_back(iActorTrade);

    m_upgrade_selected                = nullptr;
    SetCurrentItem                    (nullptr);
    SetActor                        (nullptr);
    SetPartner                        (nullptr);
    SetInvBox                        (nullptr);

    m_actor_trade                    = nullptr;
    m_partner_trade                    = nullptr;
    m_repair_mode                    = false;
    m_item_info_view                = false;
    m_highlight_clear                = true;

    DeInitInventoryMode                ();
    DeInitTradeMode                    ();
    DeInitUpgradeMode                ();
    DeInitDeadBodySearchMode        ();
}

void CUIActorMenu::BindDragDropListEvents(CUIDragDropListEx* lst)
{
    if (lst == nullptr)
        return;

    lst->m_f_item_drop                = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemDrop);
    lst->m_f_item_start_drag        = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemStartDrag);
    lst->m_f_item_db_click            = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemDbClick);
    lst->m_f_item_selected            = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemSelected);
    lst->m_f_item_rbutton_click        = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemRButtonClick);
    lst->m_f_item_focus_received    = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemFocusReceive);
    lst->m_f_item_focus_lost        = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemFocusLost);
    lst->m_f_item_focused_update    = CUIDragDropListEx::DRAG_CELL_EVENT(this,&CUIActorMenu::OnItemFocusedUpdate);
}

void CUIActorMenu::InitCallbacks()
{
    Register                        (m_trade_buy_button);
    Register                        (m_trade_sell_button);
    Register                        (m_takeall_button);
    Register                        (m_putall_button);
    Register                        (m_exit_button);
    Register                        (m_UIPropertiesBox);
    VERIFY                            (m_pUpgradeWnd);
    Register                        (m_pUpgradeWnd->m_btn_repair);

    AddCallback(m_trade_buy_button,BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::OnBtnPerformTradeBuy));
    AddCallback(m_trade_sell_button,BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::OnBtnPerformTradeSell));
    AddCallback(m_takeall_button,  BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::TakeAllFromPartner));
    AddCallback(m_putall_button,  BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::PutAllToPartner));
    AddCallback(m_exit_button,     BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::OnBtnExitClicked));
    AddCallback(m_UIPropertiesBox, PROPERTY_CLICKED, CUIWndCallback::void_function(this, &CUIActorMenu::ProcessPropertiesBoxClicked));
    AddCallback(m_pUpgradeWnd->m_btn_repair, BUTTON_CLICKED,   CUIWndCallback::void_function(this, &CUIActorMenu::TryRepairItem));
}

void CUIActorMenu::UpdateButtonsLayout()
{
    string32 tmp;
    LPCSTR str = g_pStringTable->translate("quick_use_str_1").c_str();
    strncpy_s(tmp, sizeof(tmp), str, 3);
    if(tmp[2]==',')
        tmp[1] = '\0';
    m_QuickSlot1->SetTextST(tmp);

    str = g_pStringTable->translate("quick_use_str_2").c_str();
    strncpy_s(tmp, sizeof(tmp), str, 3);
    if(tmp[2]==',')
        tmp[1] = '\0';
    m_QuickSlot2->SetTextST(tmp);

    str = g_pStringTable->translate("quick_use_str_3").c_str();
    strncpy_s(tmp, sizeof(tmp), str, 3);
    if(tmp[2]==',')
        tmp[1] = '\0';
    m_QuickSlot3->SetTextST(tmp);

    str = g_pStringTable->translate("quick_use_str_4").c_str();
    strncpy_s(tmp, sizeof(tmp), str, 3);
    if(tmp[2]==',')
        tmp[1] = '\0';
    m_QuickSlot4->SetTextST(tmp);

    UpdateConditionProgressBars        ();
}