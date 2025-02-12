////////////////////////////////////////////////////////////////////////////
//    Module         : ai_space.h
//    Created     : 12.11.2003
//  Modified     : 18.06.2004
//    Author        : Dmitriy Iassenev
//    Description : AI space class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_space.h"
#include "../xrScripts/script_engine.h"

CAI_Space *g_ai_space = 0;

CAI_Space::CAI_Space                ()
{
}

void CAI_Space::init                ()
{
    VERIFY(!g_pScriptEngine);
    g_pScriptEngine = new CScriptEngine();
    m_script_engine = g_pScriptEngine;
    g_pScriptEngine->init    ();
}

CAI_Space::~CAI_Space                ()
{
    xr_delete                (g_pScriptEngine);
}

#include "../../xrEngine/AI/alife_space.h"
namespace ALife {
    xr_token hit_types_token[] =
    {
        { "burn",                        eHitTypeBurn                                },
        { "shock",                        eHitTypeShock                                },
        { "strike",                        eHitTypeStrike                                },
        { "wound",                        eHitTypeWound                                },
        { "radiation",                    eHitTypeRadiation                            },
        { "telepatic",                    eHitTypeTelepatic                            },
        { "fire_wound",                    eHitTypeFireWound                            },
        { "chemical_burn",                eHitTypeChemicalBurn                        },
        { "explosion",                    eHitTypeExplosion                            },
        { "wound_2",                    eHitTypeWound_2                                },
    //    { "physic_strike",                eHitTypePhysicStrike                        },
        { "light_burn",                    eHitTypeLightBurn                            },
        { 0,                            0                                            }
    };
}