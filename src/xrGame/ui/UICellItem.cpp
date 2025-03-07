#include "stdafx.h"
#include "UICellItem.h"
#include "../../xrUI/UICursor.h"
#include "../inventory_item.h"
#include "UIDragDropListEx.h"
#include "../../xrEngine/xr_input.h"
#include "../Level.h"
#include "object_broker.h"
#include "../../xrUI/UIXmlInit.h"
#include "../../xrUI/Widgets/UIProgressBar.h"

#include "CustomOutfit.h"

CUICellItem* CUICellItem::m_mouse_selected_item = nullptr;

CUICellItem::CUICellItem()
{
    m_pParentList        = nullptr;
    m_pData                = nullptr;
    m_custom_draw        = nullptr;
    m_text                = nullptr;
//-    m_mark                = nullptr;
    m_custom_text        = nullptr;
    m_custom_mark        = nullptr;
    m_upgrade            = nullptr;
    m_pConditionState    = nullptr;
    m_drawn_frame        = 0;
    SetAccelerator        (0);
    m_b_destroy_childs    = true;
    m_selected            = false;
    m_select_armament    = false;
    m_cur_mark            = false;
    m_has_upgrade        = false;
    m_with_custom_text    = false;
    m_with_custom_mark    = false;
    
    init();
}

CUICellItem::~CUICellItem()
{
    if(m_b_destroy_childs)
        delete_data    (m_childs);

    delete_data        (m_custom_draw);
}

void CUICellItem::init()
{
    static CUIXml uiXml;
    static bool is_xml_ready = false;

    if (!is_xml_ready)
    {
        uiXml.Load(CONFIG_PATH, UI_PATH, "actor_menu_item.xml");
        is_xml_ready = true;
    }
    
    m_text                    = new CUIStatic();
    m_text->SetAutoDelete    ( true );
    AttachChild                ( m_text );
    CUIXmlInit::InitStatic    ( uiXml, "cell_item_text", 0, m_text );
    m_text->Show            ( false );

/*    m_mark                    = new CUIStatic();
    m_mark->SetAutoDelete    ( true );
    AttachChild                ( m_mark );
    CUIXmlInit::InitStatic    ( uiXml, "cell_item_mark", 0, m_mark );
    m_mark->Show            ( false );*/

    m_upgrade                = new CUIStatic();
    m_upgrade->SetAutoDelete( true );
    AttachChild                ( m_upgrade );
    CUIXmlInit::InitStatic    ( uiXml, "cell_item_upgrade", 0, m_upgrade );
    m_upgrade_pos            = m_upgrade->GetWndPos();
    m_upgrade->Show            ( false );

    m_pConditionState = new CUIProgressBar();
    m_pConditionState->SetAutoDelete(true);
    AttachChild(m_pConditionState);
    CUIXmlInit::InitProgressBar(uiXml, "condition_progess_bar", 0, m_pConditionState);
    m_pConditionState->Show(true);

    m_custom_text = new CUIStatic();
    m_custom_text->SetAutoDelete(true);
    AttachChild(m_custom_text);
    CUIXmlInit::InitStatic(uiXml, "cell_item_custom_text", 0, m_custom_text);
    m_custom_text_pos = m_custom_text->GetWndPos();
    m_custom_text->Show(false);

    m_custom_mark = new CUIStatic();
    m_custom_mark->SetAutoDelete(true);
    AttachChild(m_custom_mark);
    CUIXmlInit::InitStatic(uiXml, "cell_item_custom_mark", 0, m_custom_mark);
    m_custom_mark_pos = m_custom_mark->GetWndPos();
    m_custom_mark->Show(false);
}

void CUICellItem::Draw()
{    
    m_drawn_frame        = Device.dwFrame;
    
    inherited::Draw();
    if(m_custom_draw) 
        m_custom_draw->OnDraw(this);
};

