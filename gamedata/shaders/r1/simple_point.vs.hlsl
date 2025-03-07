#include "common.hlsli"

vf_point main(v_lmap v)
{
    vf_point o;

    o.hpos = mul(m_VP, v.P); // xform, input in world coords
    o.tc0 = unpack_tc_base(v.uv0, v.T.w, v.B.w); // copy tc
    //    o.tc0        = unpack_tc_base(v.tc0);                    // copy tc
    o.color = calc_point(o.tc1, o.tc2, v.P, unpack_normal(v.N)); // just hemisphere

    return o;
}
