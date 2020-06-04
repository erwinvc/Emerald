#version 330

#include "includes/globalUniforms.incl"
#include "includes/utils.incl"

in vec2 fsUv;

out vec3 outColor;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _HDR;

const float screenEdgeFadeStart = 0.5;
const float rayStep = 0.1;
const float minRayStep = 5;
const float maxSteps = 64;
const float numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

vec3 Scale = vec3(0.8);
float K = 19.19;

vec3 GetPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0 - 1.0;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = _InverseProjection * clipSpacePosition;
	viewSpacePosition /= viewSpacePosition.w;
	return viewSpacePosition.xyz;
}

vec2 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
	float depth;
	vec4 projectedCoord;

	 for(int i = 0; i < numBinarySearchSteps; i++) {
		projectedCoord = _Projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

		depth = GetPosition(projectedCoord.xy).z;

		dDepth = hitCoord.z - depth;

		dir *= 0.5;

		if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
	}

	projectedCoord = _Projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return projectedCoord.xy;
}


vec2 RayMarch(in vec3 dir, inout vec3 hitCoord, out float dDepth) {
	dir *= rayStep;
	
	float depth;
	int steps;
	vec4 projectedCoord;
	
	for(int i = 0; i < maxSteps; i++){
		hitCoord += dir;
		projectedCoord = _Projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

		depth = GetPosition(projectedCoord.xy).z;
		if(depth > 1000.0) continue;
        dDepth = hitCoord.z - depth;


        if((dir.z - dDepth) < 1.2 && dDepth <= 0.0) {
            return BinarySearch(dir, hitCoord, dDepth);
        }
		steps++;
	}
	return vec2(0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 hash(vec3 a){
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness){
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}  

void main(){
	vec3 viewPos = GetPosition(fsUv);

	vec4 misc = texture(_GMisc, fsUv);
	float metallic = misc.y;
	float roughness = misc.x;

	if(1.0 - roughness < 0.1) discard;

    if(viewPos == vec3(0) || metallic == 0) discard;
	if(texture(_Depth, fsUv).x > 0.9999999)  discard;
 
	vec3 worldPos = ViewPosToWorldPos(viewPos);
	vec3 viewNormal = normalize(texture(_GNormal, fsUv).rgb);
	vec3 worldNormal = ViewNormalToWorldNormal(viewNormal);
	vec3 viewDir = normalize(_CameraPosition - worldPos);

	vec3 albedo = texture(_GAlbedo, fsUv).rgb;

	
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);
	vec3 reflected = normalize(reflect(normalize(viewPos), viewNormal));
	vec3 fresnel = fresnelSchlickRoughness(max(dot(viewNormal, normalize(viewPos)), 0.0), F0, roughness);
	vec3 hitPos = viewPos;
	float dDepth;
	
	vec3 jitt = mix(vec3(0.0), vec3(hash(worldPos)), min(roughness, 0.01));
	vec2 coords = RayMarch(vec3(jitt) + reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);

	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	float reflectionMultiplier = pow(1 - roughness, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflected.z;
	
	float L = length(GetPosition(coords.xy).xyz - viewPos.xyz);
    L = clamp(L * 0.05, 0, 1);
    float error = 1 - L;

    vec3 SSR = texture(_HDR, coords.xy).rgb * error * clamp(reflectionMultiplier * fresnel, 0.0, 0.9);  
    outColor = SSR;
}