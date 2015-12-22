#version 130

in vec2 texpos;
uniform sampler2D tex;
uniform int use_blur;
uniform int use_grayscale;

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

vec4 grayscale(vec4 color)
{
	float gs = (0.2126 * color.r) + (0.7152 * color.g) + (0.0722 * color.b);
	return vec4(gs, gs, gs, 1.0);
}

void main(void) 
{
	vec4 texture_color = texture2D(tex, texpos);

	if (texture_color.rgb == vec3(1, 0, 1))
		discard;

	gl_FragColor = texture_color;

	if (use_blur == 1)
	{
		gl_FragColor = blur(tex, texpos);
		gl_FragColor.r = 0;
		gl_FragColor.g = 0;
	}

	if (use_grayscale == 1)
	{
		gl_FragColor = grayscale(texture_color);
	}
}