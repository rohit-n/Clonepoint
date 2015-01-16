#version 130

in vec2 texpos;
uniform sampler2D tex;
uniform vec3 color;
uniform float alpha_mod;

void main(void) 
{
	gl_FragColor = vec4(color, texture2D(tex, texpos).a * alpha_mod);
	//gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}