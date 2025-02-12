#include "stdafx.h"
#include "UIMessageBox.h"
#include "../../xrUI/UIXmlInit.h"
#include "../../xrUI/Widgets/UI3tButton.h"
#include "../../xrUI/Widgets/UIEditBox.h"

CUIMessageBox::CUIMessageBox()
{
    m_UIButtonYesOk        = nullptr;
    m_UIButtonNo        = nullptr;
    m_UIButtonCancel    = nullptr;
    m_UIButtonCopy        = nullptr;
    m_UIStaticPicture    = nullptr;
    m_UIStaticText        = nullptr;

    m_UIEditPass        = nullptr;
    m_UIEditUserPass    = nullptr;
    m_UIEditURL            = nullptr;
    m_UIEditHost        = nullptr;
    m_UIStaticPass        = nullptr;
    m_UIStaticUserPass    = nullptr;
    m_UIStaticHost        = nullptr;
}

CUIMessageBox::~CUIMessageBox()
{
    Clear();
}

#define BUTTON_UP_OFFSET 75
#define BUTTON_WIDTH 140

void CUIMessageBox::Clear(){
    xr_delete(m_UIButtonYesOk);
    xr_delete(m_UIButtonNo);
    xr_delete(m_UIButtonCancel);
    xr_delete(m_UIButtonCopy);
    xr_delete(m_UIStaticPicture);
    xr_delete(m_UIStaticText);
    xr_delete(m_UIEditPass);
    xr_delete(m_UIEditUserPass);
    xr_delete(m_UIEditURL);
    xr_delete(m_UIEditHost);
    xr_delete(m_UIStaticPass);
    xr_delete(m_UIStaticUserPass);
    xr_delete(m_UIStaticHost);
}

bool CUIMessageBox::OnMouseAction(float x, float y, EUIMessages mouse_action)
{
    return inherited::OnMouseAction(x, y, mouse_action);
}

