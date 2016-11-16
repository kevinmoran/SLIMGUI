#version 140

in vec2 vp;
uniform mat4 M,P;

void main () {
	gl_Position = P*M*vec4(vp, -1.0, 1.0);
}