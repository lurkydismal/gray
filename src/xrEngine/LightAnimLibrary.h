#pragma once

class ENGINE_API CLAItem                {
public:
    shared_str        cName;
    float           fFPS;

    using KeyMap = xr_map<int, u32>;
    using KeyPairIt = KeyMap::iterator;

    KeyMap            Keys;
    int                iFrameCount;
public:
                    CLAItem                ();
                    
    void            InitDefault            ();
    void            Load                (IReader& F);
    void            Save                (IWriter& F);
    float            Length_sec            (){return float(iFrameCount)/fFPS;}
    u32                Length_ms            (){return iFloor(Length_sec()*1000.f);}
    u32                InterpolateRGB        (int frame);
    u32                InterpolateBGR        (int frame);
    u32                CalculateRGB        (float T, int& frame);
    u32                CalculateBGR        (float T, int& frame);
    void            Resize                (int new_len);
    void            InsertKey            (int frame, u32 color);
    void            DeleteKey            (int frame);
    void            MoveKey                (int from, int to);
    bool            IsKey                (int frame){return (Keys.end()!=Keys.find(frame));}
    int                 PrevKeyFrame        (int frame);
    int                 NextKeyFrame         (int frame);
    int                 FirstKeyFrame        (){return Keys.rend()->first;}
    int                 LastKeyFrame        (){return Keys.rbegin()->first;}
    u32*            GetKey                (int frame){KeyPairIt it=Keys.find(frame); return (it!=Keys.end())?&(it->second):0;}
};

using LAItemVec = xr_vector<CLAItem*>;
using LAItemIt = LAItemVec::iterator;

#include "../../Editors/Public/xrEPropsTypes.h"

class ENGINE_API ELightAnimLibrary{
public:
    LAItemVec       Items;
    LAItemIt        FindItemI            (LPCSTR name);
    CLAItem*        FindItem            (LPCSTR name);
public:
                    ELightAnimLibrary    ();
                    ~ELightAnimLibrary    ();

    void              RemoveObject        (LPCSTR fname, EItemType type, bool& res);
    void             RenameObject        (LPCSTR fn0, LPCSTR fn1, EItemType type);

    void            OnCreate            ();
    void            OnDestroy            ();
    void            Load                ();
    void            Save                ();
    void            Reload                ();
    void            Unload                ();
    CLAItem*        AppendItem            (LPCSTR name, CLAItem* src);
    LAItemVec&        Objects                (){return Items;}
};

extern ENGINE_API ELightAnimLibrary LALib;