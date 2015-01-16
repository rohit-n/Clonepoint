#version 130

in vec4 coord;
out vec2 texpos;
uniform mat4 proj_mat;

void main(void)
{
	gl_Position = proj_mat * vec4(coord.xy, 0, 1);
	texpos = coord.zw;
}