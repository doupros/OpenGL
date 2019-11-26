# version 150 core

uniform sampler2D snowTex ;

in Vertex {
	vec2 texCoord;
	//vec4 colour;
} IN;

out vec4 gl_FragColor ;

void main ( void ) {
	vec4 color = texture(snowTex, IN.texCoord);
	//color.a = 0.5;
	gl_FragColor = color;
	//gl_FragColor.a = 1;
	gl_FragColor.rgb -= 0.2;
}

