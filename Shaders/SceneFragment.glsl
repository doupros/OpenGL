#version 150 core 

 uniform sampler2D diffuseTex; 
 uniform int useTexture; 
 
 in Vertex { 
 vec2 texCoord; 
 vec4 colour;
 } IN;
 
 out vec4 fragColour;
 
 void main(void) { 
 fragColour = IN.colour; 
   vec2 tex = IN.texCoord;
    tex.y = 1 - tex.y;
	fragColour *= texture(diffuseTex , tex); 
 }
 