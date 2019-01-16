#version 460 core

in vec2 pass_texCoords;
in vec4 pass_worldPosition;
in vec3 pass_normals;
in mat3 pass_TBN;

out vec4 out_color;

uniform vec4 ambient;
uniform vec2 uvScale;

uniform sampler2D texDiffuse;
uniform sampler2D texNormal;

uniform float reflectivity;
uniform float shineDamper;

uniform vec3 eyePosition;

uniform struct Light {
  vec3 position;
  vec3 color;
  //vec3 attenuation;
} light;

void main()
{
	vec2 texCoords = uvScale * pass_texCoords;
    // diffuse 
	vec3 normal = texture(texNormal, texCoords).xyz;
	normal = normalize(normal * 255/128 - 1.0);
	normal = normalize(pass_TBN * normal);

	vec4 texColor = texture(texDiffuse, texCoords);

    vec3 lightDir = normalize(light.position - pass_worldPosition.xyz);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(eyePosition - pass_worldPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shineDamper);
    vec3 specular = reflectivity * spec * light.color;  
        
    vec3 result = (ambient.rgb + diffuse + specular) * texColor.rgb;
    out_color = vec4(result, 1.0);
} 