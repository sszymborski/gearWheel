#version 330 core

#define ANGLE_NUMBER 16

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in float texid;

out vec3 vecColor;
out vec2 TexCoord;
out float TexID;
  
uniform mat4 transform;
uniform mat4 transform2;
uniform mat4 move1;			// przerzucanie do srodka - dla drugiego powrot
uniform mat4 move2;			// powrot z srodka - dla drugiego przerzucanie
uniform mat4 move3;			// ruch walca
uniform mat4 move4;			// umiejscowienie podstawy
uniform mat4 change;		// wlasciwa rotacja osi
uniform mat4 view;
uniform mat4 projection;


void main()
{
	if(gl_VertexID < ANGLE_NUMBER*2*2+2)
		gl_Position = projection * view * change * move2 * transform * move1 * vec4(position, 1.0f);	//pierwsze kolo zebate
	else if(gl_VertexID >= ANGLE_NUMBER*2*2+2 && gl_VertexID < (ANGLE_NUMBER*2*2+2)*2 )
		gl_Position = projection * view * change * move1 * transform2 * move2 * vec4(position, 1.0f);	//drugie kolo zebate
	else if(gl_VertexID >= (ANGLE_NUMBER*2*2+2)*2 && gl_VertexID < (ANGLE_NUMBER*2*2+2)*2 + 34)
		gl_Position = projection * view * change * move3 * vec4(position, 1.0f);	//walec
	else if(gl_VertexID >= (ANGLE_NUMBER*2*2+2)*2 + 34 && gl_VertexID < (ANGLE_NUMBER*2*2+2)*2 + 34 + 20)
		gl_Position = projection * view * change * move4 * vec4(position, 1.0f);	//podstawa
	else
		gl_Position = projection * view * change * vec4(position, 1.0f);

	vecColor = color;
    TexCoord = texCoord;
	TexID = texid;
} 