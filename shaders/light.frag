uniform vec2 lightCenter;
uniform vec3 Color;

void main() {
	float distance = length(lightCenter - gl_FragCoord.xy);
	float attenuation = 32.0 /(32.0 + distance);
	vec4 color = vec4(attenuation, attenuation, attenuation, 1) * vec4(Color, 1);

	gl_FragColor = color;
}