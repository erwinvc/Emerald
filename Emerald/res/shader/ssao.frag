#version 330 core
out float FragColor;

in vec2 fsUv;

const int KERNELSIZE = 64;

uniform sampler2D _GPosition;
uniform sampler2D _GNormal;
uniform sampler2D _Noise;

uniform vec3 _Samples[KERNELSIZE];

uniform float _Radius;
uniform float _Bias;
uniform int _Power;

uniform vec2 _NoiseScale;
uniform mat4 _Projection;
uniform mat4 _View;

void main(){
	vec3 fragPos = (_View * vec4(texture(_GPosition, fsUv).xyz, 1.0)).xyz;
    vec3 normal = normalize(texture(_GNormal, fsUv).rgb);
    vec3 randomVec = normalize(texture(_Noise, fsUv * _NoiseScale).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(_View) * mat3(tangent, bitangent, normal);
    float occlusion = 0.0;
    for(int i = 0; i < KERNELSIZE; ++i)
    {
        vec3 sample = TBN * _Samples[i];
        sample = fragPos + sample * _Radius; 
        
        vec4 offset = vec4(sample, 1.0);
        offset = _Projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
		 float sampleDepth = (_View * vec4(texture(_GPosition, offset.xy).xyz, 1.0)).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + _Bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / KERNELSIZE);
    occlusion = pow(occlusion, _Power);
    FragColor = occlusion;
}

