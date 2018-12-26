#version 400 core

in vec2 pass_textureCoords;
in vec4 pass_worldPosition;
in mat3 tbnMatrix;

out vec4 out_Color;

uniform mat4 viewMatrix;
uniform vec3 eyePos;

uniform vec4 color;
uniform vec4 lightColor;
uniform float intensity;
uniform float specularIntensity;
uniform float specularPower;
uniform vec3 direction;
uniform sampler2D tex;
uniform sampler2D texNormal;

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
	vec3 normalMapValue = normalize(tbnMatrix * (255/128 * texture(texNormal, pass_textureCoords).xyz - 1.0));
    vec4 textureColor = texture(tex, pass_textureCoords);
    
	if(textureColor.a < 0.5) discard;
    textureColor = mix(textureColor, vec4(color.xyz, 1.0), color.a);
	out_Color = textureColor * CalcLight(lightColor.rgb, intensity, direction, normalMapValue, pass_worldPosition.xyz, specularIntensity, specularPower, eyePos);
}