void CUIMessageBox::InitMessageBox(LPCSTR box_template)
{
    Clear                            ();
    CUIXml                            uiXml;
    uiXml.Load                        (CONFIG_PATH, UI_PATH, "message_box.xml");
    CUIXmlInit                        xml_init;

    string512 str;

    xr_strconcat(str,box_template,":picture");
    if (uiXml.NavigateToNode(str,0)){
        m_UIStaticPicture                        = new CUIStatic();AttachChild(m_UIStaticPicture);
        xml_init.InitStatic                        (uiXml, str, 0, m_UIStaticPicture);
    }

    xr_strconcat(str,box_template,":message_text");
    if (uiXml.NavigateToNode(str,0)){
        m_UIStaticText                        = new CUITextWnd();
        AttachChild                            (m_UIStaticText);
        xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticText);
    }

    xr_strcpy        (str,box_template);
    xml_init.InitStatic                        (uiXml, str, 0, this);

    LPCSTR _type = uiXml.ReadAttrib(str, 0, "type", nullptr);
    R_ASSERT4(_type, "Please specify type for message box", str, uiXml.m_xml_file_name);

    if (!_type)       // Assign this if we're debugging engine
        _type = "ok"; // and know what we are doing

    m_eMessageBoxStyle    = MESSAGEBOX_OK;
    if(0==_stricmp(_type,"ok")){
        m_eMessageBoxStyle    = MESSAGEBOX_OK;
    }else
    if(0==_stricmp(_type,"yes_no")){
        m_eMessageBoxStyle    = MESSAGEBOX_YES_NO;
    }else
    if(0==_stricmp(_type,"yes_no_cancel")){
        m_eMessageBoxStyle    = MESSAGEBOX_YES_NO_CANCEL;
    }else
    if(0==_stricmp(_type,"yes_no_copy")){
        m_eMessageBoxStyle    = MESSAGEBOX_YES_NO_COPY;
    }else
    if(0==_stricmp(_type,"direct_ip")){
        m_eMessageBoxStyle    = MESSAGEBOX_DIRECT_IP;
    }else
    if(0==_stricmp(_type,"ra_login")){
        m_eMessageBoxStyle    = MESSAGEBOX_RA_LOGIN;
    }else
    if(0==_stricmp(_type,"password")){
        m_eMessageBoxStyle    = MESSAGEBOX_PASSWORD;
    }else
    if(0==_stricmp(_type,"quit_windows")){
        m_eMessageBoxStyle    = MESSAGEBOX_QUIT_WINDOWS;
    }else 
    if(0==_stricmp(_type,"quit_game")){
        m_eMessageBoxStyle    = MESSAGEBOX_QUIT_GAME;
    }else 
        if(0==_stricmp(_type,"info")){
            m_eMessageBoxStyle    = MESSAGEBOX_INFO;
        };
    

    switch (m_eMessageBoxStyle){

        case MESSAGEBOX_OK:{
            xr_strconcat(str,box_template,":button_ok");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);
        }break;
        case MESSAGEBOX_INFO:
        {
        }break;

        case MESSAGEBOX_DIRECT_IP:
            xr_strconcat(str,box_template,":cap_host");
            m_UIStaticHost                        = new CUITextWnd();
            AttachChild                            (m_UIStaticHost);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticHost);

            xr_strconcat(str,box_template,":edit_host");
            m_UIEditHost                        = new CUIEditBox();
            AttachChild                            (m_UIEditHost);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditHost);            

            xr_strconcat(str,box_template,":cap_password");
            m_UIStaticPass                        = new CUITextWnd();
            AttachChild                            (m_UIStaticPass);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticPass);

            xr_strconcat(str,box_template,":edit_password");
            m_UIEditPass                        = new CUIEditBox();
            AttachChild                            (m_UIEditPass);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditPass);

            xr_strconcat(str,box_template,":button_yes");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);

            xr_strconcat(str,box_template,":button_no");
            m_UIButtonNo                        = new CUI3tButton();
            AttachChild                            (m_UIButtonNo);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonNo);
            //m_message_box_yes_no->func_on_ok = CUIWndCallback::void_function( this, &CUIActorMenu::OnMesBoxYes );

            break;
        case MESSAGEBOX_PASSWORD:{
            xr_strconcat(str,box_template,":cap_user_password");
            m_UIStaticUserPass                        = new CUITextWnd();
            AttachChild                            (m_UIStaticUserPass);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticUserPass);

            xr_strconcat(str,box_template,":cap_password");
            m_UIStaticPass                        = new CUITextWnd();
            AttachChild                            (m_UIStaticPass);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticPass);

            xr_strconcat(str,box_template,":edit_user_password");
            m_UIEditUserPass                    = new CUIEditBox();
            AttachChild                            (m_UIEditUserPass);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditUserPass);

            xr_strconcat(str,box_template,":edit_password");
            m_UIEditPass                        = new CUIEditBox();
            AttachChild                            (m_UIEditPass);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditPass);

            xr_strconcat(str,box_template,":button_yes");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);

            xr_strconcat(str,box_template,":button_no");
            m_UIButtonNo                        = new CUI3tButton();
            AttachChild                            (m_UIButtonNo);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonNo);
        }break;

        case MESSAGEBOX_RA_LOGIN:
            xr_strconcat(str,box_template,":cap_login");
            m_UIStaticUserPass                    = new CUITextWnd();
            AttachChild                            (m_UIStaticUserPass);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticUserPass);

            xr_strconcat(str,box_template,":cap_password");
            m_UIStaticPass                        = new CUITextWnd();
            AttachChild                            (m_UIStaticPass);
            xml_init.InitTextWnd                (uiXml, str, 0, m_UIStaticPass);

            xr_strconcat(str,box_template,":edit_login");
            m_UIEditUserPass                    = new CUIEditBox();
            AttachChild                            (m_UIEditUserPass);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditUserPass);

            xr_strconcat(str,box_template,":edit_password");
            m_UIEditPass                        = new CUIEditBox();
            AttachChild                            (m_UIEditPass);
            xml_init.InitEditBox                (uiXml, str, 0, m_UIEditPass);

            m_UIEditUserPass->SetNextFocusCapturer(m_UIEditPass);
            m_UIEditPass->SetNextFocusCapturer(m_UIEditUserPass);
            m_UIEditUserPass->CaptureFocus(true);

        case MESSAGEBOX_QUIT_WINDOWS:
        case MESSAGEBOX_QUIT_GAME:
        case MESSAGEBOX_YES_NO:
            {
            xr_strconcat(str,box_template,":button_yes");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);

            xr_strconcat(str,box_template,":button_no");
            m_UIButtonNo                        = new CUI3tButton();
            AttachChild                            (m_UIButtonNo);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonNo);
        }break;

        case MESSAGEBOX_YES_NO_CANCEL:{
            xr_strconcat(str,box_template,":button_yes");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);

            xr_strconcat(str,box_template,":button_no");
            m_UIButtonNo                        = new CUI3tButton();
            AttachChild                            (m_UIButtonNo);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonNo);

            xr_strconcat(str,box_template,":button_cancel");
            m_UIButtonCancel                    = new CUI3tButton();
            AttachChild                            (m_UIButtonCancel);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonCancel);
        }break;

        case MESSAGEBOX_YES_NO_COPY:{
            xr_strconcat(str,box_template,":button_yes");
            m_UIButtonYesOk                        = new CUI3tButton();
            AttachChild                            (m_UIButtonYesOk);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonYesOk);

            xr_strconcat(str,box_template,":button_no");
            m_UIButtonNo                        = new CUI3tButton();
            AttachChild                            (m_UIButtonNo);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonNo);

            xr_strconcat(str,box_template,":button_copy");
            m_UIButtonCopy                        = new CUI3tButton();
            AttachChild                            (m_UIButtonCopy);
            xml_init.Init3tButton                (uiXml, str, 0, m_UIButtonCopy);
            
            xr_strconcat(str,box_template,":edit_url");
            if ( uiXml.NavigateToNode(str) )
            {
                m_UIEditURL                        = new CUIEditBox();
                AttachChild                        (m_UIEditURL);
                xml_init.InitEditBox            (uiXml, str, 0, m_UIEditURL);
//                m_UIEditURL->read_only
            }
        }break;
    };
}


