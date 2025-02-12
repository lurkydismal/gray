#pragma once
#include "../basemonster/base_monster.h"
#include "../../../../xrScripts/script_export_space.h"

class CCat : public CBaseMonster{
    typedef        CBaseMonster    inherited;
public:
                    CCat                ();
    virtual            ~CCat                ();    

    virtual void    Load                (LPCSTR section);
    virtual void    reinit                ();

    virtual    void    UpdateCL            ();

    virtual void    CheckSpecParams        (u32 spec_params);

            void    try_to_jump            ();

    virtual    void    HitEntityInJump        (const CEntity *pEntity);

    virtual    char*    get_monster_class_name () { return (char*) "cat"; }


    DECLARE_SCRIPT_REGISTER_FUNCTION
};