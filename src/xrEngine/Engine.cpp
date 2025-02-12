// Engine.cpp: implementation of the CEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Engine.h"

CEngine                Engine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEngine::CEngine()
{
    g_pEventManager = new CEventManager;
}

CEngine::~CEngine()
{
    delete g_pEventManager;
}

void CEngine::Initialize    (void)
{
    // Other stuff
    Engine.Sheduler.Initialize            ( );
}

void CEngine::Destroy    ()
{
    Engine.Sheduler.Destroy                ( );
    Engine.External.Destroy                ( );
}
