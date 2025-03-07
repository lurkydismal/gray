#pragma once

struct system_file
{
    u8            attrib;
    xr_time_t    time_create;
    xr_time_t    time_write;
    size_t        size;
    string256    name;
};

enum FS_List
{
    FS_ListFiles    =(1<<0),
    FS_ListFolders    =(1<<1),
    FS_ClampExt        =(1<<2),
    FS_RootOnly        =(1<<3),
    FS_forcedword    =u32(-1)
};

class XRCORE_API FS_Path
{
public:
    enum{
        flRecurse    = (1<<0),
        flNotif        = (1<<1),
        flNeedRescan= (1<<2),
    };
public:
    LPSTR        m_Path;
    LPSTR        m_Root;
    LPSTR        m_Add;
    LPSTR        m_DefExt;
    LPSTR        m_FilterCaption;
    Flags32        m_Flags;
public:
                FS_Path        (LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt=0, LPCSTR _FilterString=0, u32 flags=0);
                ~FS_Path    ();
    LPCSTR        _update        (string_path& dest, LPCSTR src) const;
    void        _set        (LPCSTR add);
    void        _set_root    (LPCSTR root);

    void  rescan_path_cb    ();
};

struct XRCORE_API FS_File{
    enum{ 
        flSubDir= (1<<0),
        flVFS    = (1<<1),
    };
    unsigned     attrib;
    time_t          time_write;
    long         size;
    xr_string    name;            // low-case name
    void        set            (xr_string nm, long sz, time_t modif,unsigned attr);
public:
                FS_File        (){}
                FS_File        (xr_string nm);
                FS_File        (const system_file& f);
                FS_File        (xr_string nm, const system_file& f);
                FS_File        (xr_string nm, long sz, time_t modif,unsigned attr);
    bool         operator<    (const FS_File& _X) const    {return xr_strcmp(name.c_str(),_X.name.c_str())<0; }
};

using FS_FileSet = xr_set<FS_File>;
using FS_FileSetIt = FS_FileSet::iterator;

extern bool    XRCORE_API PatternMatch(LPCSTR s, LPCSTR mask);