#include "common.hlsli"

struct vv
{
    float4 P : POSITION;
    float2 tc : TEXCOORD0;
    float4 c : COLOR0;
};
struct vf
{
    float4 hpos : POSITION;
    float2 tc : TEXCOORD0;
    float4 c : COLOR0;

//    Igor: for additional depth dest
#ifdef USE_SOFT_PARTICLES
    float4 tctexgen : TEXCOORD1;
#endif //    USE_SOFT_PARTICLES
    float fog : FOG;
};

vf main(vv v)
{
    vf o;

    o.hpos = mul(m_WVP, v.P); // xform, input in world coords
    //    o.hpos         = mul    (m_VP, v.P);        // xform, input in world coords
    o.tc = v.tc; // copy tc
    o.c = v.c; // copy color

//    Igor: for additional depth dest
#ifdef USE_SOFT_PARTICLES
    o.tctexgen = mul(mVPTexgen, v.P);
    o.tctexgen.z = o.hpos.z;
#endif //    USE_SOFT_PARTICLES

    o.fog = saturate(calc_fogging(v.P)); // fog, input in world coords
    return o;
}
