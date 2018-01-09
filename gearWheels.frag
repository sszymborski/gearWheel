#version 330 core

in vec3 vecColor;
in vec2 TexCoord;
in float TexID;

out vec4 color;

uniform sampler2D Texture0;		//metal
uniform sampler2D Texture1;		//drewno
uniform sampler2D Texture2;		//bialy
uniform sampler2D Texture3;		//podloga
uniform sampler2D Texture4;		//otoczenie
uniform vec3 lightColor;		//oswietlenie

void main()
{
   if(TexID < -0.6f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.01); // metal
	else if(TexID > 0.6f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture0, TexCoord), texture(Texture1, TexCoord), 0.99);	//drzewo
	else if(TexID > 0.4f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture3, TexCoord), texture(Texture1, TexCoord), 0.01); // podloga
	else if(TexID == - 0.5f)
		color = vec4(vecColor, 1.0f) * mix(texture(Texture4, TexCoord), texture(Texture1, TexCoord), 0.01); // skybox
	else
		color = vec4(vecColor, 1.0f) * mix(texture(Texture2, TexCoord), texture(Texture0, TexCoord), 0.01);	//bialy
		
		color *= vec4(lightColor, 1.0f);	//oswietlenie
}
