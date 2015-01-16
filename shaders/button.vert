#version 130

in vec2 Position;
in vec2 TexCoord;
out vec2 texpos;
uniform mat4 proj_mat;

void main(void)
{
	gl_Position = proj_mat * vec4(Position, 0, 1);
	texpos = TexCoord;
}