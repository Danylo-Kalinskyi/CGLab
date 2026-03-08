varying vec2 v_uv;
uniform sampler2D u_texture;

void main()
{
    // Sample the color from the texture at the current UV coordinate
    vec4 tex_color = texture2D(u_texture, v_uv);
    
    // Output the final color to the screen
    gl_FragColor = tex_color;
}