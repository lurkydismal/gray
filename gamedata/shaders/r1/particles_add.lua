function normal(shader, t_base, t_second, t_detail)
    shader:begin("particle_add", "particle")
        :sorting(3, false)
        :blend(true, blend.one, blend.one)
        :aref(false, 0)
        :zb(true, false)
        :fog(false)
    shader:sampler("s_base"):texture(t_base)
end