void CUICellItem::Update()
{
    EnableHeading(m_pParentList->GetVerticalPlacement());
    if(Heading())
    {
        SetHeading            ( 90.0f * (PI/180.0f) );
        SetHeadingPivot        (Fvector2().set(0.0f,0.0f), Fvector2().set(0.0f,GetWndSize().y), true);
    }else
        ResetHeadingPivot    ();

    inherited::Update();
    
    if ( CursorOverWindow() )
    {
        Frect clientArea;
        m_pParentList->GetClientArea(clientArea);
        Fvector2 cp            = GetUICursor().GetCursorPosition();
        if(clientArea.in(cp))
            GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_FOCUSED_UPDATE, nullptr);
    }
    
    PIItem item = (PIItem)m_pData;
    if ( item )
    {
        m_has_upgrade = item->has_any_upgrades();

//        Fvector2 size      = GetWndSize();
//        Fvector2 up_size = m_upgrade->GetWndSize();
//        pos.x = size.x - up_size.x - 4.0f;
        Fvector2 pos;
        pos.set( m_upgrade_pos );
        if ( ChildsCount() )
        {
            pos.x += m_text->GetWndSize().x + 2.0f;
        }
        m_upgrade->SetWndPos( pos );
    }
    m_upgrade->Show( m_has_upgrade );
    UpdateCustomMarksAndText();
}

void CUICellItem::UpdateCustomMarksAndText() {
    PIItem item = (PIItem)m_pData;
    if (item) {
        m_with_custom_text = item->m_custom_text != nullptr;
        m_with_custom_mark = item->m_custom_mark;
        if (m_with_custom_text) {
            Fvector2 pos;
            pos.set(m_custom_text_pos);
            Fvector2 size = GetWndSize();
            Fvector2 up_size = m_custom_text->GetWndSize();
            pos.x = size.x - up_size.x - 4.0f;// making pos at right-end of cell
            pos.y = size.y - up_size.y - 4.0f;// making pos at bottom-end of cell
            m_custom_text->SetWndPos(pos);
            m_custom_text->TextItemControl()->SetTextST(*item->m_custom_text);

            if (item->m_custom_text_clr_inv != 0) {
                //CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
                //if (pGameSP && pGameSP->ActorMenu().IsShown())// Hrust: used for pick_up_item on CUICellItem class
                {
                    m_custom_text->TextItemControl()->SetTextColor(item->m_custom_text_clr_inv);
                }
                /*else {
                    m_custom_text->SetTextColor(item->m_custom_text_clr_hud);
                }*/
            }
            if (item->m_custom_text_font != nullptr) {
                m_custom_text->TextItemControl()->SetFont(item->m_custom_text_font);
            }

            m_custom_text->SetTextOffset(item->m_custom_text_offset.x, item->m_custom_text_offset.y);
        }
        if (m_with_custom_mark)
        {
            Fvector2 pos;
            pos.set(m_custom_mark_pos);
            Fvector2 size = GetWndSize();
            Fvector2 up_size = m_custom_mark->GetWndSize();
            pos.x = size.x - up_size.x - 4.0f;// making pos at right-end of cell
            pos.y = size.y - up_size.y - 4.0f;// making pos at bottom-end of cell
            m_custom_mark->SetWndPos(pos);

            if (item->m_custom_mark_size.x > 0.f && item->m_custom_mark_size.y > 0.f)
            {
                m_custom_mark->SetWndSize(item->m_custom_mark_size);
            }
            else if (item->m_custom_mark_size.x < 0.f || item->m_custom_mark_size.y < 0.f)
            {
                R_ASSERT("item_custom_mark_size < 0.f || item_custom_mark_size < 0.f");
            }

            if (item->m_custom_mark_texture != nullptr)
            {
                m_custom_mark->InitTextureEx(item->m_custom_mark_texture.c_str());
            }

            if (item->m_custom_mark_clr != 0)
            {
                m_custom_mark->SetTextureColor(item->m_custom_mark_clr);
            }

            m_custom_mark->SetTextureOffset(item->m_custom_mark_offset.x, item->m_custom_mark_offset.y);
        }
    }
    m_custom_text->Show(m_with_custom_text);
    m_custom_mark->Show(m_with_custom_mark);
}

