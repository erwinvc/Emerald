#version 330
in vec2 fsUV;

out vec4 outColor[2];

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GPosition;
uniform sampler2D _SSAO;

uniform vec4 _Color;
uniform vec3 _Directional;
uniform vec3 _CameraPosition;
uniform bool _SSAOEnabled;

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

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Shlick's approximation of the Fresnel factor.
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//uniform float _Roughness;
uniform float _Metallic = 0;
uniform float _BloomFactor;
void main(){
	vec3 misc = texture(_GMisc, fsUV).xyz;
	vec3 albedo = texture(_GAlbedo, fsUV).xyz;
	vec3 N = normalize(texture(_GNormal, fsUV).xyz);
	vec3 position = texture(_GPosition, fsUV).xyz;
	float ssao = texture(_SSAO, fsUV).x;
	float roughness = max(misc.x, 0.05);
	float lightInfluence = misc.y;

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, _Metallic);

	vec3 V = normalize(_CameraPosition - position);
    vec3 L = normalize(_Directional);
    vec3 H = normalize(V + L);
    vec3 radiance = _Color.rgb;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;
    
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - _Metallic;	  

    float NdotL = max(dot(N, L), 0.0); 

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * (_SSAOEnabled ? ssao : 1);

	vec3 ambient = vec3(0.03) * albedo;
	vec3 color = ambient + Lo;


	outColor[0] = vec4(mix(color, albedo, lightInfluence), 1.0);
	outColor[1] = max(outColor[0] - _BloomFactor, 0.0f);
}

//void main()
//{
//	vec3 misc = texture(_GMisc, FSUV).xyz;
//	vec3 albedo = texture(_GAlbedo, FSUV).xyz;
//	vec3 normal = normalize(texture(_GNormal, FSUV).xyz);
//	vec3 position = texture(_GPosition, FSUV).xyz;
//	float ssao = texture(_SSAO, FSUV).x;
//
//	float specular = misc.x;
//	float lightInfluence = misc.y;
//
//	vec3 l = normalize(_Directional);
//	vec3 v = normalize(_CameraPosition - position);
//	vec3 h = normalize(l + v);
//
//	vec3 color = 
//	_Diffuse * albedo.xyz * max(0.0, dot(normal.xyz, l)) +
//	specular * pow(max(0.0, dot(h, normal)), 32.0) +
//	_Ambient * albedo.xyz;
//
//	vec3 finalColor = mix(color * _Color.rgb * (_SSAOEnabled ? ssao : 1), albedo, misc.y);
//	//vec3 finalColor = mix(color * _Color.rgb, albedo, misc.y);
//	OutColor = vec4(finalColor, 1.0);
//}
