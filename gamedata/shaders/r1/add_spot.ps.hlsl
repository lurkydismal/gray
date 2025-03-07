#include "common.hlsli"

// Pixel
float4 main(vf_spot I) : COLOR
{
    float4 t_base = tex2D(s_base, I.tc0);
    float4 t_lmap = tex2Dproj(s_lmap, I.tc1); // spot-projector
    float4 t_att = tex2D(s_att, I.tc2); // spot-att

    float4 final_color = t_base * t_lmap * t_att * I.color;
    final_color.rgb *= t_base.a;
    float3 final_rgb = (final_color + final_color) * 2.0f;
    float final_a = final_color.w;

    // out
    return final_color * 2; // float4    (final_rgb,final_a);
}
