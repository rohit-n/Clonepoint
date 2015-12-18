#version 130

in vec2 texpos;
uniform sampler2D tex;
uniform int use_blur;

vec4 blur(sampler2D tex, vec2 coords)
{
	vec4 sum = vec4(0.0);
	for (int x = -4; x <= 4; x++)
	{
		for (int y = -4; y <= 4; y++)
		{
			sum += texture(
			tex,
			vec2(coords.x + x * 0.002, coords.y + y * 0.002)
			) / 81.0;
		}
	}
	return sum;
}

void main(void) 
{
	if (texture2D(tex, texpos).rgb == vec3(1, 0, 1))
		discard;

	gl_FragColor = texture2D(tex, texpos);
	if (use_blur == 1)
	{
		gl_FragColor = blur(tex, texpos);
		gl_FragColor.r = 0;
		gl_FragColor.g = 0;
	}

	//gl_FragColor = vec4(1, 0, 0, 1);
}