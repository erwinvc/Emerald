#version 330
in vec2 fsUV;

out vec3 outColor;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _SSAO;

layout (std140) uniform GlobalUniforms {
	vec3 _CameraPosition;
	mat4 _Projection;
	mat4 _View;
	mat4 _InverseProjection;
	mat4 _InverseView;
    bool _SSAOEnabled;
	vec2 _CameraPlanes;
	vec2 _ViewPort;
};

uniform vec4 _Color;
uniform vec3 _Directional;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 GetPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0f - 1.0f;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = inverse(_Projection) * clipSpacePosition;
	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldSpacePosition = inverse(_View) * viewSpacePosition;
	return viewSpacePosition.xyz;
}

void main(){
	vec4 misc = texture(_GMisc, fsUV);
	float metallic = misc.y;
	float roughness = misc.x;
	float lightInfluence = misc.w;

	vec3 albedo = texture(_GAlbedo, fsUV).rgb;
	vec3 N = texture(_GNormal, fsUV).xyz;
    N =  vec3(inverse(_View) * vec4(N, 0.0));
	vec3 viewPos = GetPosition(fsUV);
    vec3 worldPos = vec3(inverse(_View) * vec4(viewPos, 1.0));
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

	outColor = vec3(mix(albedo, color, lightInfluence));
}