bool CUICellItem::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
    if ( mouse_action == WINDOW_LBUTTON_DOWN )
    {
        GetMessageTarget()->SendMessage( this, DRAG_DROP_ITEM_LBUTTON_CLICK, nullptr );
        GetMessageTarget()->SendMessage( this, DRAG_DROP_ITEM_SELECTED, nullptr );
        m_mouse_selected_item = this;
        return false;
    }
    else if ( mouse_action == WINDOW_MOUSE_MOVE )
    {
        if ( pInput->iGetAsyncBtnState(0) && m_mouse_selected_item && m_mouse_selected_item == this )
        {
            GetMessageTarget()->SendMessage( this, DRAG_DROP_ITEM_DRAG, nullptr );
            return true;
        }
    }
    else if ( mouse_action == WINDOW_LBUTTON_DB_CLICK )
    {
        GetMessageTarget()->SendMessage( this, DRAG_DROP_ITEM_DB_CLICK, nullptr );
        return true;
    }
    else if ( mouse_action == WINDOW_RBUTTON_DOWN )
    {
        GetMessageTarget()->SendMessage( this, DRAG_DROP_ITEM_RBUTTON_CLICK, nullptr );
        return true;
    }
    
    m_mouse_selected_item = nullptr;
    return false;
};

bool CUICellItem::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
    if (WINDOW_KEY_PRESSED == keyboard_action)
    {
        if (GetAccelerator() == dik)
        {
            GetMessageTarget()->SendMessage(this, DRAG_DROP_ITEM_DB_CLICK, nullptr);
            return        true;
        }
    }
    return inherited::OnKeyboardAction(dik, keyboard_action);
}

CUIDragItem* CUICellItem::CreateDragItem()
{
    CUIDragItem* tmp;
    tmp = new CUIDragItem(this);
    Frect r;
    GetAbsoluteRect(r);

    if( m_UIStaticItem.GetFixedLTWhileHeading() )
    {
        float t1,t2;
        t1                = r.width();
        t2                = r.height()*UI().get_current_kx();

        Fvector2 cp = GetUICursor().GetCursorPosition();

        r.x1            = (cp.x-t2/2.0f);
        r.y1            = (cp.y-t1/2.0f);
        r.x2            = r.x1 + t2;
        r.y2            = r.y1 + t1;
    }
    tmp->Init(GetShader(), r, GetUIStaticItem().GetTextureRect());
    return tmp;
}

void CUICellItem::SetOwnerList(CUIDragDropListEx* p)    
{
    m_pParentList = p;
    UpdateConditionProgressBar();
}

void CUICellItem::UpdateConditionProgressBar()
{
    if (!m_pConditionState)
        return;

    if(m_pParentList && m_pParentList->GetConditionProgBarVisibility())
    {
        PIItem itm = (PIItem)m_pData;
        if (itm && itm->IsUsingCondition())
        {
            Ivector2 itm_grid_size = GetGridSize();
            if(m_pParentList->GetVerticalPlacement())
                std::swap(itm_grid_size.x, itm_grid_size.y);

            Ivector2 cell_size = m_pParentList->CellSize();
            Ivector2 cell_space = m_pParentList->CellsSpacing();
            float x = 1.f;
            float y = itm_grid_size.y * (cell_size.y + cell_space.y) - m_pConditionState->GetHeight() - 2.f;

            m_pConditionState->SetWndPos(Fvector2().set(x,y));
            m_pConditionState->SetProgressPos(iCeil(itm->GetCondition()*13.0f)/13.0f);
            m_pConditionState->Show(true);
            return;
        }
    }
    m_pConditionState->Show(false);
}

bool CUICellItem::EqualTo(CUICellItem* itm)
{
    return (m_grid_size.x==itm->GetGridSize().x) && (m_grid_size.y==itm->GetGridSize().y);
}

u32 CUICellItem::ChildsCount()
{
    return (u32)m_childs.size();
}

void CUICellItem::PushChild(CUICellItem* c)
{
    R_ASSERT(c->ChildsCount()==0);
    VERIFY                (this!=c);
    m_childs.push_back    (c);
    UpdateItemText        ();
}

