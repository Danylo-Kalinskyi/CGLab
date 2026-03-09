uniform mat4 u_model;
uniform mat4 u_viewprojection;

varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec2 v_uv;

void main()
{
    // Pass UVs
    v_uv = gl_MultiTexCoord0.xy;

    // World position
    v_world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;

    // World normal
    v_world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

    // Project
    gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
