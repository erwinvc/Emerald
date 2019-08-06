#version 330 core
out vec4 out_color;
  
in vec2 fsUv;

uniform sampler2D _HDRBuffer;
uniform int _ApplyPostProcessing;
uniform float _Gamma;
uniform bool _FXAA;
uniform float _Exposure;
uniform int _Tonemapping;

//void main()
//{             
//    const float gamma = 1;
//    vec3 hdrColor = texture(hdrBuffer, textureCoords).rgb;
//    if(true)
//    {
//       //// reinhard
//       //// vec3 result = hdrColor / (hdrColor + vec3(1.0));
//       //// exposure
//       //vec3 result = vec3(1.0) - exp(-hdrColor * 1);
//       //// also gamma correct while we're at it       
//       //result = pow(result, vec3(1.0 / gamma));
//       //out_color = vec4(result, 1.0);
//
//		vec3 tonemappedColour = hdrColor / (hdrColor + vec3(1.0));
//
//	// Apply gamma correction
//		out_color = vec4(pow(tonemappedColour, vec3(gamma)), 1.0);
//    }
//    else
//    {
//        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
//        out_color = vec4(result, 1.0);
//    }
//}  

float FXAA_SPAN_MAX = 8.0f;
float FXAA_REDUCE_MUL = 1.0f/8.0f;
float FXAA_REDUCE_MIN = 1.0f/128.0f;
float middleGrey = 0.18f;

vec3 computeFxaa()
{
    vec2 screenTextureOffset = vec2(1.0f/1920.0f, 1.0f/1080.0f);
    vec3 luma = vec3(0.299f, 0.587f, 0.114f);

    vec3 offsetNW = texture(_HDRBuffer, fsUv + (vec2(-1.0f, -1.0f) * screenTextureOffset)).xyz;
    vec3 offsetNE = texture(_HDRBuffer, fsUv + (vec2(1.0f, -1.0f) * screenTextureOffset)).xyz;
    vec3 offsetSW = texture(_HDRBuffer, fsUv + (vec2(-1.0f, 1.0f) * screenTextureOffset)).xyz;
    vec3 offsetSE = texture(_HDRBuffer, fsUv + (vec2(1.0f, 1.0f) * screenTextureOffset)).xyz;
    vec3 offsetM  = texture(_HDRBuffer, fsUv).xyz;

    float lumaNW = dot(luma, offsetNW);
    float lumaNE = dot(luma, offsetNE);
    float lumaSW = dot(luma, offsetSW);
    float lumaSE = dot(luma, offsetSE);
    float lumaM  = dot(luma, offsetNW);

    vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)),
                     ((lumaNW + lumaSW) - (lumaNE + lumaSE)));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (FXAA_REDUCE_MUL * 0.25f), FXAA_REDUCE_MIN);
    float dirCorrection = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * dirCorrection)) * screenTextureOffset;

    vec3 resultA = 0.5f * (texture(_HDRBuffer, fsUv + (dir * vec2(1.0f / 3.0f - 0.5f))).xyz +
                                    texture(_HDRBuffer, fsUv + (dir * vec2(2.0f / 3.0f - 0.5f))).xyz);

    vec3 resultB = resultA * 0.5f + 0.25f * (texture(_HDRBuffer, fsUv + (dir * vec2(0.0f / 3.0f - 0.5f))).xyz +
                                             texture(_HDRBuffer, fsUv + (dir * vec2(3.0f / 3.0f - 0.5f))).xyz);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    float lumaResultB = dot(luma, resultB);

    if(lumaResultB < lumaMin || lumaResultB > lumaMax)
        return vec3(resultA);
    else
        return vec3(resultB);
}

vec3 linearToneMapping(vec3 color)
{
	float exposure = 1.;
	color = clamp(exposure * color, 0., 1.);
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 simpleReinhardToneMapping(vec3 color)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 lumaBasedReinhardToneMapping(vec3 color)
{
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 whitePreservingLumaBasedReinhardToneMapping(vec3 color)
{
	float white = 2.;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 RomBinDaHouseToneMapping(vec3 color)
{
    color = exp( -1.0 / ( 2.72*color + 0.15 ) );
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 filmicToneMapping(vec3 color)
{
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return color;
}

vec3 Uncharted2ToneMapping(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1. / _Gamma));
	return color;
}

vec3 GTAToneMapping(vec3 color)
{
	float A = 0.22;
	float B = 0.30;
	float C = 0.10;
	float D = 0.20;
	float E = 0.01;
	float F = 0.30;
	float W = 4.0;
	float exposure = 2.0;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1.0f / _Gamma));
	return color;
}

vec3 vignette(vec3 color, vec3 color2, float a, float b) {
    float len = length(fsUv - 0.5);
    float sstep = smoothstep(a, b, len);
	return mix(color, color2, sstep);
}

vec3 tonemap_aces(vec3 color) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3(0.0), vec3(1.0));
}

vec3 toonTonemap(vec3 color){
const int levels = 8;
	return floor(color * levels) / levels; 
}

//Stephen Hill
vec3 RRTAndODTFit(vec3 v)
{
    vec3 a = v * (v + 0.0245786f) - 0.000090537f;
    vec3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

vec3 ACESFitted(vec3 color)
{
	mat3 ACESInputMat = mat3(
	    0.59719, 0.35458, 0.04823,
	    0.07600, 0.90834, 0.01566,
	    0.02840, 0.13383, 0.83777);

    color = ACESInputMat * color;

    color = RRTAndODTFit(color);

	mat3 ACESOutputMat = mat3(
     1.60475, -0.53108, -0.07367,
    -0.10208,  1.10813, -0.00605,
    -0.00327, -0.07276,  1.07602);

    color = ACESOutputMat * color;

	return clamp(color, vec3(0.0), vec3(1.0));
}

vec3 Standard(vec3 color){
	float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float mappedLuminance = (luminance * (1.0 + luminance / (1.0 * 1.0))) / (1.0 + luminance);

	
	// Scale color by ratio of average luminances.
	vec3 mappedColor = (mappedLuminance / luminance) * color;

	// Gamma correction.
	return pow(mappedColor, vec3(1.0/_Gamma));
}

void main(){
	vec3 color = texture(_HDRBuffer, fsUv).rgb * _Exposure;
	if(_FXAA) color = computeFxaa();

	if(_ApplyPostProcessing == 0) {
		out_color = vec4(color, 1.0);
		return;
	}

	vec3 toneMapped = color;

	switch(_Tonemapping){
		case 0: toneMapped = linearToneMapping(color); break;
		case 1: toneMapped = simpleReinhardToneMapping(color); break;
		case 2: toneMapped = lumaBasedReinhardToneMapping(color); break;
		case 3: toneMapped = whitePreservingLumaBasedReinhardToneMapping(color); break;
		case 4: toneMapped = RomBinDaHouseToneMapping(color); break;
		case 5: toneMapped = filmicToneMapping(color); break;
		case 6: toneMapped = Uncharted2ToneMapping(color); break;
		case 7: toneMapped = GTAToneMapping(color); break;
		case 8: toneMapped = tonemap_aces(color); break;
		case 9: toneMapped = toonTonemap(color); break;
		case 10: toneMapped = ACESFitted(color); break;
		case 11: toneMapped = Standard(color); break;
	}

	out_color = vec4(vignette(toneMapped, vec3(0), 0.3, 0.8), 1);
}