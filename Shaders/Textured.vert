#version 140

in vec2 vp;
in vec2 vt;

uniform mat4 M, P;

out vec2 tex_coords;

void main() {
	tex_coords = vt;
	gl_Position = P*M*vec4(vp, -1.0, 1.0);
}