#version 330

out vec4 outputColor;

void main()
{
	//gl_FragCoord is a built-in available in fragment shader
	//x,y,z are window coordinates; thus ABSOLUTE value of FragCoord
	//is normalized by WindowSize
	float lerpValue = gl_FragCoord.y / 500.0f;
		
	//Mix is a linear interpolator
	outputColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f), vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpValue);
}
