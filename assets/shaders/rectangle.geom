layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float width;
uniform vec2 resolution;

in vec3 v_color[];
out vec3 gColor;  

void main() {    
	float aspect = resolution.x / resolution.y;

    vec4 lineStart = gl_in[0].gl_Position;
    vec4 lineEnd = gl_in[1].gl_Position;

    vec2 a = lineStart.xy;
    vec2 b = lineEnd.xy;
    vec2 dir = normalize(b - a);

    vec2 normal = normalize(vec2(-dir.y/aspect, dir.x));
    vec2 offset = normal * (width /resolution);

    //Corners
    vec2 v0 = a + offset;
    vec2 v1 = a - offset;
    vec2 v2 = b + offset;
    vec2 v3 = b - offset;

    //Create triangle
    gColor = v_color[0];
	gl_Position = vec4(v0, lineStart.zw); EmitVertex();
    gColor = v_color[0];
    gl_Position = vec4(v1, lineStart.zw); EmitVertex();
    gColor = v_color[1];
    gl_Position = vec4(v2, lineEnd.zw); EmitVertex();
    gColor = v_color[1];
    gl_Position = vec4(v3, lineEnd.zw); EmitVertex();
    EndPrimitive();
}  