#version 400 core

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GPosition;

in vec4 fsPos;
in vec4 newPos;
in vec4 color;

out vec4 outColor;
//out vec4 outBright;

//uniform float uLightRadius;
//uniform vec3 uLightPosition;
//uniform vec3 uLightColor;

uniform vec3 uCameraPos;
uniform float reflectivity;
uniform float shineDamper;

uniform float _Diffuse;
uniform float _Specular;

void main() {
    float uLightRadius = newPos.w;
    vec3 lightPos = newPos.xyz;
    vec3 uLightColor = color.rgb;

    vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;

	vec3 misc = texture(_GMisc, uv).xyz;
	vec3 albedo = texture(_GAlbedo, uv).xyz;
	vec3 normal = normalize(texture(_GNormal, uv).xyz);
	vec3 position = texture(_GPosition, uv).xyz;
    
    vec3 lightToPosVector = position.xyz - lightPos;
    float lightDist = length(lightToPosVector);  
    vec3 l = -lightToPosVector / (lightDist);
    
    float ztest = step(0.0, uLightRadius - lightDist);
    
    float attenuation = pow(clamp(1.0 - pow(lightDist / uLightRadius, 1.0), 0.0, 1.0), 2.0) / (lightDist * lightDist + 1.0);
    vec3 v = normalize(uCameraPos - position);
    vec3 h = normalize(l + v);

    vec3 color =
    _Diffuse * uLightColor * albedo * max(0.0, dot(normal, l)) +
    _Specular * uLightColor * reflectivity * pow(max(0.0, dot(h, normal)), shineDamper); 

    color *= ztest * attenuation;

	vec3 finalColor = mix(color, albedo, misc.y);
	outColor = vec4(finalColor, 1.0);

	//float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//float brightness = (outColor.r + outColor.g + outColor.b);
    //if(brightness > 3.0) outBright = vec4(outColor.rgb, 1.0);
    //else outBright = vec4(0.0, 0.0, 0.0, 1.0);
}