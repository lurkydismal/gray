#ifndef SH_CONSTANT_H
#define SH_CONSTANT_H
#pragma once

#include "../../xrEngine/WaveForm.h"

class        IReader;
class        IWriter;

class    ECORE_API    CConstant    : public xr_resource_named                                    {
public:
    enum            { modeProgrammable=0, modeWaveForm    };
public:
    Fcolor            const_float;
    u32                const_dword;

    u32                dwFrame;
    u32                dwMode;
    WaveForm        _R;
    WaveForm        _G;
    WaveForm        _B;
    WaveForm        _A;

    // Non-Copyable (mt-safe)
#ifndef _EDITOR
    CConstant& operator=(const CConstant& Other) = delete;
    CConstant& operator=(CConstant&& Other) = delete;
    CConstant(const CConstant& Other) = delete;
#endif

    IC void Copy(const CConstant* src)
    {
        const_float = src->const_float;
        const_dword = src->const_dword;
        dwFrame = src->dwFrame;
        dwMode = src->dwMode;
        _R = src->_R;
        _G = src->_G;
        _B = src->_B;
        _A = src->_A;
    }

    CConstant()
    {
        Memory.mem_fill(this, 0, sizeof(CConstant));
    }

    IC void            set_float    (float r, float g, float b, float a)
    {
        const_float.set    (r,g,b,a);
        const_dword        = const_float.get();
    }
    IC void            set_float    (Fcolor& c)
    {
        const_float.set    (c);
        const_dword        = const_float.get();
    }
    IC void            set_dword    (u32 c)
    {
        const_float.set(c);
        const_dword        = c;
    }
    void            Calculate    ();
    IC    BOOL        Similar        (CConstant& C)        // comare by modes and params
    {
        if (dwMode!=C.dwMode)    return FALSE;
        if (!_R.Similar(C._R))    return FALSE;
        if (!_G.Similar(C._G))    return FALSE;
        if (!_B.Similar(C._B))    return FALSE;
        if (!_A.Similar(C._A))    return FALSE;
        return TRUE;
    }
    void            Load        (IReader* fs);
    void            Save        (IWriter* fs);
};

typedef    resptr_core<CConstant,resptr_base<CConstant> >    
    ref_constant_obsolette;

#endif
