#version 330
precision mediump float;                            
out vec4 FragColor;

in vec3 v_color;

void main() {
    FragColor = vec4(v_color, 1.0);  // Set the color of the line
}