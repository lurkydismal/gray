////////////////////////////////////////////////////////////////////////////
//    Module         : xrSE_Factory.cpp
//    Created     : 18.06.2004
//  Modified     : 18.06.2004
//    Author        : Dmitriy Iassenev
//    Description : Precompiled header creatore
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include "xrSE_Factory.h"
#include "ai_space.h"
#include "../xrScripts/script_engine.h"
#include "object_factory.h"
#include "xrSE_Factory_import_export.h"
#include "script_properties_list_helper.h"

#include "character_info.h"
#include "specific_character.h"
#include "xrServer_Objects_ALife.h"
//#include "character_community.h"
//#include "monster_community.h"
//#include "character_rank.h"
//#include "character_reputation.h"
extern SFillPropData            fp_data;

extern CSE_Abstract *F_entity_Create    (LPCSTR section);

extern CScriptPropertiesListHelper    *g_property_list_helper;

extern "C" {
    FACTORY_API    ISE_Abstract* __stdcall create_entity    (LPCSTR section)
    {
        return                    (F_entity_Create(section));
    }

    FACTORY_API    void        __stdcall destroy_entity    (ISE_Abstract *&abstract)
    {
        CSE_Abstract            *object = smart_cast<CSE_Abstract*>(abstract);
        F_entity_Destroy        (object);
        abstract                = 0;
    }
    FACTORY_API void __cdecl reload()
    {
        if (fp_data.counter)
        {

            fp_data.unload();
            fp_data.load();
        }
    }
};

BOOL APIENTRY DllMain        (HANDLE module_handle, DWORD call_reason, LPVOID reserved)
{
    switch (call_reason) {
        case DLL_PROCESS_ATTACH: {
//            g_temporary_stuff            = &trivial_encryptor::decode;

            Debug._initialize            (false);
             Core._initialize            ("xrSE_Factory",NULL,TRUE,"fsfactory.ltx");
            string_path                    SYSTEM_LTX;
            FS.update_path                (SYSTEM_LTX,"$game_config$","system.ltx");
            pSettings                    = new CInifile(SYSTEM_LTX);

            CCharacterInfo::InitInternal                    ();
            CSpecificCharacter::InitInternal                ();

            break;
        }
        case DLL_PROCESS_DETACH: {
            CCharacterInfo::DeleteSharedData                ();
            CCharacterInfo::DeleteIdToIndexData                ();
            CSpecificCharacter::DeleteSharedData            ();
            CSpecificCharacter::DeleteIdToIndexData            ();


            xr_delete                    (g_object_factory);
            CInifile** s                = (CInifile**)(&pSettings);
            xr_delete                    (*s);
            xr_delete                    (g_property_list_helper);
            xr_delete                    (g_ai_space);
            xr_delete                    (g_object_factory);
            Core._destroy                ();
            break;
        }
    }
    return                (TRUE);
}

void _destroy_item_data_vector_cont(T_VECTOR* vec)
{
    T_VECTOR::iterator it        = vec->begin();
    T_VECTOR::iterator it_e        = vec->end();

    xr_vector<CUIXml*>            _tmp;    
    for(;it!=it_e;++it)
    {
        xr_vector<CUIXml*>::iterator it_f = std::find(_tmp.begin(), _tmp.end(), (*it)._xml);
        if(it_f==_tmp.end())
            _tmp.push_back    ((*it)._xml);
    }
    delete_data    (_tmp);
}
