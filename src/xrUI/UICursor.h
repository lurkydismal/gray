#pragma once
#include "ui_base.h"

class UI_API CUIStatic;

class UI_API CUICursor :    
    public pureRender,
    public pureScreenResolutionChanged
{
    bool            bVisible;
    Fvector2        vPos;
    Fvector2        vPrevPos;
    CUIStatic*        m_static;
    void            InitInternal                ();
public:
                    CUICursor                    ();
    virtual            ~CUICursor                    ();
    virtual void    OnRender                    ();
    
    Fvector2        GetCursorPositionDelta        ();

    Fvector2        GetCursorPosition            ();
    void            SetUICursorPosition            (Fvector2 pos);
    void            UpdateCursorPosition        (int _dx, int _dy);
    virtual void    OnScreenResolutionChanged    ();

    bool            IsVisible                    () {return bVisible;}
    void            Show();
    void            Hide                        () {bVisible = false;}
};
