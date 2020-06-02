#version 330

//HBAO https://blenderartists.org/t/bge-hbao-ambient-occlusion-shader/690374

#include "includes/globalUniforms.incl"
#include "includes/utils.incl"

out vec3 outColor;

in vec2 fsUv;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _Noise;

bool noise = false;

float rand(vec2 co)
{
    float width = _ViewPort.y;
    float height = _ViewPort.x;
    if (noise) {
        return fract(sin(dot(co.xy,vec2(12.9898,78.233)*4.0)) * 43758.5453)*2.0-1.0;
    } else {
        return ((fract(1.0-co.s*(width/2.0))*0.3)+(fract(co.t*(height/2.0))*0.6))*2.0-1.0;
    }
}

const float noiseamount = 0.8;          //Per-Pixel noise amount, bigger values need more performance
const float jitterAmount = 0.8;         //Per-Sample noise amount, bigger values need more performance

const vec2 noiseOffset2d = vec2(max(1.0 - noiseamount, 0.0), 0.0);

vec2 rand2D(vec2 coord) //generating noise/pattern texture for dithering
{
    float width = _ViewPort.y;
    float height = _ViewPort.x;
    float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
    float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;
    
    if (noise)
    {
        noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
        noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
    }
    return (normalize(vec2(noiseX,noiseY)) * noiseamount) + noiseOffset2d;
}

uniform int _Power;
uniform float _LumInfluence;
uniform float _AngleBias;
uniform float _AttenuationScale;
uniform int _SampleDirections;    //Main sample count, affects performance heavily 
uniform int _SampleSteps;         //SubSample count, sometimes higher values are faster
uniform float _SampleRadius;

const float TWO_PI = 6.283185307;

void main(){
    vec3 color = texture(_GAlbedo, fsUv).xyz;

    mat4 viewProjectionInverseMatrix = _InverseView * _InverseProjection;
    
    vec3 originVS =  GetViewPosition(_Depth, fsUv);
    vec3 normalVS = normalize(texture(_GNormal, fsUv).xyz);
    
    float radiusWS = 0.0;

    vec4 temp0 = viewProjectionInverseMatrix * vec4(0.0, 0.0, -1.0, 1.0);
    vec3 out0 = temp0.xyz;
    vec4 temp1 = viewProjectionInverseMatrix * vec4(_SampleRadius, 0.0, -1.0, 1.0);
    vec3 out1 = temp1.xyz;
    
	radiusWS = min(tan(_SampleRadius * _ViewPort.z * 0.0087266462597222) * originVS.z, length(out1 - out0));
    
    float theta = TWO_PI / float(_SampleDirections);
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    
    mat2 deltaRotationMatrix = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    vec2 deltaUV = vec2(1.0, 0.0) * (_SampleRadius / (float(_SampleDirections * _SampleSteps) + 1.0));
    
    vec2 sampleNoise = normalize(rand2D(fsUv));
    mat2 rotationMatrix = mat2(sampleNoise.x, -sampleNoise.y, sampleNoise.y,  sampleNoise.x);
    
    deltaUV = rotationMatrix * deltaUV;
    
    float occlusion = 0.0;
    float jitter = rand(fsUv) * jitterAmount;
    for(int i = 0; i < _SampleDirections; i++) {
        deltaUV = deltaRotationMatrix * deltaUV;
        
        vec2 sampleDirUV = deltaUV / originVS.z * vec2(20.0);
        float oldAngle = _AngleBias;
        
        for(int j = 0; j < _SampleSteps; j++) {
            vec2 offsetNoise = mod(jitter + vec2(j * 0.429, j * 0.555), 1.0);
            vec2 sampleUV = fsUv + (mix(vec2(1.0), offsetNoise, jitterAmount) * sampleDirUV);
            vec3 sampleVS = GetViewPosition(_Depth, sampleUV);
            vec3 sampleDirVS = (sampleVS - originVS);
            
            float gamma = 1.570796326 - acos(dot(normalVS, normalize(sampleDirVS)));
            
            if (gamma > oldAngle) {
                float value = sin(gamma) - sin(oldAngle);
                float attenuation = clamp(1.0 - pow(length(sampleDirVS) / radiusWS * _AttenuationScale, 2.0), 0.0, 1.0);
                occlusion += attenuation * value;
                oldAngle = gamma;
            }
        }
    }
    
    occlusion = 1.0 - occlusion / float(_SampleDirections);
    occlusion = clamp(pow(occlusion, 1.0 + _Power), 0.0, 1.0);
    
    vec3 lumcoeff = vec3(0.299,0.587,0.114);
    float lum = dot(color.rgb, lumcoeff) * _LumInfluence;
    
    occlusion = lum + ((1.0 - lum) * occlusion);
    outColor = vec3(occlusion);

    //gl_FragColor.a = occlusion;
}

