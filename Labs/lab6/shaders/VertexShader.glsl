#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 position;
layout(location = 3) in vec3 normal;
layout(location = 2) in vec2 texture;
layout(location = 1) in vec3 color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;
out vec3 frag_pos;
out vec3 frag_norm;
out vec2 frag_texture;
 
void main()
{
    // TODO(student): Send output to fragment shader
    frag_pos = position;
    frag_color = color;
    frag_texture = texture;
    frag_norm = normal;

    // TODO(student): Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
