#version 330

out vec4 outputColor;

void main()
{
	//gl_FragCoord is a built-in available in fragment shader
	//x,y,z are window coordinates; thus ABSOLUTE value of FragCoord
	//is normalized by WindowSize
	float lerpValue = gl_FragCoord.y / 500.0f;
	float lerpValue2 = gl_FragCoord.z / 0.25f;
	float lerpValue3 = gl_FragCoord.x / 500.0f;
		
	//Mix is a linear interpolator
	// Note all three of these are actually independent
	vec4 y_lin = mix( vec4( 1.0f, 1.0f, 1.0f, 1.0f ), vec4( 0.2f, 0.2f, 0.2f, 1.0f ), lerpValue );
	vec4 z_lin = mix( y_lin, vec4( 0.7f, 0.3f, 0.5f, 1.0f ), lerpValue2 );
	outputColor = mix( z_lin, vec4( 0.3f, 0.7f, 1.0f, 1.0f ), lerpValue3 );
}
