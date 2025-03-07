#include "StdAfx.h"

#include "UITrackBar.h"
#include "UI3tButton.h"
#include "UITextureMaster.h"
#include "UIXmlInit.h"
#include "../../xrEngine/xr_input.h"

#define DEF_CONTROL_HEIGHT        16.0f

CUITrackBar::CUITrackBar()
    : m_f_min(0),
      m_f_max(1),
      m_f_val(0),
      m_f_opt_backup_value(0),
     m_f_step(0.01f),
    m_b_is_float(true),
    m_b_invert(false)
{    
    m_pSlider                        = new CUI3tButton();            
    AttachChild                        (m_pSlider);        
    m_pSlider->SetAutoDelete        (true);
    m_b_mouse_capturer                = false;
}

bool CUITrackBar::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
    CUIWindow::OnMouseAction(x, y, mouse_action);

    switch (mouse_action)
    {
    case WINDOW_MOUSE_MOVE:
        {
            if(m_bCursorOverWindow && m_b_mouse_capturer)
            {
                if (pInput->iGetAsyncBtnState(0))
                    UpdatePosRelativeToMouse();
            }
        }break;
    case WINDOW_LBUTTON_DOWN:
        {
            m_b_mouse_capturer = m_bCursorOverWindow;
            if(m_b_mouse_capturer)
                UpdatePosRelativeToMouse();
        }break;

    case WINDOW_LBUTTON_UP:
        {
            m_b_mouse_capturer = false;
        }
        break;
    case WINDOW_MOUSE_WHEEL_UP:
        {
            if(m_b_is_float)
            {
                m_f_val -= GetInvert()?-m_f_step:m_f_step;
                clamp(m_f_val, m_f_min, m_f_max);
            }
            else
            {
                m_i_val -= GetInvert()?-m_i_step:m_i_step;
                clamp(m_i_val, m_i_min, m_i_max);
            }
            GetMessageTarget()->SendMessage(this, BUTTON_CLICKED, nullptr);
            UpdatePos            ();
            OnChangedOptValue    ();
        }
        break;
    case WINDOW_MOUSE_WHEEL_DOWN:
        {
            if(m_b_is_float)
            {
                m_f_val += GetInvert()?-m_f_step:m_f_step;
                clamp(m_f_val, m_f_min, m_f_max);
            }
            else
            {
                m_i_val += GetInvert()?-m_i_step:m_i_step;
                clamp(m_i_val, m_i_min, m_i_max);
            }
            GetMessageTarget()->SendMessage(this, BUTTON_CLICKED, nullptr);
            UpdatePos();
            OnChangedOptValue    ();
        }
        break;
    };
    return true;
}

void CUITrackBar::InitTrackBar(Fvector2 pos, Fvector2 size)
{
    const float EditBoxSize = 20.f * UI().get_current_kx();

    CUIXml xml_doc;
    xml_doc.Load            (CONFIG_PATH, UI_PATH, "trackbar.xml");

    LPCSTR nodevalue_button = xml_doc.Read("button_texture_name", 0, "ui_inGame2_opt_slider_box");
    LPCSTR nodevalue_track    = xml_doc.Read("track_texture_name", 0, "ui_inGame2_opt_slider_bar");

    float                    item_height;
    float                    item_width;

    Fvector2 TrySize = size;

    if (m_b_use_editbox)
    {
        TrySize.x -= EditBoxSize * 1.4f;
    }

    InitIB(pos, TrySize);

    InitState                (S_Enabled, nodevalue_track);
    InitState                (S_Disabled, nodevalue_track);
    string128                name_button_e;
    xr_sprintf                (name_button_e, "%s%s", nodevalue_button, "_e");
    item_width                = CUITextureMaster::GetTextureWidth(name_button_e);
    item_height                = CUITextureMaster::GetTextureHeight(name_button_e);

    item_width                *= UI().get_current_kx();

    m_pSlider->InitButton    (Fvector2().set(0.0f, 0.0f), Fvector2().set(item_width, item_height) );            //size
    m_pSlider->InitTexture    (nodevalue_button);
    m_pSlider->m_background->SetStretchTexture(xml_doc.ReadInt("stretch", 0, TRUE));
    
    SetCurrentState(S_Enabled);

    if (m_b_use_editbox)
    {
        Fvector2 InitPos = { size.x - EditBoxSize, 0 };
        m_pEditBox = new CUIEditBox();
        m_pEditBox->SetParent(this);
        m_pEditBox->SetWndPos(InitPos);
        m_pEditBox->SetWndSize({ EditBoxSize + 10, EditBoxSize });
        m_pEditBox->TextItemControl()->SetFont(g_FontManager->GetFont("ui_font_arial_14"));
        m_pEditBox->SetText(xr_string::ToString(m_f_val).c_str());
    }
}    

void CUITrackBar::Draw()
{
    CUI_IB_FrameLineWnd::Draw();
    m_pSlider->Draw();

    if (m_pEditBox != nullptr)
        m_pEditBox->Draw();
}

void CUITrackBar::Update()
{
    CUIWindow::Update();

    if(m_b_mouse_capturer)
    {
        if(!pInput->iGetAsyncBtnState(0))
            m_b_mouse_capturer = false;
    }
}

void CUITrackBar::SetCurrentOptValue()
{
    CUIOptionsItem::SetCurrentOptValue();
    if(m_b_is_float)
        GetOptFloatValue    (m_f_val, m_f_min, m_f_max);
    else
        GetOptIntegerValue    (m_i_val, m_i_min, m_i_max);

    UpdatePos            ();
}

void CUITrackBar::SaveOptValue()
{
    CUIOptionsItem::SaveOptValue    ();
    if(m_b_is_float)
        SaveOptFloatValue            (m_f_val);
    else
        SaveOptIntegerValue            (m_i_val);
}

