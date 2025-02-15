#pragma once

template<typename TypeListElement>
void game_state_accumulator::init_acpv_list()
{
    static_assert(Loki::TL::is_Typelist<TypeListElement>::value,
        "Type must have a Loki type list type use _ADD_ACCUMULATIVE_STATE_ macro define");
    
    init_acpv_list<typename TypeListElement::Tail>();

    player_state_param*    tmp_obj_inst = new typename TypeListElement::Head::value_type(this);
    
    m_accumulative_values.insert(
        std::make_pair(
            TypeListElement::Head::value_id,
            tmp_obj_inst
        )
    );
}