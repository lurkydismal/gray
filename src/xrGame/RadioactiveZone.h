#pragma once
#include "CustomZone.h"
#include "../xrScripts/script_export_space.h"

class CRadioactiveZone : public CCustomZone
{
private:
    typedef    CCustomZone    inherited;
public:
    CRadioactiveZone(void);
    virtual ~CRadioactiveZone(void);

    virtual void Load                    (LPCSTR section);
    virtual void Affect                    (SZoneObjectInfo* O);
    virtual    void feel_touch_new            (CObject* O    );
    virtual    void UpdateWorkload            (u32    dt    );                // related to fast-mode optimizations
    virtual    BOOL feel_touch_contact        (CObject* O    );
            float nearest_shape_radius    (SZoneObjectInfo* O);

protected:
    virtual bool BlowoutState            ();
    DECLARE_SCRIPT_REGISTER_FUNCTION
};