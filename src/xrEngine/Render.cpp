#include "stdafx.h"
#include "Render.h"

//ENGINE_API    IRender_interface*    Render        = nullptr;

// resources
IRender_Light::~IRender_Light    ()            {    
    ::Render->light_destroy        (this    );
}
IRender_Glow::~IRender_Glow        ()            {    
    ::Render->glow_destroy        (this    );    
}
