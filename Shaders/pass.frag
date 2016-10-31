#version 410

uniform vec4 quad_colour;

out vec4 frag_color;

void main () {
	frag_color = quad_colour;
}