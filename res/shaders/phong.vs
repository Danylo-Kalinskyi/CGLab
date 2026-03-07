attribute vec3 a_vertex;
attribute vec2 a_uv;
attribute vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform mat4 u_normal_matrix;

varying vec3 v_position;
varying vec2 v_uv;
varying vec3 v_normal;

void main() {
    v_position = (u_model * vec4(a_vertex, 1.0)).xyz;
    v_uv = a_uv;
    // Proper normal transformation using the normal matrix
    v_normal = (u_normal_matrix * vec4(a_normal, 0.0)).xyz;

    // Project the vertex
    gl_Position = u_viewprojection * vec4(v_position, 1.0);
}