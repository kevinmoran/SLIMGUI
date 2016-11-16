#version 140

in vec2 tex_coords;
uniform sampler2D texture1;
out vec4 frag_colour;

void main() {
	vec4 texel = texture (texture1, tex_coords);
	if(texel.r<0.5) discard;
	frag_colour = texel;
}