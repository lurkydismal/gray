////////////////////////////////////////////////////////////////////////////
//    Module         : script_world_property.h
//    Created     : 19.03.2004
//  Modified     : 19.03.2004
//    Author        : Dmitriy Iassenev
//    Description : Script world property
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_engine_space.h"
#include "../xrScripts/script_export_space.h"

typedef GraphEngineSpace::CWorldProperty CScriptWorldProperty;

class CScriptWorldPropertyWrapper 
{
public:
    DECLARE_SCRIPT_REGISTER_FUNCTION
};

#include "script_world_property_inline.h"