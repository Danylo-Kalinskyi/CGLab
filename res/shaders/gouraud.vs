attribute vec3 a_vertex;
attribute vec2 a_uv;
attribute vec3 a_normal;

uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform mat4 u_normal_matrix;
uniform vec3 u_eye;
uniform vec3 u_ambient_light;
uniform int u_num_lights;
uniform struct Light {
    vec3 position;
    vec3 color;
} u_lights[10];
uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_ns;
uniform int u_use_color_texture;
uniform int u_use_specular_texture;
uniform int u_use_normal_texture;
uniform sampler2D u_color_texture;
uniform sampler2D u_specular_texture;
uniform sampler2D u_normal_texture;

varying vec4 v_color;

vec3 computeLighting(vec3 position, vec3 normal, vec2 uv) {
    vec3 ka = u_ka;
    vec3 kd = u_kd;
    vec3 ks = u_ks;

    // Sample textures if available
    if (u_use_color_texture != 0) {
        vec4 color_sample = texture2D(u_color_texture, uv);
        ka = color_sample.rgb;
        kd = color_sample.rgb;
        ks = vec3(color_sample.a); // Alpha channel for specular
    }

    if (u_use_specular_texture != 0) {
        vec4 specular_sample = texture2D(u_specular_texture, uv);
        ks = specular_sample.rgb;
    }

    vec3 normal_world = (u_normal_matrix * vec4(normal, 0.0)).xyz;
    if (u_use_normal_texture != 0) {
        vec3 normal_sample = texture2D(u_normal_texture, uv).rgb;
        normal_sample = normal_sample * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
        normal_world = normalize(u_normal_matrix * vec4(normal_sample, 0.0)).xyz;
    }

    vec3 position_world = (u_model * vec4(position, 1.0)).xyz;
    vec3 view_dir = normalize(u_eye - position_world);

    vec3 total_color = u_ambient_light * ka;

    for (int i = 0; i < u_num_lights; ++i) {
        vec3 light_dir = normalize(u_lights[i].position - position_world);
        vec3 reflect_dir = reflect(-light_dir, normal_world);

        // Diffuse
        float diff = max(dot(normal_world, light_dir), 0.0);
        vec3 diffuse = u_lights[i].color * kd * diff;

        // Specular
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_ns);
        vec3 specular = u_lights[i].color * ks * spec;

        total_color += diffuse + specular;
    }

    return total_color;
}

void main() {
    vec3 position_world = (u_model * vec4(a_vertex, 1.0)).xyz;
    vec3 normal_world = normalize((u_model * vec4(a_normal, 0.0)).xyz);

    // Compute lighting at vertex
    v_color = vec4(computeLighting(a_vertex, a_normal, a_uv), 1.0);

    // Project the vertex
    gl_Position = u_viewprojection * u_model * vec4(a_vertex, 1.0);
}