#pragma once
#include "../state.h"

template<typename Object>
class CStateBurerShield : public CState<Object> 
{
private:
    typedef                CState<Object>    inherited;

public:
                        CStateBurerShield        (Object *obj);

    virtual    void        initialize                ();
    virtual    void        execute                    ();
    virtual void        finalize                ();
    virtual void        critical_finalize        ();
    virtual void        remove_links            (CObject* object_) { inherited::remove_links(object_);}

    virtual bool        check_start_conditions    ();
    virtual bool        check_completion        ();

private:
    TTime                m_last_shield_started;
    TTime                m_next_particle_allowed;
    float                m_shield_start_anim_length_sec;
    bool                m_started;
};

#include "burer_state_attack_shield_inline.h"

