#version 400 core

in vec2 fsUv;

out vec3 outColor;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _HDR;

uniform mat4 _Projection;
uniform mat4 _View;
uniform mat4 _InverseProjection;
uniform mat4 _InverseView;
uniform vec3 _CameraPosition;

const float screenEdgeFadeStart = 0.5;
const float rayStep = 0.1f;
const float minRayStep = 0.001f;
const float maxSteps = 128;
const float numBinarySearchSteps = 8;
const float reflectionSpecularFalloffExponent = 3.0;

vec3 GetPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0f - 1.0f;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = inverse(_Projection) * clipSpacePosition;
	viewSpacePosition /= viewSpacePosition.w;
	return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
	float depth;
	vec4 projectedCoord;

	 for(int i = 0; i < numBinarySearchSteps; i++) {
		projectedCoord = _Projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;

		//depth = textureLod(_Depth, projectedCoord.xy, 2.0).x;
		depth = GetPosition(projectedCoord.xy).z;

		dDepth = hitCoord.z - depth;

		dir *=0.5f;

		if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
	}

	projectedCoord = _Projection * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}


vec4 RayMarch(in vec3 dir, inout vec3 hitCoord, out float dDepth) {
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


        if((dir.z - dDepth) < 1.2f && dDepth <= 0.0) {
            vec4 Result;
            Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

            return Result;
        }
		steps++;
	}

	return vec4(projectedCoord.xy, depth, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 hash(vec3 a)
{
    a = fract(a * 1);
    a += dot(a, a.yxz + 1);
    return fract((a.xxy + a.yxx)*a.zyx);
}

void main(){
	float metallic = texture(_GMisc, fsUv).y;
	float spec = texture(_GMisc, fsUv).x;

	if(GetPosition(fsUv).xyz == vec3(0) || metallic == 0) discard;

	
 
	vec3 viewPos = GetPosition(fsUv);
	vec3 viewNormal = normalize(texture(_GNormal, fsUv).rgb);

	vec3 albedo = texture(_GAlbedo, fsUv).rgb;
	

	
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, 0.5);
	vec3 fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);
	
	vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));
	
	vec3 hitPos = viewPos;
	float dDepth;
	
	vec3 wp = vec3(vec4(viewPos, 1.0) * _InverseView);
	vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
	vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
	
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	
	float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	
	float ReflectionMultiplier = pow(metallic, reflectionSpecularFalloffExponent) * 
	            screenEdgefactor * 
	            -reflected.z;
	
	vec4 SSR = vec4(texture(_HDR, coords.xy).rgb, clamp(ReflectionMultiplier, 0.0, 0.9) * fresnel);  

	outColor = vec3(SSR.r * SSR.a, SSR.g * SSR.a, SSR.b * SSR.a);
	//outColor = vec4(SSR.w * SSR.r, SSR.w* SSR.g, SSR.w * SSR.b, 1.0);
}