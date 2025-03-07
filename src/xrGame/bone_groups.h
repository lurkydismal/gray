#pragma once

#include "../xrCore/associative_vector.h"

class CActor;

namespace award_system
{

    class bone_group
    {
    public:
        enum enum_group_id
        {
            gid_head = 0x00,
            gid_eyes,
            gid_spine,
        };//enum enum_group_id
        static u16 const gid_any = u16(-1);

        bone_group();
        ~bone_group();

        void init(CActor* first_spawned_actor);
        bool is_bone_in_group(u16 bone_id, enum_group_id gid) const;
    private:
        using bone_groups_map_t = associative_vector<u16, enum_group_id>;

        bone_groups_map_t m_bone_groups;
    };
}