uniform mat4 u_model;
uniform vec3 u_ambient_light;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;
uniform int u_num_lights;
uniform vec3 u_light_positions[2];
uniform vec3 u_light_colors[2];
uniform sampler2D u_color_texture;
uniform sampler2D u_normal_texture;
uniform bool u_use_color_texture;
uniform bool u_use_specular_texture;
uniform bool u_use_normal_texture;

varying vec3 v_world_position;
varying vec3 v_world_normal;
varying vec2 v_uv;

void main()
{
    // Normal
    vec3 N = normalize(v_world_normal);
    if (u_use_normal_texture) {
        vec3 normal_tex = texture2D(u_normal_texture, v_uv).xyz * 2.0 - 1.0; // -1 to 1
        // Transform to world space
        normal_tex = normalize((u_model * vec4(normal_tex, 0.0)).xyz);
        // Mix with interpolated normal
        N = mix(N, normal_tex, 0.1); // or some factor
    }

    vec3 V = normalize(-v_world_position); // Camera at origin

    // Material properties
    vec3 Ka = u_Ka;
    vec3 Kd = u_Kd;
    vec3 Ks = u_Ks;

    if (u_use_color_texture) {
        vec4 tex_color = texture2D(u_color_texture, v_uv);
        Ka = tex_color.rgb;
        Kd = tex_color.rgb;
        if (u_use_specular_texture) {
            Ks = vec3(tex_color.a); // grayscale
        }
    }

    // Ambient
    vec3 ambient = Ka * u_ambient_light;

    vec3 total_diffuse = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    for (int i = 0; i < u_num_lights; i++) {
        vec3 L = normalize(u_light_positions[i] - v_world_position);

        // Diffuse
        float NdotL = max(dot(N, L), 0.0);
        total_diffuse += Kd * u_light_colors[i] * NdotL;

        // Specular
        vec3 R = reflect(-L, N);
        float RdotV = max(dot(R, V), 0.0);
        total_specular += Ks * u_light_colors[i] * pow(RdotV, u_shininess);
    }

    vec3 final_color = ambient + total_diffuse + total_specular;

    gl_FragColor = vec4(final_color, 1.0);
}