#version 330 core
out float FragColor;

in vec2 fsUv;

const int KERNELSIZE = 64;

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GPosition;
uniform sampler2D _Noise;

uniform vec3 _Samples[KERNELSIZE];

uniform int _SampleCount;
uniform float _Radius;
uniform float _Bias;
uniform int _Power;

uniform mat4 _Projection;
uniform mat4 _View;

uniform vec2 _ScreenSize;
uniform vec2 _CameraPlanes;

//float getLinearDepth( in vec2 uv, in vec2 zLinear )
//{
//  float depth = texture(_Depth, uv).r;
//  return zLinear.y / (depth - zLinear.x);
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

void main(){
	vec4 misc = texture(_GMisc, fsUv);
	vec3 fragPos = (_View * vec4(texture(_GPosition, fsUv).xyz, 1.0)).xyz;
    vec3 normal = normalize(texture(_GNormal, fsUv).rgb);
    vec3 randomVec = normalize(texture(_Noise, fsUv * _ScreenSize).xyz);
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(_View) * mat3(tangent, bitangent, normal);
    float occlusion = 0.0;
    for(int i = 0; i < _SampleCount; ++i)
    {
        vec3 sample = TBN * _Samples[i];
        sample = fragPos + sample * _Radius; 
        
        vec4 offset = vec4(sample, 1.0);
        offset = _Projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        float sampleDepth = (_View * vec4(texture(_GPosition, offset.xy).xyz, 1.0)).z;
        
        float rangeCheck = smoothstep(0.0, 1.0, _Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth <= sample.z + _Bias ? 1.0 : 0.0) * rangeCheck;      
	}
    occlusion = (occlusion / _SampleCount);
    occlusion = pow(occlusion, _Power);
    FragColor = occlusion;
}

//HBAO https://blenderartists.org/t/bge-hbao-ambient-occlusion-shader/690374
//float width = 1920;
//float height = 1080;
//vec2 rand2D(vec2 coord) //generating noise/pattern texture for dithering
//{
//    float noiseX = ((fract(1.0-coord.s*(width/2.0))*0.25)+(fract(coord.t*(height/2.0))*0.75))*2.0-1.0;
//    float noiseY = ((fract(1.0-coord.s*(width/2.0))*0.75)+(fract(coord.t*(height/2.0))*0.25))*2.0-1.0;
//    //noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
//    //noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
//    //if (noise)
//    //{
//    //    noiseX = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0-1.0;
//    //    noiseY = clamp(fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0-1.0;
//    //}
//    return vec2(noiseX,noiseY)*1.0f;
//}
//
//float rand(vec2 co)
//{
//    //if (noise) {
//        //return fract(sin(dot(co.xy,vec2(12.9898,78.233)*3.0)) * 43758.5453);
//    //} else {
//        return ((fract(1.0-co.s*(width/2.0))*0.3)+(fract(co.t*(height/2.0))*0.6))*2.0-1.0;
//    //}
//}
//
//uniform float lumInfluence = 0.3f;
//uniform float angleBias = 0.2f;
//uniform float attenuationScale = 1.0f;
//uniform float sampleDirections = 10.0f;    //Main sample count, affects performance heavily 
//uniform float sampleSteps = 32.0f;         //SubSample count, sometimes higher values are faster
//uniform float sampleRadius = 9.0f;
//const float fov = 70.0f;     //FoV
//float thfov = tan(fov * 0.0087266462597222);
//float aspectratio = 1920.0f/1080.0f;
//const float TWO_PI = 6.283185307;
//
//void main(){
//    mat4 VPM = _View * _Projection;
//    mat4 viewProjectionInverseMatrix  = inverse(VPM);
// 
//    vec3 color =  texture(_GAlbedo, fsUv).xyz;
//    
//    vec3 originVS =  texture(_GPosition, fsUv).xyz;
//    vec3 normalVS = normalize(texture(_GNormal, fsUv)).xyz;
//    
//    float radiusSS = 0.0;
//    float radiusWS = 0.0;
//
//    radiusSS = sampleRadius;
//    vec4 temp0 = viewProjectionInverseMatrix * vec4(0.0, 0.0, -1.0, 1.0);
//    vec3 out0 = temp0.xyz;
//    vec4 temp1 = viewProjectionInverseMatrix * vec4(radiusSS, 0.0, -1.0, 1.0);
//    vec3 out1 = temp1.xyz;
//    
//	radiusWS = min(tan(radiusSS * fov * 0.0087266462597222) * originVS.z, length(out1 - out0));
//    
//    float theta = TWO_PI / float(sampleDirections);
//    float cosTheta = cos(theta);
//    float sinTheta = sin(theta);
//    
//    mat2 deltaRotationMatrix = mat2(cosTheta, -sinTheta, sinTheta, cosTheta);
//    vec2 deltaUV = vec2(1.0, 0.0) * (radiusSS / (float(sampleDirections * sampleSteps) + 1.0));
//    
//    vec2 sampleNoise = rand2D(fsUv);
//    mat2 rotationMatrix = mat2(sampleNoise.x, -sampleNoise.y, 
//                               sampleNoise.y,  sampleNoise.x);
//    
//    deltaUV = rotationMatrix * deltaUV;
//    
//    float occlusion = 0.0;
//    float jitter = rand(fsUv) * 1.0f;
//    
//    for(int i = 0; i < sampleDirections; i++) {
//        deltaUV = deltaRotationMatrix * deltaUV;
//        
//        vec2 sampleDirUV = deltaUV;
//        float oldAngle = angleBias;
//        
//        for(int j = 0; j < sampleDirections; j++) {
//            vec2 sampleUV = fsUv + (jitter + float(j)) * sampleDirUV;
//            vec3 sampleVS = texture(_GPosition, sampleUV).xyz;
//            vec3 sampleDirVS = (sampleVS - originVS);
//            
//            float gamma = 1.570796326 - acos(dot(normalVS, normalize(sampleDirVS)));
//            
//            if (gamma > oldAngle) {
//                float value = sin(gamma) - sin(oldAngle);
//                
//               //if(useAttenuation) {
//                   float attenuation = clamp(1.0 - pow(length(sampleDirVS) / radiusWS * attenuationScale, 2.0), 0.0, 1.0);
//                   occlusion += attenuation * value;
//               //} else {
//                   // occlusion += value;
//               // }
//                
//                oldAngle = gamma;
//            }
//        }
//    }
//    
//    occlusion = 1.0 - occlusion / float(sampleDirections);
//    occlusion = clamp(pow(occlusion, 1.0 + _Power), 0.0, 1.0);
//    
//    vec3 lumcoeff = vec3(0.299,0.587,0.114);
//    float lum = dot(color.rgb, lumcoeff) * lumInfluence;
//    
//    occlusion = lum + ((1.0 - lum) * occlusion);
//    FragColor = occlusion;
//}