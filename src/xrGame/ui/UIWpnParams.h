#pragma once

#include "../../xrUI/Widgets/UIWindow.h"
#include "../../xrUI/Widgets/UIDoubleProgressBar.h"

class CUIXml;
class CInventoryItem;

#include "../../xrScripts/script_export_space.h"

struct SLuaWpnParams;

class CUIWpnParams : 
    public CUIWindow 
{
public:
                            CUIWpnParams        ();
    virtual                    ~CUIWpnParams        ();

    void                     InitFromXml            (CUIXml& xml_doc);
    void                    SetInfo                (CInventoryItem* slot_wpn, CInventoryItem& cur_wpn);
    bool                     Check                (CInventoryItem& wpn_section);

protected:
    CUIDoubleProgressBar    m_progressAccuracy; // red or green
    CUIDoubleProgressBar    m_progressHandling;
    CUIDoubleProgressBar    m_progressDamage;
    CUIDoubleProgressBar    m_progressRPM;

    CUIStatic                m_icon_acc;
    CUIStatic                m_icon_dam;
    CUIStatic                m_icon_han;
    CUIStatic                m_icon_rpm;

    CUIStatic                m_stAmmo;
    CUITextWnd                m_textAccuracy;
    CUITextWnd                m_textHandling;
    CUITextWnd                m_textDamage;
    CUITextWnd                m_textRPM;
    CUITextWnd                m_textAmmoTypes;
    CUITextWnd                m_textAmmoUsedType;
    CUITextWnd                m_textAmmoCount;
    CUITextWnd                m_textAmmoCount2;
    CUIStatic                m_stAmmoType1;
    CUIStatic                m_stAmmoType2;
    CUIStatic                m_Prop_line;
};

// -------------------------------------------------------------------------------------------------

class CUIConditionParams : public CUIWindow 
{
public:
                            CUIConditionParams    ();
    virtual                    ~CUIConditionParams    ();

    void                     InitFromXml            (CUIXml& xml_doc);
    void                    SetInfo                (CInventoryItem const* slot_wpn, CInventoryItem const& cur_wpn);

protected:
    CUIDoubleProgressBar    m_progress; // red or green
    CUIStatic                m_text;
};