//const float PI = 3.14159265;
//
//uniform vec2 FocalLen;
//uniform vec2 UVToViewA;
//uniform vec2 UVToViewB;
//
//uniform vec2 LinMAD;// = vec2(0.1-10.0, 0.1+10.0) / (2.0*0.1*10.0);
//
//vec2 AORes = vec2(1024.0, 768.0);
//vec2 InvAORes = vec2(1.0/1024.0, 1.0/768.0);
//vec2 NoiseScale = vec2(1024.0, 768.0) / 4.0;
//
//float AOStrength = 1.9;
//float R = 0.3;
//float R2 = 0.3*0.3;
//float NegInvR2 = - 1.0 / (0.3*0.3);
//float TanBias = tan(30.0 * PI / 180.0);
//float MaxRadiusPixels = 50.0;
//
//int NumDirections = 8;
//int NumSamples = 32;
//
//float ViewSpaceZFromDepth(float d)
//{
//	// [0,1] -> [-1,1] clip space
//	d = d * 2.0 - 1.0;
//
//	// Get view space Z
//	return -1.0 / (LinMAD.x * d + LinMAD.y);
//}
//
//vec3 UVToViewSpace(vec2 uv, float z)
//{
//	uv = UVToViewA * uv + UVToViewB;
//	return vec3(uv * z, z);
//}
//
//vec3 GetViewPosPoint(ivec2 uv)
//{
//	ivec2 coord = ivec2(gl_FragCoord.xy) + uv;
//	float z = texelFetch(_Depth, coord, 0).r;
//	return UVToViewSpace(uv, z);
//}
//
//float TanToSin(float x)
//{
//	return x * inversesqrt(x*x + 1.0);
//}
//
//float InvLength(vec2 V)
//{
//	return inversesqrt(dot(V,V));
//}
//
//float Tangent(vec3 V)
//{
//	return V.z * InvLength(V.xy);
//}
//
//float BiasedTangent(vec3 V)
//{
//	return V.z * InvLength(V.xy) + TanBias;
//}
//
//float Tangent(vec3 P, vec3 S)
//{
//    return -(P.z - S.z) * InvLength(S.xy - P.xy);
//}
//
//float Length2(vec3 V)
//{
//	return dot(V,V);
//}
//
//vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
//{
//    vec3 V1 = Pr - P;
//    vec3 V2 = P - Pl;
//    return (Length2(V1) < Length2(V2)) ? V1 : V2;
//}
//
//vec2 SnapUVOffset(vec2 uv)
//{
//    return round(uv * AORes) * InvAORes;
//}
//
//float Falloff(float d2)
//{
//	return d2 * NegInvR2 + 1.0f;
//}
//
//float HorizonOcclusion(	vec2 deltaUV,
//						vec3 P,
//						vec3 dPdu,
//						vec3 dPdv,
//						float randstep,
//						float numSamples)
//{
//	float ao = 0;
//
//	// Offset the first coord with some noise
//	vec2 uv = fsUv + SnapUVOffset(randstep*deltaUV);
//	deltaUV = SnapUVOffset( deltaUV );
//
//	// Calculate the tangent vector
//	vec3 T = deltaUV.x * dPdu + deltaUV.y * dPdv;
//
//	// Get the angle of the tangent vector from the viewspace axis
//	float tanH = BiasedTangent(T);
//	float sinH = TanToSin(tanH);
//
//	float tanS;
//	float d2;
//	vec3 S;
//
//	// Sample to find the maximum angle
//	for(float s = 1; s <= numSamples; ++s)
//	{
//		uv += deltaUV;
//		S = GetViewPosition(_Depth, uv);
//		tanS = Tangent(P, S);
//		d2 = Length2(S - P);
//
//		// Is the sample within the radius and the angle greater?
//		if(d2 < R2 && tanS > tanH)
//		{
//			float sinS = TanToSin(tanS);
//			// Apply falloff based on the distance
//			ao += Falloff(d2) * (sinS - sinH);
//
//			tanH = tanS;
//			sinH = sinS;
//		}
//	}
//	
//	return ao;
//}
//
//vec2 RotateDirections(vec2 Dir, vec2 CosSin)
//{
//    return vec2(Dir.x*CosSin.x - Dir.y*CosSin.y,
//                  Dir.x*CosSin.y + Dir.y*CosSin.x);
//}
//
//void ComputeSteps(inout vec2 stepSizeUv, inout float numSteps, float rayRadiusPix, float rand)
//{
//    // Avoid oversampling if numSteps is greater than the kernel radius in pixels
//    numSteps = min(NumSamples, rayRadiusPix);
//
//    // Divide by Ns+1 so that the farthest samples are not fully attenuated
//    float stepSizePix = rayRadiusPix / (numSteps + 1);
//
//    // Clamp numSteps if it is greater than the max kernel footprint
//    float maxNumSteps = MaxRadiusPixels / stepSizePix;
//    if (maxNumSteps < numSteps)
//    {
//        // Use dithering to avoid AO discontinuities
//        numSteps = floor(maxNumSteps + rand);
//        numSteps = max(numSteps, 1);
//        stepSizePix = MaxRadiusPixels / numSteps;
//    }
//
//    // Step size in uv space
//    stepSizeUv = stepSizePix * InvAORes;
//}
//
//void main(void)
//{
//	float numDirections = NumDirections;
//
//	vec3 P, Pr, Pl, Pt, Pb;
//	P 	= GetViewPosition(_Depth, fsUv);
//
//	// Sample neighboring pixels
//    Pr 	= GetViewPosition(_Depth, fsUv + vec2( InvAORes.x, 0));
//    Pl 	= GetViewPosition(_Depth, fsUv + vec2(-InvAORes.x, 0));
//    Pt 	= GetViewPosition(_Depth, fsUv + vec2( 0, InvAORes.y));
//    Pb 	= GetViewPosition(_Depth, fsUv + vec2( 0,-InvAORes.y));
//
//    // Calculate tangent basis vectors using the minimu difference
//    vec3 dPdu = MinDiff(P, Pr, Pl);
//    vec3 dPdv = MinDiff(P, Pt, Pb) * (AORes.y * InvAORes.x);
//
//    // Get the random samples from the noise texture
//	vec3 random = texture(_Noise, fsUv.xy * NoiseScale).rgb;
//
//	// Calculate the projected size of the hemisphere
//    vec2 rayRadiusUV = 0.5 * R * FocalLen / -P.z;
//    float rayRadiusPix = rayRadiusUV.x * AORes.x;
//
//    float ao = 1.0;
//
//    // Make sure the radius of the evaluated hemisphere is more than a pixel
//    if(rayRadiusPix > 1.0)
//    {
//    	ao = 0.0;
//    	float numSteps;
//    	vec2 stepSizeUV;
//
//    	// Compute the number of steps
//    	ComputeSteps(stepSizeUV, numSteps, rayRadiusPix, random.z);
//
//		float alpha = 2.0 * PI / numDirections;
//
//		// Calculate the horizon occlusion of each direction
//		for(float d = 0; d < numDirections; ++d)
//		{
//			float theta = alpha * d;
//
//			// Apply noise to the direction
//			vec2 dir = RotateDirections(vec2(cos(theta), sin(theta)), random.xy);
//			vec2 deltaUV = dir * stepSizeUV;
//
//			// Sample the pixels along the direction
//			ao += HorizonOcclusion(	deltaUV,
//									P,
//									dPdu,
//									dPdv,
//									random.z,
//									numSteps);
//		}
//
//		// Average the results and produce the final AO
//		ao = 1.0 - ao / numDirections * AOStrength;
//	}
//
//	outColor = vec3(ao, ao, ao);
//}