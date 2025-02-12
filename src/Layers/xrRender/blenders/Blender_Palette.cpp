#include "stdafx.h"
#pragma hdrstop

#include "Blender.h"

//////////////////////////////////////////////////////////////////////
#include "Blender_CLSID.h"
IC bool        p_sort            (IBlender* A, IBlender* B)
{
    return _stricmp(A->getComment(),B->getComment())<0;
}

#ifdef __BORLANDC__
    #define TYPES_EQUAL(A,B) (typeid(A) == typeid(B))
#else
    #define TYPES_EQUAL(A,B) (typeid(A).raw_name() == typeid(B).raw_name())
#endif

void        IBlender::CreatePalette(xr_vector<IBlender*> &palette)
{
    // Create palette itself
    R_ASSERT(palette.empty());
    palette.push_back(Create(B_DEFAULT));
    palette.push_back(Create(B_DEFAULT_AREF));
    palette.push_back(Create(B_VERT));
    palette.push_back(Create(B_VERT_AREF));
    palette.push_back(Create(B_SCREEN_SET));
    palette.push_back(Create(B_SCREEN_GRAY));
    palette.push_back(Create(B_EDITOR_WIRE));
    palette.push_back(Create(B_EDITOR_SEL));
    //palette.push_back(Create(B_LIGHT));
    palette.push_back(Create(B_LaEmB));
    palette.push_back(Create(B_LmEbB));
    palette.push_back(Create(B_BmmD));
    //palette.push_back(Create(B_B));
    palette.push_back(Create(B_SHADOW_WORLD));
    palette.push_back(Create(B_BLUR));
    palette.push_back(Create(B_MODEL));
    palette.push_back(Create(B_MODEL_EbB));
    palette.push_back(Create(B_DETAIL));
    palette.push_back(Create(B_TREE));
    palette.push_back(Create(B_PARTICLE));

    // Remove duplicated classes (some of them are really the same in different renderers)
    for (u32 i=0; i<palette.size(); i++)
    {
        IBlender* A        = palette[i];
        for (u32 j=i+1; j<palette.size(); j++)
        {
            IBlender* B        = palette[j];
            if (TYPES_EQUAL(*A,*B))
            {
                xr_delete(palette[j]);
                j--;
            }
        }
    }

    // Sort by desc and return
    std::sort        (palette.begin(),palette.end(),p_sort);
}

#ifndef _EDITOR
// Engine
#include "../../../xrEngine/Render.h"
IBlender*    IBlender::Create    (CLASS_ID cls)
{
    return ::RImplementation.blender_create    (cls);
}
void        IBlender::Destroy    (IBlender*& B)
{
    ::RImplementation.blender_destroy        (B);
}
#else

// Editor
#include "../../xrRenderPC_R1/BlenderDefault.h"
#include "../../xrRenderPC_R1/Blender_default_aref.h"
#include "../../xrRenderPC_R1/Blender_Vertex.h"
#include "../../xrRenderPC_R1/Blender_Vertex_aref.h"
#include "../Blender_Screen_SET.h"
#include "../../xrRenderPC_R1/Blender_Screen_GRAY.h"
#include "../Blender_Editor_Wire.h"
#include "../Blender_Editor_Selection.h"
//#include "blenders/blender_light.h"
#include "../../xrRenderPC_R1/Blender_LaEmB.h"
#include "../Blender_Lm(EbB).h"
#include "../Blender_BmmD.h"
//#include "blenders/blender_B.h"
#include "../../xrRenderPC_R1/Blender_Shadow_World.h"
#include "../../xrRenderPC_R1/Blender_Blur.h"
#include "../../xrRenderPC_R1/Blender_Model.h"
#include "../Blender_Model_EbB.h"
#include "../Blender_detail_still.h"
#include "../Blender_tree.h"
#include "../Blender_Particle.h"

IBlender*    IBlender::Create    (CLASS_ID cls)
{    
    switch (cls)
    {
    case B_DEFAULT:            return new CBlender_default();        
    case B_DEFAULT_AREF:    return new CBlender_default_aref();    
    case B_VERT:            return new CBlender_Vertex();        
    case B_VERT_AREF:        return new CBlender_Vertex_aref();    
    case B_SCREEN_SET:        return new CBlender_Screen_SET();    
    case B_SCREEN_GRAY:        return new CBlender_Screen_GRAY();    
    case B_EDITOR_WIRE:        return new CBlender_Editor_Wire();    
    case B_EDITOR_SEL:        return new CBlender_Editor_Selection();
    //case B_LIGHT:            return new CBlender_LIGHT();        
    case B_LaEmB:            return new CBlender_LaEmB();        
    case B_LmEbB:            return new CBlender_LmEbB();        
    //case B_B:                return new CBlender_B();            
    case B_BmmD:            return new CBlender_BmmD();            
    case B_SHADOW_WORLD:    return new CBlender_ShWorld();        
    case B_BLUR:            return new CBlender_Blur();            
    case B_MODEL:            return new CBlender_Model();        
    case B_MODEL_EbB:        return new CBlender_Model_EbB();    
    case B_DETAIL:            return new CBlender_Detail_Still();    
    case B_TREE:            return new CBlender_Tree();    
    case B_PARTICLE:        return new CBlender_Particle();    
    }
    return 0;
}
void        IBlender::Destroy    (IBlender*& B)
{
    xr_delete    (B);
}
#endif
