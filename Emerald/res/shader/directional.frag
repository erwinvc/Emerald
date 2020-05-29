#version 330

#include "includes/globalUniforms.incl"
#include "includes/utils.incl"
#include "includes/pbr.incl"

in vec2 fsUV;

out vec3 outColor;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _SSAO;
uniform sampler2D _Shadow;

uniform vec4 _Color;
uniform vec3 _Directional;
uniform mat4 _LightSpaceMatrix;

float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float SampleVarianceShadowMap(sampler2D shadowMap, vec2 coords, float compare){
    vec2 moments = texture2D(shadowMap, coords.xy).xy;

    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.00002);

    float d = compare - moments.x;
    float pMax = linstep( 0.2, 1.0, variance / (variance + d*d));

    return min(max(p, pMax), 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    return SampleVarianceShadowMap(_Shadow, projCoords.xy, projCoords.z);

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(_Shadow, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    float bias = 0.005;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(_Shadow, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(_Shadow, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main(){
	vec4 misc = texture(_GMisc, fsUV);
	float metallic = misc.y;
	float roughness = misc.x;

	vec3 albedo = texture(_GAlbedo, fsUV).rgb;
	vec3 viewNormal = normalize(texture(_GNormal, fsUV).xyz);
    vec3 N = ViewNormalToWorldNormal(viewNormal);
	vec3 viewPos = GetViewPosition(_Depth, fsUV);
    vec3 worldPos = ViewPosToWorldPos(viewPos);
	float ssao = texture(_SSAO, fsUV).x;

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	vec3 V = normalize(_CameraPosition - worldPos); 
    vec3 L = normalize(-_Directional);
    vec3 H = normalize(V + L);

    vec3 radiance = _Color.rgb;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);
    
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;	  

    float NdotL = max(dot(N, L), 0.0); 

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 color = Lo * ssao;

    vec4 fragPosLightSpace = _LightSpaceMatrix * vec4(worldPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);     

	outColor = color * shadow;
}