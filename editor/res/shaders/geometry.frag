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
uniform float _SSSIntensity = 1.0;
uniform float _SSSScale = 0.8;
uniform float _SSSPower = 12.0;

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

float subsurfaceScattering(vec3 N, vec3 L, vec3 V) {
    // Calculate light falloff using dot product between normal and light direction
    float NL = max(dot(N, L), 0.0);
    
    // View-dependent scattering: light entering the surface
    float NV = max(dot(N, V), 0.0);
    
    // Simulate light diffusion under the surface
    float scatter = exp(-pow(1.0 - NV, _SSSPower) / _SSSScale);
    
    return mix(NL, scatter, _SSSIntensity);
}

void main() {
	// Ambient lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Sample normal map and transform to tangent space
    vec3 normalMap = texture(_Normal, vsData.uv).rgb * 2.0 - 1.0;
    vec3 norm = normalize(vsData.TBN * normalMap);

    // Light and view directions
    vec3 L = normalize(_LightPosition - vsData.pos.xyz); 
    vec3 V = normalize(_ViewPosition - vsData.pos.xyz);

	// Diffuse lighting
    //float sss = subsurfaceScattering(norm, L, V);
    float diff = max(dot(norm, _LightDirection), 0.0);
    vec3 diffuse = diff * lightColor;// * sss; 
    
	// Specular lighting (Blinn-Phong)
    vec3 halfDir = normalize(L + V);
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