////////////////////////////////////////////////////////////////////////////
//    Module         : level_spawn_constructor.h
//    Created     : 16.10.2004
//  Modified     : 16.10.2004
//    Author        : Dmitriy Iassenev
//    Description : Level spawn constructor
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../xrForms/xrThread.h"
#include "spawn_constructor_space.h"

class CLevelGraph;
class CGameLevelCrossTable;
class CGameSpawnConstructor;
class CSE_ALifeCreatureActor;
class CSE_Abstract;
class CSE_ALifeObject;
class CSE_ALifeGraphPoint;
//class CSE_SpawnGroup;
class CSE_ALifeAnomalousZone;
class CSpaceRestrictorWrapper;
class CPatrolPathStorage;
class CSE_ALifeDynamicObject;

class CLevelSpawnConstructor : public CThread {
public:
    typedef SpawnConstructorSpace::LEVEL_POINT_STORAGE            LEVEL_POINT_STORAGE;
    typedef SpawnConstructorSpace::LEVEL_CHANGER_STORAGE        LEVEL_CHANGER_STORAGE;
    typedef xr_vector<CSE_ALifeObject*>                            SPAWN_STORAGE;
    typedef xr_vector<CSE_ALifeGraphPoint*>                        GRAPH_POINT_STORAGE;
    typedef xr_vector<CSpaceRestrictorWrapper*>                    SPACE_RESTRICTORS;

private:
    CGameGraph::SLevel                    m_level;
    SPAWN_STORAGE                        m_spawns;
    LEVEL_POINT_STORAGE                    m_level_points;
    GRAPH_POINT_STORAGE                    m_graph_points;
    SPACE_RESTRICTORS                    m_space_restrictors;
    CGameSpawnConstructor                *m_game_spawn_constructor;
    CSE_ALifeCreatureActor                *m_actor;
    CLevelGraph                            *m_level_graph;
    LEVEL_CHANGER_STORAGE                m_level_changers;
    bool                                m_no_separator_check;

private:
    const IGameLevelCrossTable            *m_cross_table;

protected:
            void                        init                                ();
            void                        load_objects                        ();
            void                        correct_objects                        ();
            void                        generate_artefact_spawn_positions    ();
            void                        correct_level_changers                ();
            void                        verify_space_restrictors            ();
            void                        fill_level_changers                    ();
            CSE_Abstract                *create_object                        (IReader                *chunk);
            void                        add_graph_point                        (CSE_Abstract            *abstract);
            void                        add_story_object                    (CSE_ALifeDynamicObject *dynamic_object);
            void                        add_space_restrictor                (CSE_ALifeDynamicObject *dynamic_object);
            void                        add_free_object                        (CSE_Abstract            *abstract);
            void                        add_level_changer                    (CSE_Abstract            *abstract);
            void                        update_artefact_spawn_positions        ();
    IC        const CGameGraph            &game_graph                            () const;
    IC        const CLevelGraph            &level_graph                        () const;
    IC        const IGameLevelCrossTable    &cross_table                        () const;
    IC        LEVEL_CHANGER_STORAGE        &level_changers                        () const;
    IC        u32                            level_id                            (shared_str level_name) const;

public:
    IC                                    CLevelSpawnConstructor                (const CGameGraph::SLevel &level, CGameSpawnConstructor *game_spawn_constructor, bool no_separator_check);
    virtual                                ~CLevelSpawnConstructor                ();
    virtual void                        Execute                                ();
    IC        CSE_ALifeCreatureActor        *actor                                () const;
    IC        const CGameGraph::SLevel    &level                                () const;
            void                        update                                ();
    IC        CGameSpawnConstructor        &game_spawn_constructor                () const;
};

#include "level_spawn_constructor_inline.h"