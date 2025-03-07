#pragma once
#include "../../xrUI/Widgets/UIDialogWnd.h"
#include "../../xrUI/Widgets/UIWndCallback.h"

class CUIXml;
class CUIGameLog;
class CUIEditBox;
class CUITextWnd;

class CUIChatWnd: public CUIDialogWnd, public CUIWndCallback
{
    typedef CUIDialogWnd inherited;

public:
                        CUIChatWnd            ();
    virtual void        Show                (bool status);
    virtual bool        NeedCursor            () {return false;}
    void                Init                (CUIXml& uiXml);
    void                SetEditBoxPrefix    (LPCSTR prefix);
    void                ChatToAll            (bool b) { sendNextMessageToAll = b; }
    void                PendingMode            (bool const is_pending_mode);
    virtual bool        NeedCursor            ()const {return false;}
    virtual void        SendMessage            (CUIWindow* pWnd, s16 msg, void* pData = NULL);


protected:
    CUIEditBox*            UIEditBox;
    CUITextWnd*            UIPrefix;

    bool                sendNextMessageToAll;
    bool                pendingGameMode;
    
    Frect                pending_prefix_rect;
    Frect                pending_edit_rect;
    
    Frect                inprogress_prefix_rect;
    Frect                inprogress_edit_rect;

    void         OnChatCommit        (CUIWindow* w, void* d);
    void         OnChatCancel        (CUIWindow* w, void* d);
};
