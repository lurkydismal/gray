#pragma once

//#include "../states/monster_state_rest_idle.h"
#include "../states/state_custom_action.h"

#define TEMPLATE_SPECIALIZATION template <\
    typename _Object\
>

#define CStateCaptureJumpBloodsuckerAbstract CStateCaptureJumpBloodsucker<_Object>


TEMPLATE_SPECIALIZATION
CStateCaptureJumpBloodsuckerAbstract::CStateCaptureJumpBloodsucker(_Object *obj) : inherited(obj)
{
    this->add_state(eStateCustom,                new CStateMonsterCustomAction<_Object>         (obj));
}

TEMPLATE_SPECIALIZATION
CStateCaptureJumpBloodsuckerAbstract::~CStateCaptureJumpBloodsucker    ()
{
}

TEMPLATE_SPECIALIZATION
void CStateCaptureJumpBloodsuckerAbstract::execute()
{
    // check alife control
    this->select_state    (eStateCustom);

    this->get_state_current()->execute();
    this->prev_substate = this->current_substate;
}
TEMPLATE_SPECIALIZATION
void CStateCaptureJumpBloodsuckerAbstract::setup_substates()
{
    state_ptr state = this->get_state_current();
    if (this->current_substate == eStateCustom) {
        SStateDataAction data;

        data.action        = ACT_STAND_IDLE;
        data.time_out    = 0;            // do not use time out
        /*data.sound_type    = MonsterSound::eMonsterSoundIdle;
        data.sound_delay = object->db().m_dwIdleSndDelay;
        */
        state->fill_data_with(&data, sizeof(SStateDataAction));

        return;
    }
}
#undef TEMPLATE_SPECIALIZATION
#undef CStateCaptureJumpBloodsuckerAbstract
