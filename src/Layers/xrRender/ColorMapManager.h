#ifndef    ColorMapManager_included
#define    ColorMapManager_included
#pragma once

//    Reduces amount of work if the texture was not changed.
//    Stores used textures in a separate map to avoid removal of
//    of color map textures from memory.

class ColorMapManager
{
public:
    ColorMapManager();

    void    SetTextures(const shared_str &tex0, const shared_str &tex1);

private:

    void    UpdateTexture(const shared_str &strTexName, int iTex);

private:

    struct str_pred {
        IC bool operator()(const shared_str &x, const shared_str &y) const
        {    return x<y;    }
    };

    using map_Tex = xr_map<shared_str, ref_texture, str_pred>;
    using map_TexIt = map_Tex::iterator;

private:

    ref_texture        m_CMap[2];
    shared_str        m_strCMap[2];

    map_Tex            m_TexCache;
};

#endif    //    ColorMapManager_included