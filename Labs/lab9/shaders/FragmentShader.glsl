#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;

uniform float time;
uniform int use_time;

// TODO(student): Declare various other uniforms
uniform bool mix_texture;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.
    vec4 col1 = texture2D(texture_1, texcoord);
	vec4 col2 = texture2D(texture_2, texcoord);

	if(mix_texture == true) {
		if(use_time == 1) {
			out_color = mix(col1, col2, abs(sin(time)));
		} else {
			out_color = mix(col1, col2, 0.6f);
		}
	} else {
		out_color = col1;
	}

	if(out_color.a < 0.5f) {
		discard;
	}
}
