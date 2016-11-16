#version 140

uniform vec4 colour;

out vec4 frag_color;

void main () {
	frag_color = colour;
}