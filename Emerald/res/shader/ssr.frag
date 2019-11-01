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

uniform float screenEdgeFadeStart = 0.5;
const float rayStep = 0.1f;
const float minRayStep = 0.1f;
const float maxSteps = 128;
const float numBinarySearchSteps = 4;
const float reflectionSpecularFalloffExponent = 3.0f;

vec3 GetPosition(vec2 coord){
	float z = texture(_Depth, coord).x * 2.0f - 1.0f;
	vec4 clipSpacePosition = vec4(coord * 2.0 - 1.0, z, 1.0);
	vec4 viewSpacePosition = _InverseProjection * clipSpacePosition;
	viewSpacePosition /= viewSpacePosition.w;
	return viewSpacePosition.xyz;
}

vec3 GetPositionLOD(vec2 coord, float LOD){
	float z = textureLod(_Depth, coord, LOD).x * 2.0f - 1.0f;
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
		depth = GetPositionLOD(projectedCoord.xy, 2.0f).z;

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

		depth = GetPositionLOD(projectedCoord.xy, 2.0f).z;

		if(depth > 1000.0) continue;
 
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2) {
            if(dDepth <= 0.0) {   
                vec4 Result;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
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
    a = fract(a * vec3(0.8f));
    a += dot(a, a.yxz + 19.19f);
    return fract((a.xxy + a.yxx)*a.zyx);
}

void main() {
	vec4 misc = texture(_GMisc, fsUv);
	float metallic = 1;
	float specular = misc.x;

	vec3 viewNormal = normalize(vec3(texture(_GNormal, fsUv) * _InverseView));
	//vec3 viewPos = vec3(_View * vec4(GetPosition(fsUv), 1.0));
	vec3 viewPos = GetPosition(fsUv);
	vec3 albedo = texture(_GAlbedo, fsUv).rgb;

    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 fresnel = fresnelSchlick(max(dot(viewNormal, viewPos), 0.0), F0);

	vec3 reflected = normalize(reflect(normalize(viewPos), viewNormal));
	vec3 hitPos = viewPos;
	float dDepth;

	vec3 wp = vec3(vec4(viewPos, 1.0) * _InverseView);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), specular);
    vec4 coords = RayMarch(reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);
 
	//vec4 coords = RayCast(reflected * max(minRayStep, -viewPos.z), hitPos, dDepth);

	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5f, 0.5f) - coords.xy));
 
	float maxDimension = min( 1.0, max( abs( dCoords.x), abs( dCoords.y)));
	float screenEdgefactor = 1.0 - (max( 0.0, maxDimension - screenEdgeFadeStart) / (1.0 - screenEdgeFadeStart));
    //float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
    float multiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdgefactor * -reflected.z;
	vec3 SSR = texture(_HDR, coords.xy).rgb * clamp(multiplier, 0.0, 0.9) * fresnel;
	outColor = SSR;
}