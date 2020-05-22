#version 330

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _SSAO;

uniform samplerCube depthMap;

in vec4 fsPos;
in vec4 fsPos2;
in vec4 newPos;
in vec4 fsColor;

out vec3 outColor;
//out vec4 outBright;

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

vec3 GetWorldPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0f - 1.0f;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = inverse(_Projection) * clipSpacePosition;

	viewSpacePosition /= viewSpacePosition.w;
	vec4 worldSpacePosition = inverse(_View) * viewSpacePosition;
	return worldSpacePosition.xyz;
}

uniform float Far;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


//float ShadowCalculation(vec3 fragPos, float nDotL)
//{
//    // get vector between fragment position and light position
//    vec3 fragToLight = fragPos - newPos.xyz;
//    // ise the fragment to light vector to sample from the depth map    
//    float closestDepth = texture(depthMap, fragToLight).r;
//    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
//    closestDepth *= Far;
//    // now get current linear depth as the length between the fragment and light position
//    float currentDepth = length(fragToLight);
//    // test for shadows
//
//    //float bias = 0.005*tan(acos(nDotL)); // cosTheta is dot( n,l ), clamped between 0 and 1
//    //bias = clamp(bias, 0,0.01);
//    float bias = 0.005; // we use a much larger bias since depth is now in [near_plane, far_plane] range
//    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;        
//    // display closestDepth as debug (to visualize depth cubemap)
//    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
//        
//    return shadow;
//}

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - newPos.xyz;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.015;
    int samples = 20;
    float viewDistance = length(_CameraPosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / Far)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= Far;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

//void main(){
//	float uLightRadius = newPos.w;
//	vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;
//	vec4 misc = texture(_GMisc, uv);
//	float roughness = max(misc.x, 0.05);
//	float metallic = misc.y;
//	float lightInfluence = misc.w;
//	if(lightInfluence == 0) discard;
//
//	vec3 albedo = texture(_GAlbedo, uv).xyz;
//	vec3 N = mat3(inverse(_View)) * normalize(texture(_GNormal, uv).xyz);
//	vec3 position = GetPosition(uv);
//	//vec3 position = GetPosition(uv).xyz;
//	float ssao = texture(_SSAO, uv).x;
//
//
//	vec3 F0 = vec3(0.04); 
//	F0 = mix(F0, albedo, metallic);
//
//	vec3 V = normalize(_CameraPosition - position);
//	vec3 lightPos = newPos.xyz;
//    vec3 lightToPosVector = position.xyz - lightPos;
//    float lightDist = length(lightToPosVector);  
//    //vec3 L = -lightToPosVector / (lightDist);
//	vec3 L = normalize(lightPos - position);
//    vec3 H = normalize(V + L);
//    //float attenuation = clamp(1.0 - lightDist/(uLightRadius), 0.0, 1.0);
//	//attenuation *= attenuation; 
//	//float attenuation = 1.0 / (1.0 + 0.1*lightDist + 0.01*lightDist*lightDist); 
//	//float attenuation = 1.0f / lightDist; //Not gamma!
//	float attenuation = 1.0f / (lightDist * lightDist); //Gamma!
//    vec3 radiance = fsColor.rgb * attenuation;
//
//    float NDF = DistributionGGX(N, H, roughness);   
//    float G   = GeometrySmith(N, V, L, roughness);      
//    vec3 F    = fresnelSchlick(max(dot(N, V), 0.0), F0);
//       
//    vec3 kD = vec3(1.0) - F;
//    kD *= 1.0 - metallic;	  
//
//    float NdotL = max(dot(N, L), 0.0);        
//
//	vec3 nominator    = NDF * G * F; 
//    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL + 0.001;
//    vec3 specular = nominator / max(denominator, 0.001); 
//
//    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * (_SSAOEnabled ? ssao : 1);
//
//	vec3 ambient = vec3(0.03) * albedo;
//	vec3 color = ambient + Lo;
//    float ztest = step(0.0, uLightRadius - lightDist);
//
//	 float d = lightDist / uLightRadius;
//    float at = 1.0 - d;
//
//    float shadow = ShadowCalculation(position.xyz);                      
//	color *= ztest * at * ssao;
//	outColor = color * (1.0 - shadow);
//}

void main() {
  
  // get screen-space position of light sphere
  // (remember to do perspective division.)
    vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;
    vec3 lightPos = newPos.xyz;
    float uLightRadius = newPos.w;
  // now we can sample from the gbuffer for every fragment the light sphere covers.
    vec3 albedo = texture(_GAlbedo, uv).xyz;
  
    vec3 viewNormal = normalize(texture(_GNormal, uv).xyz);
    vec3 worldNormal = vec3(_InverseView * vec4(viewNormal, 0.0));
    vec3 worldPos = GetWorldPosition(uv);
  
    vec3 lightToPosVector = worldPos.xyz - lightPos;
    float lightDist = length(lightToPosVector);  // position from light.
  
    vec3 l = -lightToPosVector / (lightDist);
  
  // implement fake z-test. If too far from light center, then 0.
    float ztest = step(0.0, uLightRadius - lightDist);
  // light attenuation.
    float d = lightDist / uLightRadius;
    float attenuation = 1.0 - d;
    vec3 v = normalize(_CameraPosition - worldPos);
    vec3 h = normalize(l + v);
  
  float nDotL = dot(worldNormal.xyz, l);
    vec3 color =
  // diffuse
    fsColor.rgb * albedo.xyz * max(0.0, nDotL) +
  // specular
    fsColor.rgb * 0.4 * pow(max(0.0, dot(h, worldNormal)), 12.0); 
  
  // finally ztest and attenuation.
    color *= ztest * attenuation;
    float shadow = ShadowCalculation(worldPos.xyz);       
    outColor = color * (1.0 - shadow);
    //outColor = vec3(shadow);
}