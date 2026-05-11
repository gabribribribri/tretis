uniform sampler2D texture;
uniform float blur_radius;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    // gaussian blur 9x9
    for (float x = -4.0; x <= 4.0; x += 1.0) {
        for (float y = -4.0; y <= 4.0; y += 1.0) {
            float weight = 1.0 - (abs(x) + abs(y)) / 10.0; 
            vec2 offset = vec2(x, y) * blur_radius;
            color += texture2D(texture, texCoord + offset) * weight;
            totalWeight += weight;
        }
    }

    vec4 blurredColor = color / totalWeight;

    // took this luminescence function from somewhere in the internet
    float gray = dot(blurredColor.rgb, vec3(0.2126, 0.7152, 0.0722));

    gl_FragColor = vec4(vec3(gray), blurredColor.a);
}
