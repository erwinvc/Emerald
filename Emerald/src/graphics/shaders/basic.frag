#version 400 core

in vec2 pass_textureCoords;
in vec4 pass_worldPosition;
in mat3 tbnMatrix;

in vec3 toCameraVector;
in vec3 toLightVector[4];

out vec4 out_Color;

uniform mat4 viewMatrix;
uniform vec3 eyePos;

uniform vec4 color;
uniform vec4 lightColor[4];
uniform float intensity;
uniform float specularIntensity;
uniform float specularPower;
uniform vec3 direction;
uniform sampler2D tex;
uniform sampler2D texNormal;

uniform float ambientIntensity;

vec4 CalcLight(vec3 color, float intensity, vec3 direction, vec3 normal, vec3 worldPos, float specularIntensity, float specularPower, vec3 eyePos)
{
    float diffuseFactor = clamp(dot(normal, -direction), 0.0, 1.0);
    
    vec4 diffuseColor = vec4(0,0,0,0);
    vec4 specularColor = vec4(0,0,0,0);
    
    diffuseColor = vec4(color, 1.0) * intensity * diffuseFactor;
        
    vec3 directionToEye = normalize(eyePos - worldPos);
    vec3 reflectDirection = normalize(reflect(direction, normal));
    
    float specularFactor = dot(directionToEye, reflectDirection);
    specularFactor = clamp(pow(specularFactor, specularPower), 0.0, 1.0);
        
    if(specularFactor > 0)
    {
        specularColor = vec4(color, 1.0) * specularIntensity * specularFactor;
    }
    
    return diffuseColor + specularColor;
}

void main() {
	vec3 normal = texture(texNormal, pass_textureCoords).xyz;
	normal = normalize(normal * 255/128 - 1.0);
	normal = normalize(tbnMatrix * normal);

    vec4 textureColor = texture(tex, pass_textureCoords);
    
	if(textureColor.a < 0.5) discard;
    textureColor = mix(textureColor, vec4(color.xyz, 1.0), color.a);
	vec4 ambient = textureColor * ambientIntensity;
	out_Color = textureColor * 1 + ambient; //CalcLight(lightColor.rgb, intensity, direction, normal, pass_worldPosition.xyz, specularIntensity, specularPower, eyePos) + ambient;


	vec3 unitVectorToCamera = normalize(toCameraVector);

    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for(int i = 0; i < 4; i++){
        float distance = length(toLightVector[i]);
        float attFactor = 1 + (0.01f * distance) + (0.002f * distance * distance);
        vec3 unityLightVector = normalize(toLightVector[i]);
        float nDotl = dot(normal, unityLightVector);
        float brightness = max(nDotl, 0.0);
        vec3 lightDirection = -unityLightVector;
        vec3 reflectedLightDirection = reflect(lightDirection, normal);
        float specularFactor = dot(reflectedLightDirection, unitVectorToCamera);
        specularFactor = max(specularFactor, 0.0);
        float dampedFactor = pow(specularFactor, specularPower);
        totalDiffuse += (brightness * vec3(1, 1, 1))/attFactor;
        totalSpecular += (dampedFactor * lightColor[i].rgb * specularIntensity)/attFactor;
    }

    totalDiffuse = max(totalDiffuse, 0.1);
	out_Color = vec4(totalDiffuse, 1) * textureColor + vec4(totalSpecular, 0);
}
