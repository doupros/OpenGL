# version 150 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
glUniform1f float time;


in vec3 position;
in vec4 colour;
in vec3 normal;
in vec2 texCoord;

out Vertex{
vec3 colour;
vec2 texCoord;
vec3 normal;
vec3 tangent;
vec3 binormal;
vec3 worldPos;
vec4 shadowProj;
} OUT;



void main(void) {
	position = vec3(position.x, position.y * time, position.z);
	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	OUT.normal = normalize(normalMatrix * normalize(normal));

	OUT.worldPos = (modelMatrix * vec4(position, 1)).xyz;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) *vec4(position, 1.0);
}