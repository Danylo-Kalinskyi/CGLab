attribute vec3 a_vertex;
attribute vec2 a_uv;

uniform mat4 u_model;
uniform mat4 u_viewprojection;

varying vec2 v_uv;

void main() {
    v_uv = a_uv;
    // Project the vertex: ViewProjection * Model * LocalPos
    gl_Position = u_viewprojection * u_model * vec4(a_vertex, 1.0);
}