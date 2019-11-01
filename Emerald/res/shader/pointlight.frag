#version 400 core

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _SSAO;

uniform mat4 _Projection;
uniform mat4 _View;

in vec4 fsPos;
in vec4 newPos;
in vec4 color;

out vec3 outColor[2];
//out vec4 outBright;

//uniform float uLightRadius;
//uniform vec3 uLightPosition;
//uniform vec3 uLightColor;

uniform vec3 _CameraPosition;
uniform float shineDamper;
uniform bool _SSAOEnabled;
uniform float _BloomFactor;
uniform float _Diffuse;

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
//uniform float _Metallic;

vec3 GetPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0f - 1.0f;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = inverse(_Projection) * clipSpacePosition;

	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldSpacePosition = inverse(_View) * viewSpacePosition;
	return worldSpacePosition.xyz;
}

void main(){
	float uLightRadius = newPos.w;
	vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;
	vec4 misc = texture(_GMisc, uv);
	float roughness = max(misc.x, 0.05);
	float metallic = misc.y;
	float lightInfluence = misc.w;
	if(lightInfluence == 0) discard;

	vec3 albedo = texture(_GAlbedo, uv).xyz;
	vec3 N = normalize(texture(_GNormal, uv).xyz);
	vec3 position = GetPosition(uv);
	//vec3 position = GetPosition(uv).xyz;
	float ssao = texture(_SSAO, uv).x;


	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	vec3 V = normalize(_CameraPosition - position);
	vec3 lightPos = newPos.xyz;
    vec3 lightToPosVector = position.xyz - lightPos;
    float lightDist = length(lightToPosVector);  
    //vec3 L = -lightToPosVector / (lightDist);
	vec3 L = normalize(lightPos - position);
    vec3 H = normalize(V + L);
    float attenuation = clamp(1.0 - lightDist/(uLightRadius), 0.0, 1.0);
	attenuation *= attenuation; 
	//float attenuation = 1.0 / (1.0 + 0.1*lightDist + 0.01*lightDist*lightDist); 
	//float attenuation = 1.0f / (lightDist * lightDist);
    vec3 radiance = color.rgb * attenuation;

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(N, V), 0.0), F0);
       
    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - metallic;	  

    float NdotL = max(dot(N, L), 0.0);        

	vec3 nominator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
    vec3 specular = nominator / max(denominator, 0.001); 

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * (_SSAOEnabled ? ssao : 1);

	vec3 ambient = vec3(0.03) * albedo;
	vec3 color = ambient + Lo;
    float ztest = step(0.0, uLightRadius - lightDist);

	 float d = lightDist / uLightRadius;
    float at = 1.0 - d;

	color *= ztest * at;
	outColor[0] = mix(albedo, color, lightInfluence);
	outColor[1] = max(outColor[0] - _BloomFactor, 0.0f);
}

//void main3() {
//    float uLightRadius = newPos.w;
//    vec3 lightPos = newPos.xyz;
//    vec3 uLightColor = color.rgb;
//
//    vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;
//
//	vec3 misc = texture(_GMisc, uv).xyz;
//	vec3 albedo = texture(_GAlbedo, uv).xyz;
//	vec3 normal = normalize(texture(_GNormal, uv).xyz);
//	vec3 position = texture(_GPosition, uv).xyz;
//    float ssao = texture(_SSAO, uv).x;
//
//	float specular = misc.x;
//	float lightInfluence = misc.y;
//
//    vec3 lightToPosVector = position.xyz - lightPos;
//    float lightDist = length(lightToPosVector);  
//    vec3 l = -lightToPosVector / (lightDist);
//    
//    float ztest = step(0.0, uLightRadius - lightDist);
//    
//    float attenuation = pow(clamp(1.0 - pow(lightDist / uLightRadius, 1.0), 0.0, 1.0), 2.0) / (lightDist * lightDist + 1.0);
//    vec3 v = normalize(uCameraPos - position);
//    vec3 h = normalize(l + v);
//
//    vec3 color =
//    _Diffuse * uLightColor * albedo * max(0.0, dot(normal, l)) +
//    uLightColor * specular * pow(max(0.0, dot(h, normal)), shineDamper); 
//
//    color *= ztest * attenuation;
//
//	vec3 finalColor = mix(color, albedo, lightInfluence);
//	outColor = vec4(finalColor * (_SSAOEnabled ? ssao : 1), 1.0);
//
//
//	//float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
//	//float brightness = (outColor.r + outColor.g + outColor.b);
//    //if(brightness > 3.0) outBright = vec4(outColor.rgb, 1.0);
//    //else outBright = vec4(0.0, 0.0, 0.0, 1.0);
//}