bool CUITrackBar::IsChangedOptValue() const
{
    if(m_b_is_float)
    {
        return !fsimilar(m_f_opt_backup_value, m_f_val); 
    }else
    {
        return (m_i_opt_backup_value != m_i_val);
    }
}

void CUITrackBar::SaveBackUpOptValue()
{
    CUIOptionsItem::SaveBackUpOptValue();

    if(m_b_is_float)
        m_f_opt_backup_value        = m_f_val;
    else
        m_i_opt_backup_value        = m_i_val;
}

void CUITrackBar::UndoOptValue()
{
    if(m_b_is_float)
        m_f_val            = m_f_opt_backup_value;
    else
        m_i_val            = m_i_opt_backup_value;

    UpdatePos            ();
    CUIOptionsItem::UndoOptValue();
}

void CUITrackBar::SetStep(float step)
{
    if(m_b_is_float)
        m_f_step    = step;
    else
        m_i_step    = iFloor(step);
}

void CUITrackBar::Enable(bool status)
{
    m_bIsEnabled                = status;
    SetCurrentState                (m_bIsEnabled?S_Enabled:S_Disabled);
    m_pSlider->Enable            (m_bIsEnabled);
}

void CUITrackBar::UpdatePosRelativeToMouse()
{
    float _bkf        = 0.0f;
    int _bki        = 0;
    if(m_b_is_float)
    {
        _bkf = m_f_val; 
    }else
    {
        _bki = m_i_val; 
    }


    float btn_width                = m_pSlider->GetWidth();
    float window_width            = GetWidth();        
    float fpos                    = cursor_pos.x;

    if( GetInvert() )
        fpos                    = window_width - fpos;

    if (fpos < btn_width/2)
        fpos = btn_width/2;
    else 
    if (fpos > window_width - btn_width/2)
        fpos = window_width - btn_width/2;

    float __fval;
    float __fmax    = (m_b_is_float)?m_f_max:(float)m_i_max;
    float __fmin    = (m_b_is_float)?m_f_min:(float)m_i_min;
    float __fstep    = (m_b_is_float)?m_f_step:(float)m_i_step;

    __fval                        = (__fmax - __fmin)*(fpos - btn_width/2)/(window_width - btn_width)+ __fmin;
    
    float _d    = (__fval-__fmin);
    
    float _v    = _d/__fstep;
    int _vi        = iFloor(_v);
    float _vf    = __fstep*_vi;
    
    if(_d-_vf>__fstep/2.0f)    
        _vf        += __fstep;

    __fval        = __fmin+_vf;
    
    clamp        (__fval, __fmin, __fmax);

    if(m_b_is_float)
        m_f_val    = __fval;
    else
        m_i_val    = iFloor(__fval);
    

    bool b_ch = false;
    if(m_b_is_float)
    {
        b_ch  = !fsimilar(_bkf, m_f_val); 
    }else
    {
        b_ch  =  (_bki != m_i_val);
    }

    if(b_ch)
        GetMessageTarget()->SendMessage(this, BUTTON_CLICKED, nullptr);

    UpdatePos    ();
    OnChangedOptValue    ();
}

void CUITrackBar::UpdatePos()
{
#ifdef DEBUG
    
    if(m_b_is_float)
        R_ASSERT2(m_f_val >= m_f_min && m_f_val <= m_f_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max" );
    else
        R_ASSERT2(m_i_val >= m_i_min && m_i_val <= m_i_max, "CUITrackBar::UpdatePos() - m_val >= m_min && m_val <= m_max" );

#endif

    float btn_width                = m_pSlider->GetWidth();
    float window_width            = GetWidth();        
    float free_space            = window_width - btn_width;
    Fvector2 pos                = m_pSlider->GetWndPos();
    
    float __fval    = (m_b_is_float)?m_f_val:(float)m_i_val;
    float __fmax    = (m_b_is_float)?m_f_max:(float)m_i_max;
    float __fmin    = (m_b_is_float)?m_f_min:(float)m_i_min;


    pos.x                        = (__fval - __fmin)*free_space/(__fmax - __fmin);
    if( GetInvert() )
        pos.x                    = free_space-pos.x;

    m_pSlider->SetWndPos        (pos);

    if (m_b_use_editbox)
    {
        m_pEditBox->SetText(xr_string::ToString(m_b_is_float ? m_f_val * 100 : m_i_val).c_str());
    }
}

void CUITrackBar::OnMessage(LPCSTR message)
{
    if (0 == xr_strcmp(message, "set_default_value"))
    {
        if(m_b_is_float)
            m_f_val = m_f_min + (m_f_max - m_f_min)/2.0f;
        else
            m_i_val = m_i_min + iFloor((m_i_max - m_i_min)/2.0f);

        UpdatePos();
    }
}

bool CUITrackBar::GetCheck()
{
    VERIFY(!m_b_is_float);
    return !!m_i_val;
}

void CUITrackBar::SetCheck(bool b)
{
    VERIFY(!m_b_is_float);
    m_i_val = (b)?m_i_max:m_i_min;
}

void CUITrackBar::SetOptIBounds(int imin, int imax)
{
    m_i_min                    = imin;
    m_i_max                    = imax;
    if(m_i_val<m_i_min || m_i_val>m_i_max)
    {
        clamp                    (m_i_val, m_i_min, m_i_max);
        OnChangedOptValue    ();
    }
}

void CUITrackBar::SetOptFBounds(float fmin, float fmax)
{
    m_f_min                    = fmin;
    m_f_max                    = fmax;
    if(m_i_val<m_i_min || m_i_val>m_i_max)
    {
        clamp                (m_f_val, m_f_min, m_f_max);
        OnChangedOptValue    ();
    }
}
