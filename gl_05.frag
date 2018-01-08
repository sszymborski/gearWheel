#version 330 core
in vec3 vecColor;
in vec2 TexCoord;
in float TexID;

out vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;

void main()
{
   if(TexID < 0.0f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.01); // metal
	else
	color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.99);


//if(TexID < 0.0f)
//	color = texture2D(Texture0, TexCoord);
//else
//	color = vec4(vecColor, 1.0f);

}
