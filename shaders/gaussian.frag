#version 460 core

in vec4 frag_color;    
// in vec2 frag_pos;

out vec4 out_color;

void main()
{
    // vec2 d = gl_FragCoord.xy - frag_pos;

    // mat2 cov2Dinv = mat2(frag_cov2D_inv.xy, frag_cov2D_inv.zw);
    // float g = exp(-0.5f * dot(d, cov2Dinv * d));

    out_color.rgb = frag_color.rgb * frag_color.a;
    out_color.a = frag_color.a;

    // if ((frag_color.a) <= (1.0f / 256.0f)) {
    //     discard;
    // }
}