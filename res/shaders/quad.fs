varying vec2 v_uv;
uniform vec2 u_res;     
uniform int u_task;     
uniform int u_subtask; 
uniform float u_time;

void main()
{
	vec3 color = vec3(0.0);
    float aspect = u_res.x / u_res.y;

    // TASK 1
    if (u_task == 1) 
    {
        // 1.a - gradient
        if (u_subtask == 0) {
            vec3 colorStart = vec3(0.0, 0.0, 1.0); 
            vec3 colorEnd   = vec3(1.0, 0.0, 0.0);
            color = mix(colorStart, colorEnd, v_uv.x);
        }
        // 1.b gradient circle 
        else if (u_subtask == 1) {
            vec2 p = (v_uv - 0.5);
            p.x *= aspect;
            float dist = length(p);
            float gradient = 1.0 - (dist * 2.0);
            color = vec3(step(0.3, dist));
            color = vec3(clamp(gradient, 0.0, 1.0));
        }
        // 1.c stripes
        else if (u_subtask == 2 ) {
            float local_x = mod(v_uv.x * 5.0, 1.0);
            float local_y = mod(v_uv.y * 4.0, 1.0);
            // 1. Define the center and the thickness of the line
            float thickness = 0.04; 
            float blur = 0.1;

            // 2. Calculate the distance from the current pixel to the center line
            float dist_x = abs(local_x - 0.5);
            float dist_y = abs(local_y - 0.5);

            // 3. Use step to create the sharp line
            // If the distance is less than half the thickness, result is 1.0 (Red)
            // Otherwise, result is 0.0 (Black)
            float red_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_x);
            float blue_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_y);


            // 4. Set the color to Red based on the mask
            color = vec3(red_mask, 0.0, blue_mask);
        }
        // 1.e grid
        else if (u_subtask == 4) {
            // 10 squares
            vec2 p = v_uv * 10;
            p.x *= aspect; // adapt dimensions
            vec2 grid = floor(p);
            float check = mod(grid.x + grid.y, 2.0);
            
            // Mix two colors
            color = vec3(check);
        }
    }
    gl_FragColor = vec4(color, 1.0);
}