CUICellItem* CUICellItem::PopChild(CUICellItem* needed)
{
    CUICellItem* itm    = m_childs.back();
    m_childs.pop_back    ();
    
    if(needed)
    {    
      if(itm!=needed)
        std::swap        (itm->m_pData, needed->m_pData);
    }else
    {
        std::swap        (itm->m_pData, m_pData);
    }
    UpdateItemText        ();
    R_ASSERT            (itm->ChildsCount()==0);
    itm->SetOwnerList    (nullptr);
    return                itm;
}

bool CUICellItem::HasChild(CUICellItem* item)
{
    return (m_childs.end() != std::find(m_childs.begin(), m_childs.end(), item) );
}

void CUICellItem::UpdateItemText()
{
    if ( ChildsCount() )
    {
        string32    str;
        xr_sprintf( str, "x%d", ChildsCount()+1 );
        m_text->TextItemControl()->SetText( str );
        m_text->Show( true );
    }
    else
    {
        m_text->TextItemControl()->SetText( "" );
        m_text->Show( false );
    }
}

void CUICellItem::Mark( bool status )
{
    m_cur_mark = status;
}

void CUICellItem::SetCustomDraw(ICustomDrawCellItem* c)
{
    if (m_custom_draw)
        xr_delete(m_custom_draw);
    m_custom_draw = c;
}

// -------------------------------------------------------------------------------------------------

CUIDragItem::CUIDragItem(CUICellItem* parent)
{
    m_custom_draw                    = nullptr;
    m_back_list                        = nullptr;
    m_pParent                        = parent;
    AttachChild                        (&m_static);
    Device.seqRender.Add            (this, REG_PRIORITY_LOW-5000);
    Device.seqFrame.Add                (this, REG_PRIORITY_LOW-5000);
    VERIFY                            (m_pParent->GetMessageTarget());
}

CUIDragItem::~CUIDragItem()
{
    Device.seqRender.Remove            (this);
    Device.seqFrame.Remove            (this);
    delete_data                        (m_custom_draw);
}

void CUIDragItem::SetCustomDraw(ICustomDrawDragItem* c)
{
    if (m_custom_draw)
        xr_delete(m_custom_draw);
    m_custom_draw = c;
}

void CUIDragItem::Init(const ui_shader& sh, const Frect& rect, const Frect& text_rect)
{
    SetWndRect                        (rect);
    m_static.SetShader                (sh);
    m_static.SetTextureRect            (text_rect);
    m_static.SetWndPos                (Fvector2().set(0.0f,0.0f));
    m_static.SetWndSize                (GetWndSize());
    m_static.TextureOn                ();
    m_static.SetTextureColor        (color_rgba(255,255,255,170));
    m_static.SetStretchTexture        (true);
    m_pos_offset.sub                (rect.lt, GetUICursor().GetCursorPosition());
}

bool CUIDragItem::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
    if(mouse_action == WINDOW_LBUTTON_UP)
    {
        m_pParent->GetMessageTarget()->SendMessage(m_pParent,DRAG_DROP_ITEM_DROP,nullptr);
        return true;
    }
    return false;
}

void CUIDragItem::OnRender()
{
    Draw            ();
}

void CUIDragItem::OnFrame()
{
    Update            ();
}

void CUIDragItem::Draw()
{
    Fvector2 tmp;
    tmp.sub                    (GetWndPos(), GetUICursor().GetCursorPosition());
    tmp.sub                    (m_pos_offset);
    tmp.mul                    (-1.0f);
    MoveWndDelta            (tmp);
    inherited::Draw            ();
    if(m_custom_draw) 
        m_custom_draw->OnDraw(this);
}

void CUIDragItem::SetBackList(CUIDragDropListEx* l)
{
    if(m_back_list)
        m_back_list->OnDragEvent(this, false);

    m_back_list                    = l;

    if(m_back_list)
        l->OnDragEvent            (this, true);
}

Fvector2 CUIDragItem::GetPosition()
{
    return Fvector2().add(m_pos_offset, GetUICursor().GetCursorPosition());
}

