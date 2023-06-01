#version 460 core
out vec4 FragColor;

//in vec3 ourColor;
in vec2 TexCoord;
in vec4 TexCol;
in vec3 normal;
in vec3 lightS;
in vec3 modPos;
in vec3 viewP;


vec3 lightColor = vec3(0.4824, 0.5647, 0.8941);
//uniform sampler2D textures;
//uniform sampler2D tex;

void main()
{
   vec3 lightColor = vec3(0.1569, 0.1686, 0.9804);
   float ambStrength = 0.1;
   vec3 ambient = ambStrength*lightColor;
   float diff = max(dot(normal, lightS), 0.04f);
   float specularStrength = 2;
   vec3 viewDir = normalize(viewP - modPos);
   vec3 reflectDir = reflect(-lightS, normal);  

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
   vec3 specular = specularStrength * spec * lightColor; 


   FragColor = vec4((ambient + diff + specular)*TexCol.xyz, 1);
   //FragColor = vec4(0.4f, 0.5f, 0.8f, 1.0f);
   //FragColor = texture(tex, TexCoord);
}//