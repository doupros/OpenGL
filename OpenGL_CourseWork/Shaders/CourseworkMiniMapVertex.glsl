#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;
in vec2 texCoord;

out Vertex {
	vec2 texCoord;
	vec4 colour;
} OUT;

void main(void) {
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position.xy, 0.0, 1.0);
	gl_Position.x = gl_Position.x + 1.0f;
	gl_Position.y = gl_Position.y + 1.0f;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
}