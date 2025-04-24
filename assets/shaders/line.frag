precision mediump float;                            
out vec4 FragColor;

#ifdef USING_GEOMETRY_SHADER
in vec3 gColor;
#define v_color gColor
#else
in vec3 v_color;
#endif

void main() {
    FragColor = vec4(v_color, 1.0);  // Set the color of the line
}