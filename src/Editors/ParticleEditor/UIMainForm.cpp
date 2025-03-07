#include "stdafx.h"

#include "../xrECore/Editor/EditorChooseEvents.h"
UIMainForm* MainForm = nullptr;
UIMainForm::UIMainForm()
{
#ifdef DEBUG
    xrLogger::EnableFastDebugLog();
#endif
    EnableReceiveCommands();
    if (!ExecCommand(COMMAND_INITIALIZE, (u32)0, (u32)0)) 
    {
        xrLogger::FlushLog();
        exit(-1);
    }
    ExecCommand(COMMAND_UPDATE_GRID);
    ExecCommand(COMMAND_RENDER_FOCUS);
    FillChooseEvents();
    m_TopBar = new UITopBarForm();
    m_Render = new UIRenderForm();
    m_MainMenu = new UIMainMenuForm();
    m_LeftBar = new UILeftBarForm();
    m_RightBar = new UIRightBarForm();
}

UIMainForm::~UIMainForm()
{
    ClearChooseEvents();
    xr_delete(m_RightBar);
    xr_delete(m_LeftBar);
    xr_delete(m_MainMenu);
    xr_delete(m_Render);
    xr_delete(m_TopBar);
    ExecCommand(COMMAND_DESTROY, (u32)0, (u32)0);
}

void UIMainForm::Draw()
{
    m_MainMenu->Draw();
    m_TopBar->Draw();
    m_RightBar->Draw();
    m_LeftBar->Draw();
    m_Render->Draw();
}

bool UIMainForm::Frame()
{
    if(UI)  return UI->Idle();
    return false;
}
