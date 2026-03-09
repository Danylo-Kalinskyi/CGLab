uniform mat4 u_model;
uniform mat4 u_viewprojection;
uniform vec3 u_ambient_light;
uniform vec3 u_Ka;
uniform vec3 u_Kd;
uniform vec3 u_Ks;
uniform float u_shininess;
uniform int u_num_lights;
uniform vec3 u_light_positions[2];
uniform vec3 u_light_colors[2];

varying vec2 v_uv;
varying vec3 v_color;

void main()
{
    // Pass UVs to fragment shader
    v_uv = gl_MultiTexCoord0.xy;

    // Convert local position to world space
    vec3 world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;

    // Convert local normal to world space
    vec3 world_normal = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

    // View direction (assuming camera at origin for simplicity, or pass camera position)
    vec3 V = normalize(-world_position); // Assuming camera at (0,0,0) in world space

    // Ambient
    vec3 ambient = u_Ka * u_ambient_light;

    vec3 total_diffuse = vec3(0.0);
    vec3 total_specular = vec3(0.0);

    for (int i = 0; i < u_num_lights; i++) {
        // Light direction
        vec3 L = normalize(u_light_positions[i] - world_position);

        // Diffuse
        float NdotL = max(dot(world_normal, L), 0.0);
        total_diffuse += u_Kd * u_light_colors[i] * NdotL;

        // Specular
        vec3 R = reflect(-L, world_normal);
        float RdotV = max(dot(R, V), 0.0);
        total_specular += u_Ks * u_light_colors[i] * pow(RdotV, u_shininess);
    }

    // Final color
    v_color = ambient + total_diffuse + total_specular;

    // Project the vertex
    gl_Position = u_viewprojection * vec4(world_position, 1.0);
}