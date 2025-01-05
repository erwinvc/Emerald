#version 430 core

struct Data {
    vec4 pos;
    vec2 uv;
    vec3 normal;
	mat3 TBN;
};

in Data vsData;
out vec4 FragColor;

uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform sampler2D _VSM;

uniform mat4 _LightSpaceMatrix;
uniform vec3 _ViewPosition;
uniform vec3 _LightPosition;
uniform vec3 _LightDirection;
uniform vec4 _BaseColor;
uniform float _Metallic;
uniform float _Roughness;

vec3 lightColor = vec3(1.0, 1.0, 1.0); 
float ambientStrength = 0.1;

float chebyshevShadow(vec4 lightSpacePos)
{
    // Transform position from world-space to light clip-space
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    // Remap to [0..1]
    projCoords = projCoords * 0.5 + 0.5;

    // Sample the VSM texture
    vec2 moments = texture(_VSM, projCoords.xy).rg;
    float receiverDepth = projCoords.z;   // The fragment's depth in [0..1]

    // 1) If the fragment is in front of E[z], it's guaranteed lit
    if (receiverDepth <= moments.x)
        return 1.0;

    // 2) Otherwise compute variance
    float minVariance = 0.000001; // prevents division by zero
    float variance    = max(moments.y - (moments.x * moments.x), minVariance);

    // 3) Chebyshev’s upper bound
    float d = receiverDepth - moments.x;
    // p_max is the probability the fragment is lit
    float p_max = variance / (variance + d * d);

    return clamp(p_max, 0.0, 1.0);
}

void main() {
	// Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Sample normal map and transform to tangent space [-1,1]
    vec3 normalMap = texture(_Normal, vsData.uv).rgb * 2.0 - 1.0;

    // Transform normal from tangent space to world space
    vec3 norm = normalize(vsData.TBN * normalMap);

	// Diffuse lighting
    float diff = max(dot(norm, _LightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
	
	// Specular lighting (Blinn-Phong)
    vec3 viewDir = normalize(_ViewPosition - vsData.pos.xyz);
    vec3 halfDir = normalize(_LightDirection + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = spec * lightColor; 
	
    // Shadow mapping
    vec4 lightSpacePos = _LightSpaceMatrix * vec4(vsData.pos.xyz, 1.0);
    float shadow = chebyshevShadow(lightSpacePos);

	// Combine all lighting components
    vec3 lighting = ambient + (diffuse + specular) * shadow;

    // Alpha clipping
	vec4 albedo = texture(_Albedo, vsData.uv).rgba;
	if (albedo.a < 0.2) discard;

    FragColor = _BaseColor * albedo * vec4(lighting, 1.0);
}