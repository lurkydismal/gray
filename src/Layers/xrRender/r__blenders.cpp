#include "stdafx.h"

#include "uber_deffer.h"

#include "Blender_BmmD.h"
#include "blender_deffer_flat.h"
#include "blender_deffer_model.h"
#include "blender_deffer_aref.h"
#include "Blender_Screen_SET.h"
#include "Blender_Editor_Wire.h"
#include "Blender_Editor_Selection.h"
#include "Blender_tree.h"
#include "Blender_detail_still.h"
#include "Blender_Particle.h"
#include "Blender_Model_EbB.h"
#include "Blender_Lm(EbB).h"

IBlender*    CRender::blender_create    (CLASS_ID cls)
{    
    switch (cls)
    {
    case B_DEFAULT:            return new CBlender_deffer_flat        ();        
    case B_DEFAULT_AREF:    return new CBlender_deffer_aref        (true);
    case B_VERT:            return new CBlender_deffer_flat        ();
    case B_VERT_AREF:        return new CBlender_deffer_aref        (false);
    case B_SCREEN_SET:        return new CBlender_Screen_SET        ();    
    case B_SCREEN_GRAY:        return 0;
    case B_EDITOR_WIRE:        return new CBlender_Editor_Wire        ();    
    case B_EDITOR_SEL:        return new CBlender_Editor_Selection();
    case B_LIGHT:            return 0;
    case B_LmBmmD:            return new CBlender_BmmD            ();    
    case B_LaEmB:            return 0;
    case B_LmEbB:            return new CBlender_LmEbB            ();
    case B_B:                return 0;
    case B_BmmD:            return new CBlender_BmmD            ();    
    case B_SHADOW_TEX:        return 0;
    case B_SHADOW_WORLD:    return 0;
    case B_BLUR:            return 0;
    case B_MODEL:            return new CBlender_deffer_model    ();        
    case B_MODEL_EbB:        return new CBlender_Model_EbB        ();    
    case B_DETAIL:            return new CBlender_Detail_Still    ();    
    case B_TREE:            return new CBlender_Tree            ();    
    case B_PARTICLE:        return new CBlender_Particle        ();
    }
    return 0;
}

void        CRender::blender_destroy(IBlender* &B)
{
    xr_delete(B);
}
