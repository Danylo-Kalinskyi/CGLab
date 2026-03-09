varying vec2 v_uv;
varying vec3 v_color;
uniform sampler2D u_color_texture;
uniform bool u_use_color_texture;

void main()
{
    vec4 final_color = vec4(v_color, 1.0);
    
    if (u_use_color_texture) {
        vec4 tex_color = texture2D(u_color_texture, v_uv);
        final_color *= tex_color;
    }
    
    gl_FragColor = final_color;
}