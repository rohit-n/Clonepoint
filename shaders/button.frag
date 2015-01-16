#version 130

in vec2 texpos;
uniform sampler2D tex;

void main(void) 
{
	if (texture2D(tex, texpos).rgb == vec3(1, 0, 1))
		discard;
	gl_FragColor = texture2D(tex, texpos);
	//gl_FragColor = vec4(1, 0, 0, 1);
}