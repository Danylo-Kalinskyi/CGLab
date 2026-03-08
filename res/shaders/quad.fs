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

        // 1.b - gradient circle 
        else if (u_subtask == 1) {
            vec2 p = (v_uv - 0.5);
            p.x *= aspect;
            float dist = length(p);
            float gradient = 1.0 - (dist * 2.0);
            color = vec3(step(0.3, dist));
            color = vec3(clamp(gradient, 0.0, 1.0));
        }

        // 1.c - stripes
        else if (u_subtask == 2 ) {
            // Pattern for each type of stripes
            float local_x = mod(v_uv.x * 5.0, 1.0);
            float local_y = mod(v_uv.y * 4.0, 1.0);
            float thickness = 0.04; 
            float blur = 0.1;
            // Distance
            float dist_x = abs(local_x - 0.5);
            float dist_y = abs(local_y - 0.5);
            // Smooth step 
            float red_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_x);
            float blue_mask = 1.0 - smoothstep(thickness, thickness + blur, dist_y);

            color = vec3(red_mask, 0.0, blue_mask);
        }

        // 1.d - radial gradient
        else if (u_subtask == 3) {
            // Set up grid
            vec2 p = v_uv * 10;
            p.x *= aspect; // adapt dimensions
            vec2 grid = floor(p); // We use floor to find which square we are in
            // Normalize coordinates 
            float color_x = grid.x / (10.0 * aspect);
            float color_y = grid.y / 10;
            // Define corner colors
            vec3 topLeft = vec3(0.0, 1.0, 0.0);
            vec3 topRight = vec3(1.0, 1.0, 0.0);
            vec3 bottomLeft = vec3(0.0, 0.0, 0.0);
            vec3 bottomRight = vec3(1.0, 0.0, 0.0);
            // Mix 
            vec3 colorBottom = mix(bottomLeft, bottomRight, clamp(color_x, 0.0, 1.0));
            vec3 colorTop = mix(topLeft, topRight, clamp(color_x, 0.0, 1.0));
            
            color = mix(colorBottom, colorTop, clamp(color_y, 0.0, 1.0));

        }

        // 1.e - grid
        else if (u_subtask == 4) {
            vec2 p = v_uv * 10;
            p.x *= aspect; // adapt dimensions
            vec2 grid = floor(p);
            float check = mod(grid.x + grid.y, 2.0);

            color = vec3(check);
        }

        // 1.f - sinusoid
        else if (u_subtask == 5) {
            // Sine wave
            float wave = sin(v_uv.x * 6) * 0.3 + 0.5;
            // Top gradient
            float grad_top = 1.0 - v_uv.y;
            vec3 colorTop = vec3(0.0, grad_top, 0.0);
            // Bottom gradient 
            float grad_bottom = v_uv.y;
            vec3 colorBottom = vec3(0.0, grad_bottom, 0.0);
            // Split 
            float isAbove = step(wave, v_uv.y);

            color = mix(colorBottom, colorTop, isAbove);
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

        // 2.b - color inversion
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
            // Distance 
            float dist = distance(v_uv, vec2(0.5));
            // Darken edges
            color *= smoothstep(0.6, 0.05, dist);
        }
        
        // 2.f - blur (3x3 box blur)
        else if (u_subtask == 5) {
            vec2 texelSize = vec2(5) / u_res; 
            vec3 sum = vec3(0.0);
            // Sample 9 points around current pixel
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
    // TASK 3
    else if (u_task == 3) {
        vec2 uv = v_uv;

        // 3.a animated rotation
        if (u_subtask == 0) {
            // move origin to center (0.5, 0.5)
            vec2 p = uv - 0.5;
            
            // get rotation angle based on time
            float angle = u_time;
            float s = sin(angle);
            float c = cos(angle);
            
            // apply rotation matrix
            vec2 rotatedP;
            rotatedP.x = p.x * c - p.y * s;
            rotatedP.y = p.x * s + p.y * c;
            
            // move origin back and sample
            uv = rotatedP + 0.5;
            color = texture2D(u_tex, uv).rgb;
        }

        // 3.b pixelization
        else if (u_subtask == 1) {
            // size = min size + ((range from -1 to 1 with frequency 0.25) + 1 to make it from 0 to 2) * amplitude/2 (as our range is from 0 to 1)
            float size = 10.0 + (sin(u_time * 0.25) + 1.0) * 50.0;
            vec2 pixelatedUV = floor(v_uv * size) / size;
            
            color = texture2D(u_tex, pixelatedUV).rgb;
        }
    }
    gl_FragColor = vec4(color, 1.0);
}
