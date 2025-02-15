#ifndef ACCUMULATIVE_STATES_INCLUDED
#define ACCUMULATIVE_STATES_INCLUDED

namespace award_system
{

enum enum_accumulative_player_values
{
    acpv_command_switch_count    =    0x00,
    acpv_kill_in_raw,
    acpv_death_count,
    acpv_spots,
    acpv_my_team_win_score,
    acpv_my_team_win_score_now,
    acpv_enemy_top_player_div,
    acpv_blitzkrieg_time,
    acpv_enemy_team_score,
    acpv_enemy_team_score_now,
    acpv_artdeliver_count,
    acpv_multi_champion,
    acpv_mad,
    acpv_achilles_heel_ready,
    acpv_faster_than_bullets_time,
    acpv_harvest_count,
    acpv_skewer_count,
    acpv_double_shot_double_kill_time,
    acpv_climber,
    acpv_ammo_elapsed,
    acpv_opener_ready,
    acpv_toughy,
    acpv_invincible_fury,
    acpv_move_state,
    acpv_move_velocity,
    acpv_move_ang_velocity,
    acpv_sprinter_victim_velocity,
    acpv_marksman_count,
    acpv_ambassador,
    acpv_remembrance,
    acpv_avenger,
    acpv_cherub_ready,
    acpv_stalker_flair,
    acpv_black_list,
    acpv_thunder_count,
    acpv_killer_victim_angle,
    acpv_count,
}; //enum enum_accumulative_player_values

namespace detail
{
    template<enum_accumulative_player_values ValueId, typename T>
    struct accumulative_pair_t
    {
        static enum_accumulative_player_values const    value_id = ValueId;
        typedef T                                        value_type;
    };//struct accumulative_pair_t
} //namespace detail

#define ACCUMULATIVE_STATE_LIST Loki::NullType
#define ADD_ACCUMULATIVE_STATE(id, type) typedef Loki::Typelist<detail::accumulative_pair_t<id, type>, ACCUMULATIVE_STATE_LIST> Accumulative_State_Type_##type;
#define SAVE_TYPE_LIST(id, type) Accumulative_State_Type_##type

} //namespace award_system

#endif //#ifndef ACCUMULATIVE_STATES_INCLUDED