void CUIMessageBox::OnYesOk()
{
    switch (m_eMessageBoxStyle)
    {
    case         MESSAGEBOX_OK:
    case         MESSAGEBOX_INFO:
        GetMessageTarget()->SendMessage(m_UIButtonYesOk, MESSAGE_BOX_OK_CLICKED);
        GetMessageTarget()->SendMessage(this, MESSAGE_BOX_OK_CLICKED);
        break;

    case         MESSAGEBOX_DIRECT_IP: 
    case         MESSAGEBOX_RA_LOGIN: 
    case         MESSAGEBOX_YES_NO_CANCEL:
    case         MESSAGEBOX_YES_NO_COPY:
    case         MESSAGEBOX_PASSWORD:
    case         MESSAGEBOX_YES_NO:
        GetMessageTarget()->SendMessage(m_UIButtonYesOk, MESSAGE_BOX_YES_CLICKED);
        GetMessageTarget()->SendMessage(this, MESSAGE_BOX_YES_CLICKED);
        break;
    case         MESSAGEBOX_QUIT_WINDOWS:
        GetMessageTarget()->SendMessage(this, MESSAGE_BOX_QUIT_WIN_CLICKED);
        break;
    case         MESSAGEBOX_QUIT_GAME:
        GetMessageTarget()->SendMessage(this, MESSAGE_BOX_QUIT_GAME_CLICKED);
        break;
    };
}

