#version 130

in vec2 texpos;
uniform sampler2D tex;

void main(void) 
{
	if (texture2D(tex, texpos).rgb == vec3(1, 0, 1))
		discard;
	gl_FragColor = texture2D(tex, texpos);
	gl_FragColor.r = 0;
	gl_FragColor.g = 0;
	gl_FragColor.b = gl_FragColor.b * 1.25;
}