#version 330

#include "includes/globalUniforms.incl"
#include "includes/utils.incl"

out vec3 outColor;

in vec2 fsUv;

const int KERNELSIZE = 64;

uniform sampler2D _Depth;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GMisc;
uniform sampler2D _Noise;

uniform vec3 _Samples[KERNELSIZE];

uniform int _SampleCount;
uniform float _Radius;
uniform float _Bias;
uniform int _Power;

void main(){
    vec2 screenSize = _ViewPort.yx / vec2(4.0);
	vec3 fragPos = GetViewPosition(_Depth, fsUv);
    vec3 normal = texture(_GNormal, fsUv).xyz;
    vec3 randomVec = normalize(texture(_Noise, fsUv * screenSize).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for(int i = 0; i < _SampleCount; i++)
    {
        vec3 sample = TBN * _Samples[i];
        sample = fragPos + sample * _Radius; 
        
        vec4 offset = vec4(sample, 1.0);
        offset = _Projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = GetViewPosition(_Depth, offset.xy).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + _Bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / _SampleCount);
    occlusion = clamp(pow(occlusion, 1.0 + _Power), 0.0, 1.0);

    outColor = vec3(occlusion, occlusion, occlusion);
}