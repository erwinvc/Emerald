#version 330 core
out float FragColor;

in vec2 fsUv;

const int KERNELSIZE = 64;

uniform sampler2D _GPosition;
uniform sampler2D _GNormal;
uniform sampler2D _Noise;
uniform sampler2D _Depth;

uniform vec3 _Samples[KERNELSIZE];

uniform float _Radius;
uniform float _Bias;
uniform int _Power;

uniform mat4 _Projection;
uniform mat4 _View;

uniform vec2 _ScreenSize;
uniform vec2 _CameraPlanes;
//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // back to NDC 
//    return (2.0 * near * far) / (far + near - z * (far - near));    
//}
//
//vec3 posFromDepth(in vec2 Tex, in float d){ // get eye coordinate from depth
//    vec3 pos = vec3(Tex, d); 
//
//    // transform by the projection inverse 
//    vec4 clip = inverse(_Projection)*vec4(pos*2-1,1);
//
//    // divide by w to get the position. 
//    return clip.xyz/clip.w;
//}
//
//void main(){
//    float originDepth = texture(_Depth, fsUv).z;
//	vec3 originPos = posFromDepth(fsUv, originDepth);
//	vec3 fragPos = (_View * vec4(texture(_GPosition, fsUv).xyz, 1.0)).xyz;
//    vec3 normal = normalize(texture(_GNormal, fsUv).rgb);
//    vec3 randomVec = normalize(texture(_Noise, fsUv * _NoiseScale).xyz);
//    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
//    vec3 bitangent = cross(normal, tangent);
//    mat3 TBN = mat3(_View) * mat3(tangent, bitangent, normal);
//    float occlusion = 0.0;
//
//	float sampleDepth = 0.0;
//    for(int i = 0; i < KERNELSIZE; ++i)
//    {
//        vec3 sample = TBN * _Samples[i];
//        sample = fragPos + sample * _Radius; 
//       // vec3 sample = originPos + kernel[i];
//        vec4 offset = vec4(sample, 1.0);
//        offset = _Projection * offset;
//        offset.xyz /= offset.w;
//        offset.xyz = offset.xyz * 0.5 + 0.5;
//        
//		//float sampleDepth = (_View * vec4(texture(_GPosition, offset.xy).xyz, 1.0)).z;
//        //float sampleDepth = texture(_Depth, offset.xy).x;
//        //float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(fragPos.z - sampleDepth));
//        //occlusion += (sampleDepth >= sample.z + _Bias ? 1.0 : 0.0) * rangeCheck;           
//
//		sampleDepth = texture(_Depth, offset.xy).z;
//        sampleDepth = posFromDepth(offset.xy, sampleDepth).z;
//
//        float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(originPos.z - sampleDepth));
//        occlusion += step(sampleDepth,sample.z)*rangeCheck;
//    }
//    occlusion = 1.0 - (occlusion / KERNELSIZE);
//    occlusion = pow(occlusion, _Power);
//    FragColor = occlusion;
//}


float getLinearDepth( in vec2 uv, in vec2 zLinear )
{
  float depth = texture(_Depth, uv).r;
  return zLinear.y / (depth - zLinear.x);
}


/**
  Casts numSample rays in the fragment hemisphere (toward its normal)
  & update the ambientOcclusion term when their intersects occluders.
*/
void castRay( in vec3 random,
			  in vec2 zLinear,
              in vec3 position,
              in vec3 normal,
              in int numSample,
              in float radius,
              inout float ambientOcclusion )
{
  float ao = 0.0f;
  
  for(int i=0; i<numSample; ++i)
  {
    vec3 ray = radius * reflect(_Samples[i], random);
         ray *= sign(dot(ray,normal));

    vec3 samplePosition = position + ray;
    float sampleDepth = getLinearDepth( samplePosition.xy, zLinear );

    // test whereas fragment is behind an occluder or not
    float depthDiff = position.z - sampleDepth;

    // calculate the difference between the normals as a weight
    vec3 sampleNormal = texture( _GNormal, samplePosition.st).xyz;
    float normalDiff = 1.0f - dot( sampleNormal, normal);
  
    float occ = normalDiff * step(_Bias, depthDiff);
          occ *= 1.0f - smoothstep(_Bias, 1.5f,  depthDiff);

    ao += occ;
  }
  
  ambientOcclusion = ao / numSample;
}


void main(void){
  vec2 zLinear;
  zLinear.x = _CameraPlanes.y / (_CameraPlanes.y - _CameraPlanes.x);
  zLinear.y = -_CameraPlanes.x *  zLinear.x;

  vec2 uv = fsUv * _ScreenSize;

  // Per Fragment noise vector
  vec3 random = texture(_Noise, uv).xyz;
       random = 2.0f * random - 1.0f;

  vec3 normal = texture(_GNormal, fsUv).xyz;

  // fragment depth
  float fragDepth = getLinearDepth(fsUv, zLinear);

  // Screen-space fragment position
  vec3 position = vec3(fsUv.xy, fragDepth);


  float ao = 0.0f;

  castRay( random, zLinear, position, normal, 8, _Radius, ao);

  ao *= _Power;
  ao = 1 - ao;
  ao = clamp( ao, 0.0f, 1.0f);     
  
  FragColor = ao;
}