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

bool noise = false;

float rand(vec2 co)
{
    float width = _ViewPort.y;
    float height = _ViewPort.x;
    if (noise) {
        return fract(sin(dot(co.xy,vec2(12.9898,78.233)*3.0)) * 43758.5453);
    } else {
        return ((fract(1.0-co.s*(width/2.0))*0.3)+(fract(co.t*(height/2.0))*0.6))*2.0-1.0;
    }
}

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
    return vec2(noiseX,noiseY);
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
    mat4 viewProjectionInverseMatrix = _InverseView * _InverseProjection;
 
    vec3 color =  texture(_GAlbedo, fsUv).xyz;
    
    vec3 originVS =  GetViewPosition(_Depth, fsUv);
    vec3 normalVS = normalize(texture(_GNormal, fsUv)).xyz;
    
    float radiusWS = 0.0;

    vec4 temp0 = viewProjectionInverseMatrix * vec4(0.0, 0.0, -1.0, 1.0);
    vec3 out0 = temp0.xyz;
    vec4 temp1 = viewProjectionInverseMatrix * vec4(_SampleRadius, 0.0, -1.0, 1.0);
    vec3 out1 = temp1.xyz;
    
	radiusWS = min(tan(_SampleRadius * 90 * 0.0087266462597222) * originVS.z, length(out1 - out0)); //HARDCODED FOV TO 90
    
    float theta = TWO_PI / _SampleDirections;
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    
    mat2 deltaRotationMatrix = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
    vec2 deltaUV = vec2(1.0, 0.0) * (_SampleRadius / (float(_SampleDirections * _SampleSteps) + 1.0));
    
    vec2 sampleNoise = rand2D(fsUv);
    sampleNoise = vec2(0, 0);
    mat2 rotationMatrix = mat2(sampleNoise.x, -sampleNoise.y, 
                               sampleNoise.y,  sampleNoise.x);
    
    deltaUV = deltaUV;
    
    float occlusion = 0.0;
    float jitter = rand(fsUv) * 1.0f;
    for(int i = 0; i < _SampleDirections; i++) {
        deltaUV = deltaRotationMatrix * deltaUV;
        
        vec2 sampleDirUV = deltaUV;
        float oldAngle = _AngleBias;
        
        for(int j = 0; j < _SampleDirections; j++) {
            vec2 sampleUV = fsUv + (jitter + float(j)) * sampleDirUV;
            vec3 sampleVS = GetViewPosition(_Depth, sampleUV).xyz;
            vec3 sampleDirVS = (sampleVS - originVS);
            
            float gamma = 1.570796326 - acos(dot(normalVS, normalize(sampleDirVS)));
            
            if (gamma > oldAngle) {
                float value = sin(gamma) - sin(oldAngle);
                float attenuation = clamp(1.0 - pow(length(sampleDirVS) / radiusWS * _AttenuationScale, 2.0), 0.0, 1.0);
                occlusion += attenuation * value;
                //occlusion += value;
                oldAngle = gamma;
            }
        }
    }
    
    occlusion = 1.0 - occlusion / _SampleDirections;
    occlusion = clamp(pow(occlusion, 1.0 + _Power), 0.0, 1.0);
    
    vec3 lumcoeff = vec3(0.299,0.587,0.114);
    float lum = dot(color.rgb, lumcoeff) * _LumInfluence;
    
    occlusion = lum + ((1.0 - lum) * occlusion);
    outColor = vec3(occlusion);
}