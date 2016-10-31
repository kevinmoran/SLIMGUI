#version 410

in vec2 vp;
uniform mat4 P;

void main () {
	gl_Position = P*vec4(vp, -1.0, 1.0);
}