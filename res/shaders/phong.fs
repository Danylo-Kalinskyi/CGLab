varying vec3 v_position;
varying vec2 v_uv;
varying vec3 v_normal;

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

void main() {
    vec3 ka = u_ka;
    vec3 kd = u_kd;
    vec3 ks = u_ks;

    // Sample textures if available
    if (u_use_color_texture != 0) {
        vec4 color_sample = texture2D(u_color_texture, v_uv);
        ka = color_sample.rgb;
        kd = color_sample.rgb;
        ks = vec3(color_sample.a); // Alpha channel for specular
    }

    if (u_use_specular_texture != 0) {
        vec4 specular_sample = texture2D(u_specular_texture, v_uv);
        ks = specular_sample.rgb;
    }

    vec3 normal_world = normalize(v_normal);
    if (u_use_normal_texture != 0) {
        vec3 normal_sample = texture2D(u_normal_texture, v_uv).rgb;
        normal_sample = normal_sample * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
        // Mix interpolated normal with texture normal for smoothing
        normal_world = normalize(mix(v_normal, normal_sample, 0.5));
    }

    vec3 view_dir = normalize(u_eye - v_position);

    vec3 total_color = u_ambient_light * ka;

    for (int i = 0; i < u_num_lights; ++i) {
        vec3 light_dir = normalize(u_lights[i].position - v_position);
        vec3 reflect_dir = reflect(-light_dir, normal_world);

        // Diffuse
        float diff = max(dot(normal_world, light_dir), 0.0);
        vec3 diffuse = u_lights[i].color * kd * diff;

        // Specular
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), u_ns);
        vec3 specular = u_lights[i].color * ks * spec;

        total_color += diffuse + specular;
    }

    gl_FragColor = vec4(total_color, 1.0);
}