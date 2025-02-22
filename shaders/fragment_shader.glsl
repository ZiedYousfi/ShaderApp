#version 410 core
out vec4 FragColor;

void main() {
    vec2 pos = gl_FragCoord.xy;

    float time = sin(pos.x / 100.0) * cos(pos.y / 50.0) * 3.14159;

    vec3 crazyColor = vec3(
            sin(time) * cos(pos.x / 42.0),
            cos(time * 2.0) * sin(pos.y / 23.0),
            tan(time / 2.0) * 0.5 + 0.5
        );

    float blink = step(sin(time * 10.0), 0.0);

    FragColor = vec4(
            mod(crazyColor * blink + vec3(0.5), 1.0),
            1.0
        );
}
