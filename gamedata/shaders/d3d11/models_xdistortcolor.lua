function normal(shader, t_base, t_second, t_detail)
    shader:begin("model_distort", "particle_hard")
        :sorting(3, false)
        :blend(true, blend.srcalpha, blend.invsrcalpha)
        :aref(true, 0)
        :zb(true, false)
        :fog(false)
        :distort(true)

    shader:dx10texture("s_base", t_base)
    shader:dx10sampler("smp_base")
end

function l_special(shader, t_base, t_second, t_detail)
    shader:begin("model_distort", "particle_distort_hard")
        :sorting(3, false)
        :blend(true, blend.srccolor, blend.invsrcalpha)
        :zb(true, false)
        :fog(false)
        :distort(true)

    shader:dx10texture("s_base", t_base)
    shader:dx10texture("s_distort", t_base)
    shader:dx10sampler("smp_linear")
end
