#pragma once
#include "monster_event_manager_defs.h"

using typeEvent = xr_delegate<void(IEventData*)>;

class CMonsterEventManager {
    
    // delayed remove
    struct event_struc {
        typeEvent    delegate;
        bool        need_remove;
        
        event_struc        (typeEvent e) : delegate(e) {need_remove = false;}
    };
    
    struct pred_remove {
        bool operator() (const event_struc &param) {return param.need_remove;}
    };

    using EVENT_VECTOR = xr_vector<event_struc>;
    using EVENT_VECTOR_IT = EVENT_VECTOR::iterator;

    using EVENT_MAP = xr_map<EEventType, EVENT_VECTOR>;
    using EVENT_MAP_IT = EVENT_MAP::iterator;

    EVENT_MAP        m_event_storage;
public:
                CMonsterEventManager    ();
                ~CMonsterEventManager    ();

    void        add_delegate            (EEventType event, typeEvent delegate);
    void        remove_delegate            (EEventType event, typeEvent delegate);
    
    void        raise                    (EEventType, IEventData *data = 0);

private:
    void        clear        ();
};