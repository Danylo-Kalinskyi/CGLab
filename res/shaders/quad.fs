varying vec2 v_uv;
uniform vec2 u_res;     
uniform int u_task;     
uniform int u_subtask; 
uniform float u_time;
uniform sampler2D u_tex;

void main()
{
	vec3 color = vec3(0.0);
    float aspect = u_res.x / u_res.y;

    // TASK 1
    if (u_task == 1) {

        // 1.a - gradient
        if (u_subtask == 0) {
            vec3 colorStart = vec3(0.0, 0.0, 1.0); 
            vec3 colorEnd = vec3(1.0, 0.0, 0.0);
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
            // We create a pattern for each type of stripes
            float local_x = mod(v_uv.x * 5.0, 1.0);
            float local_y = mod(v_uv.y * 4.0, 1.0);
            // We define the center and the thickness of the lines
            float thickness = 0.04; 
            float blur = 0.1;

            // We calculate the distance from the current pixel to the center line
            float dist_x = abs(local_x - 0.5);
            float dist_y = abs(local_y - 0.5);

            // We make a smooth step 
            float red_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_x);
            float blue_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_y);
            // We set the color
            color = vec3(red_mask, 0.0, blue_mask);
        }
        // 1.d radial gradient
        else if (u_subtask == 3) {
            // Set up grid
            vec2 p = v_uv * 10;
            p.x *= aspect; // adapt dimensions
            vec2 grid = floor(p); // We use floor to find which square we are in
            
            // 3. Normalize the coordinates back to 0.0-1.0 range 
            // but keep the 'stepped' grid values for the colors
            float color_x = grid.x / (10.0 * aspect);
            float color_y = grid.y / 10;

            // Define corner colors
            vec3 topLeft     = vec3(0.0, 1.0, 0.0);
            vec3 topRight    = vec3(1.0, 1.0, 0.0);
            vec3 bottomLeft  = vec3(0.0, 0.0, 0.0);
            vec3 bottomRight = vec3(1.0, 0.0, 0.0);

            // Mix based on position
            vec3 colorBottom = mix(bottomLeft, bottomRight, clamp(color_x, 0.0, 1.0));
            vec3 colorTop    = mix(topLeft, topRight, clamp(color_x, 0.0, 1.0));
            
            color = mix(colorBottom, colorTop, clamp(color_y, 0.0, 1.0));

        }

        // 1.e grid
        else if (u_subtask == 4) {
            vec2 p = v_uv * 10;
            p.x *= aspect; // adapt dimensions
            vec2 grid = floor(p);
            float check = mod(grid.x + grid.y, 2.0);
            
            // Mix two colors
            color = vec3(check);
        }

        // 1.f - sinusoid (does not work yet)
        else if (u_subtask == 5) {
            // 1. Define the "offset" or "bend" of the wave
            // This creates the wavy shape horizontally
            float wave_offset = sin(v_uv.x * 6.28) * 0.2;

            // 2. Shift the Y coordinate by that wave amount
            // This "pushes" the color up and down based on X
            float shifted_y = v_uv.y + wave_offset;

            // 3. Create a smooth gradient centered at 0.5
            // It gets greener as it approaches the (wavy) center
            float dist_from_center = abs(shifted_y - 0.5);
            
            // 4. Calculate the green intensity
            // 1.0 at the wavy center, fading to 0.0 at the top/bottom
            float green_intensity = 1.0 - smoothstep(0.0, 0.4, dist_from_center);

            // 5. Final output
            color = vec3(0.0, green_intensity, 0.0);
        }
    }

    // TASK 2
    else if (u_task == 2) {
        vec4 texture_color = texture2D( u_tex, v_uv);
        color = texture_color.rgb;

        // 2.a - greyscale
        if (u_subtask == 0) {
            float gray = dot(color, vec3(0.299, 0.587, 0.114));
            color = vec3(gray);
        }

        // 2.b - inversion
        else if (u_subtask == 1) {
            color = 1.0 - color;
        }

        // 2.c - yellow
        else if (u_subtask == 2) {
            float r = dot(color, vec3(0.45, 0.85, 0.20));
            float g = dot(color, vec3(0.40, 0.80, 0.15));
            float b = dot(color, vec3(0.20, 0.45, 0.10));
            color = vec3(r, g, b);
        }

        // 2.d - black and white
        else if (u_subtask == 3) {
            float brightness = dot(color, vec3(0.333));
            color = vec3(step(0.5, brightness));
        }

        // 2.e - vignette
        else if (u_subtask == 4) {
            // Calculate distance from center (0.5, 0.5)
            float dist = distance(v_uv, vec2(0.5));
            // Darken the edges based on distance
            color *= smoothstep(0.6, 0.05, dist);
        }
        
        // 2.f - blur (3x3 box blur)
        else if (u_subtask == 5) {
            // Increase this number to make the blur stronger (e.g., 4.0 or 8.0)
            float blurAmount = 5.0; 
            vec2 texelSize = vec2(blurAmount) / u_res; 
            
            vec3 sum = vec3(0.0);
            
            // Sample 9 points around the current pixel
            sum += texture2D(u_tex, v_uv + vec2(-1.0,  1.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 0.0,  1.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 1.0,  1.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2(-1.0,  0.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 0.0,  0.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 1.0,  0.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2(-1.0, -1.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 0.0, -1.0) * texelSize).rgb;
            sum += texture2D(u_tex, v_uv + vec2( 1.0, -1.0) * texelSize).rgb;
            
            color = sum / 9.0;
        }
    }
    gl_FragColor = vec4(color, 1.0);
}