void CUIMessageBox::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
    if ( msg == BUTTON_CLICKED )
    {
        switch(m_eMessageBoxStyle)
        {
        case MESSAGEBOX_OK:
            if(pWnd == m_UIButtonYesOk)
            {
                OnYesOk();
            }
            break;
        case MESSAGEBOX_DIRECT_IP:
        case MESSAGEBOX_RA_LOGIN:
        case MESSAGEBOX_PASSWORD:
        case MESSAGEBOX_YES_NO:
        case MESSAGEBOX_QUIT_GAME:
        case MESSAGEBOX_QUIT_WINDOWS:
            if(pWnd == m_UIButtonYesOk)
            {
                OnYesOk();
            }
            else if(pWnd == m_UIButtonNo)
            {
                GetMessageTarget()->SendMessage(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
                GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
            }
            break;
        case MESSAGEBOX_YES_NO_CANCEL:
            if(pWnd == m_UIButtonYesOk)
            {
                OnYesOk();
            }
            else if(pWnd == m_UIButtonNo)
            {
                GetMessageTarget()->SendMessage(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
                GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
            }
            else if(pWnd == m_UIButtonCancel)
            {
                GetMessageTarget()->SendMessage(m_UIButtonCancel, MESSAGE_BOX_CANCEL_CLICKED);
                GetMessageTarget()->SendMessage(this, MESSAGE_BOX_CANCEL_CLICKED);
            }
            break;
        case MESSAGEBOX_YES_NO_COPY:
            if(pWnd == m_UIButtonYesOk)
            {
                OnYesOk();
            }
            else if(pWnd == m_UIButtonNo)
            {
                GetMessageTarget()->SendMessage(m_UIButtonNo, MESSAGE_BOX_NO_CLICKED);
                GetMessageTarget()->SendMessage(this, MESSAGE_BOX_NO_CLICKED);
            }
            else if(pWnd == m_UIButtonCopy)
            {
                GetMessageTarget()->SendMessage(m_UIButtonCopy, MESSAGE_BOX_COPY_CLICKED);
                GetMessageTarget()->SendMessage(this, MESSAGE_BOX_COPY_CLICKED);
            }
            break;
        };
    };
    inherited::SendMessage(pWnd, msg, pData);
}

void CUIMessageBox::SetText(LPCSTR str)
{
    m_UIStaticText->SetTextST(str);
}

LPCSTR CUIMessageBox::GetText()
{
    return m_UIStaticText->GetText();
}

LPCSTR CUIMessageBox::GetHost()
{
    if (m_UIEditHost)
    {
        m_ret_val.clear();
        xr_string tmp= m_UIEditHost->GetText();
        xr_string::size_type pos = tmp.find(":");

        if (xr_string::npos != pos)
        {
            m_ret_val.assign(tmp.begin(), tmp.begin()+pos);
            tmp.erase(tmp.begin(), tmp.begin()+pos + 1);

            m_ret_val += "/port=";            
        }
        m_ret_val += tmp;

        return m_ret_val.c_str();
    }
    else 
        return nullptr;
}

LPCSTR CUIMessageBox::GetPassword()
{
    if (m_UIEditPass)
        return m_UIEditPass->GetText();
    else 
        return nullptr;
}

LPCSTR CUIMessageBox::GetUserPassword()
{
    if (m_UIEditUserPass)
        return m_UIEditUserPass->GetText();
    else 
        return nullptr;
}

void CUIMessageBox::SetTextEditURL(LPCSTR text)
{
    if (m_UIEditURL)
    {
        m_UIEditURL->SetText( text );
    }
}

LPCSTR CUIMessageBox::GetTextEditURL()
{
    if (m_UIEditURL)
    {
        return m_UIEditURL->GetText();
    }
    return nullptr;
}

void CUIMessageBox::SetUserPasswordMode(bool b)
{
    if(m_UIEditUserPass)
        m_UIEditUserPass->Show(b);
    if(m_UIStaticUserPass)
        m_UIStaticUserPass->Show(b);
}
void CUIMessageBox::SetPasswordMode(bool b)
{
    if(m_UIEditPass)
        m_UIEditPass->Show(b);
    if(m_UIStaticPass)
        m_UIStaticPass->Show(b);
}
