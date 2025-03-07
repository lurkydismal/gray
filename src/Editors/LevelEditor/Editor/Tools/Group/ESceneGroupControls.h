#pragma once

class TUI_ControlGroupAdd: public TUI_CustomControl
{
    bool      AfterAppendCallback(TShiftState Shift, CCustomObject* obj);
public:
    TUI_ControlGroupAdd(int st, int act, ESceneToolBase* parent);
    virtual ~TUI_ControlGroupAdd(){}
    virtual bool Start  (TShiftState _Shift);
    virtual bool End    (TShiftState _Shift);
    virtual void Move   (TShiftState _Shift);
};
