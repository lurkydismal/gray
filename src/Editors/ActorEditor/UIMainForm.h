#pragma once
#include "../xrECore/Editor/UIToolbar.h"

class UIMainForm :public IEditorWnd
{
public:
    UIMainForm();
    virtual ~UIMainForm();
    virtual void Draw();
    bool Frame();
    IC UILeftBarForm* GetLeftBarForm() {return m_LeftBar;}
    IC UITopBarForm* GetTopBarForm() { return m_TopBar; }
    IC UIKeyForm* GetKeyForm() { return m_KeyForm; }

    IC UIRenderForm* GetRenderForm() { return m_Render; }

private:
    UITopBarForm *m_TopBar;
    UIRenderForm* m_Render;
    UIMainMenuForm* m_MainMenu;
    UILeftBarForm* m_LeftBar;
    UIKeyForm* m_KeyForm;
    CUIToolbar* ToolBar = nullptr;
};
extern UIMainForm* MainForm;
