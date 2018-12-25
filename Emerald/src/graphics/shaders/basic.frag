#version 400 core

in vec2 pass_textureCoords;
in vec4 pass_worldPosition;
in vec3 pass_normal;

out vec4 out_Color;

uniform mat4 viewMatrix;
uniform vec3 eyePos;

uniform vec4 color;
uniform vec4 lightColor;
uniform float intensity;
uniform vec3 direction;
uniform sampler2D tex;

vec4 calcLight(vec3 direction, vec3 normal){
    float diffuseFactor = dot(normal, -direction);

    vec4 diffuseColor = vec4(0,0,0,0);
    vec4 specularColor = vec4(0,0,0,0);

    if(diffuseFactor > 0){
        diffuseColor = vec4(lightColor) * intensity * diffuseFactor;

        vec3 directionToEye = normalize(eyePos - pass_worldPosition.xyz);
        vec3 reflectDirection = normalize(reflect(direction, normal));

        float specularFactor = dot(directionToEye, reflectDirection);
        specularFactor = pow(specularFactor, 1);

        if(specularFactor > 0)
        {
            specularColor = vec4(lightColor) * specularFactor;
        }
    }
    return diffuseColor + specularColor;
}

vec4 calcDirectionalLight(vec3 normal){
    return calcLight(-direction, normal);
}
void main() {
    vec4 textureColor = texture(tex, pass_textureCoords);
    if(textureColor.a < 0.5) discard;
    textureColor = mix(textureColor, vec4(color.xyz, 1.0), color.a);
	out_Color = textureColor * calcDirectionalLight(normalize(pass_normal));
}
