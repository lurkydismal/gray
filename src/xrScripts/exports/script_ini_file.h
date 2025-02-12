////////////////////////////////////////////////////////////////////////////
//    Module         : script_ini_file.h
//    Created     : 21.05.2004
//  Modified     : 21.05.2004
//    Author        : Dmitriy Iassenev
//    Description : Script ini file class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_token_list.h"
#include "../xrScripts/script_export_space.h"

class SCRIPTS_API CScriptIniFile :
    public CInifile 
{
protected:
    typedef CInifile inherited;

public:
                        CScriptIniFile        (IReader *F, LPCSTR path=0);
                        CScriptIniFile        (LPCSTR szFileName, BOOL ReadOnly=TRUE, BOOL bLoadAtStart=TRUE, BOOL SaveAtEnd=TRUE, LPCSTR path=nullptr);

    virtual             ~CScriptIniFile        ();
            bool        line_exist            (LPCSTR S, LPCSTR L);
            bool        section_exist        (LPCSTR S);
            int            r_clsid                (LPCSTR S, LPCSTR L);
            bool        r_bool                (LPCSTR S, LPCSTR L);
            int            r_token                (LPCSTR S, LPCSTR L, const CScriptTokenList &token_list);
            LPCSTR        r_string_wb            (LPCSTR S, LPCSTR L);
            LPCSTR        update                (LPCSTR file_name);
            u32            line_count            (LPCSTR S);
            LPCSTR        r_string            (LPCSTR S, LPCSTR L);
            u32            r_u32                (LPCSTR S, LPCSTR L);
            int            r_s32                (LPCSTR S, LPCSTR L);
            float        r_float                (LPCSTR S, LPCSTR L);
            Fvector        r_fvector3            (LPCSTR S, LPCSTR L);
            void        set_override_names    (bool b);
            bool        save_as(LPCSTR new_fname);
            void        set_readonly(bool b);
            DECLARE_SCRIPT_REGISTER_FUNCTION
};

#include "script_ini_file_inline.h"