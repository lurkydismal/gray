////////////////////////////////////////////////////////////////////////////
//    Module         : UIHelper.cpp
//    Created     : 17.01.2008
//    Author        : Evgeniy Sokolov
//    Description : UI Helper class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIHelper.h"
#include "UIXmlInit.h"

#include "Widgets/UIProgressBar.h"
#include "Widgets/UIFrameLineWnd.h"
#include "Widgets/UIFrameWindow.h"
#include "Widgets/UI3tButton.h"
#include "Widgets/UICheckButton.h"
#include "Widgets/UIHint.h"
#include "Widgets/UIEditBox.h"

CUIStatic* UIHelper::CreateStatic( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUIStatic* ui            = new CUIStatic();
    if(parent)
    {
        parent->AttachChild    ( ui );
        ui->SetAutoDelete    ( true );
    }
    CUIXmlInit::InitStatic    ( xml, ui_path, 0, ui );
    return ui;
}

CUITextWnd* UIHelper::CreateTextWnd( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUITextWnd* ui            = new CUITextWnd();
    if(parent)
    {
        parent->AttachChild    ( ui );
        ui->SetAutoDelete    ( true );
    }
    CUIXmlInit::InitTextWnd    ( xml, ui_path, 0, ui );
    return ui;
}

CUIEditBox* UIHelper::CreateEditBox( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUIEditBox* ui            = new CUIEditBox();
    if(parent)
    {
        parent->AttachChild    ( ui );
        ui->SetAutoDelete    ( true );
    }
    CUIXmlInit::InitEditBox    ( xml, ui_path, 0, ui );
    return ui;
}

CUIProgressBar* UIHelper::CreateProgressBar( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUIProgressBar* ui            = new CUIProgressBar();
    parent->AttachChild            ( ui );
    ui->SetAutoDelete            ( true );
    CUIXmlInit::InitProgressBar ( xml, ui_path, 0, ui );
    return ui;
}

CUIFrameLineWnd* UIHelper::CreateFrameLine( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUIFrameLineWnd* ui            = new CUIFrameLineWnd();
    if(parent)
    {
        parent->AttachChild        ( ui );
        ui->SetAutoDelete        ( true );
    }
    CUIXmlInit::InitFrameLine    ( xml, ui_path, 0, ui );
    return ui;
}

CUIFrameWindow* UIHelper::CreateFrameWindow( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUIFrameWindow* ui            = new CUIFrameWindow();
    if(parent)
    {
        parent->AttachChild        ( ui );
        ui->SetAutoDelete        ( true );
    }
    CUIXmlInit::InitFrameWindow    ( xml, ui_path, 0, ui );
    return ui;
}

CUI3tButton* UIHelper::Create3tButton( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUI3tButton* ui                = new CUI3tButton();
    parent->AttachChild            ( ui );
    ui->SetAutoDelete            ( true );
    CUIXmlInit::Init3tButton    ( xml, ui_path, 0, ui );
    return ui;
}

CUICheckButton* UIHelper::CreateCheck( CUIXml& xml, LPCSTR ui_path, CUIWindow* parent )
{
    CUICheckButton* ui            = new CUICheckButton();
    parent->AttachChild            ( ui );
    ui->SetAutoDelete            ( true );
    CUIXmlInit::InitCheck        ( xml, ui_path, 0, ui );
    return ui;
}

UIHint* UIHelper::CreateHint( CUIXml& xml, LPCSTR ui_path)
{
    UIHint* ui                    = new UIHint();
    ui->SetAutoDelete            ( true );
    ui->init_from_xml            ( xml, ui_path );
    return ui;
}