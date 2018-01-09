#version 330 core
in vec3 vecColor;
in vec2 TexCoord;
in float TexID;

out vec4 color;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;
uniform sampler2D Texture4;

void main()
{
   if(TexID < -0.6f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.01); // metal
	else if(TexID > 0.6f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.99);
	else if(TexID > 0.4f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture3, TexCoord), texture(Texture1, TexCoord), 0.01); // podloga
	else if(TexID < 0.0f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture4, TexCoord), texture(Texture1, TexCoord), 0.01); // podloga
	else
	color = vec4(vecColor, 1.0f) * mix(texture(Texture2, TexCoord), texture(Texture0, TexCoord), 0.01);



//if(TexID < 0.0f)
//	color = texture2D(Texture0, TexCoord);
//else
//	color = vec4(vecColor, 1.0f);

}
