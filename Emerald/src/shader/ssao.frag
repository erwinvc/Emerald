#version 330 core
out vec4 FragColor;

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

void main()
{
    // get input for SSAO algorithm
	vec3 fragPos = (_View * vec4(texture(_GPosition, fsUv).xyz, 1.0)).xyz;
    vec3 normal = mat3(_View) * normalize(texture(_GNormal, fsUv).rgb);
    vec3 randomVec = normalize(texture(_Noise, fsUv * _NoiseScale).xyz);
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < KERNELSIZE; ++i)
    {
        // get sample position
        vec3 sample = TBN * _Samples[i]; // from tangent to view-space
        sample = fragPos + sample * _Radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = _Projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
		 float sampleDepth = (_View * vec4(texture(_GPosition, offset.xy).xyz, 1.0)).z;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + _Bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / KERNELSIZE);
    occlusion = pow(occlusion, _Power);
    FragColor = vec4(occlusion, occlusion, occlusion, 1);
}

