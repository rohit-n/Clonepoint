#version 130

in vec2 texpos;
uniform sampler2D tex;
uniform vec3 color;

void main(void) 
{
	if (texture2D(tex, texpos).rgb == vec3(1, 0, 1))
		discard;
	gl_FragColor = vec4(color, 1);
}