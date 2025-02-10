#include "stdafx.h"

#include "light_execute.h"
#include "xrDeflector.h"

void light_execute::run( CDeflector& D )
{
    D.Light    (&DB,&LightsSelected,H);
}

