#version 330 core
out vec4 out_color;
  
in vec2 textureCoords;

uniform sampler2D hdrBuffer;
uniform int applyPostProcessing;

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

float gamma = 1.0;

vec3 linearToneMapping(vec3 color)
{
	float exposure = 1.;
	color = clamp(exposure * color, 0., 1.);
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 simpleReinhardToneMapping(vec3 color)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 lumaBasedReinhardToneMapping(vec3 color)
{
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 whitePreservingLumaBasedReinhardToneMapping(vec3 color)
{
	float white = 2.;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 RomBinDaHouseToneMapping(vec3 color)
{
    color = exp( -1.0 / ( 2.72*color + 0.15 ) );
	color = pow(color, vec3(1. / gamma));
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
	color = pow(color, vec3(1. / gamma));
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
	color = pow(color, vec3(1.0f / gamma));
	return color;
}

vec3 vignette(vec3 color, vec3 color2, float a, float b) {
    float len = length(textureCoords - 0.5);
    float sstep = smoothstep(a, b, len);
	return mix(color, color2, sstep);
}

vec3 tonemap_aces(vec3 color) {
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return color = clamp((color * (a * color + b)) / (color * (c * color + d) + e), vec3(0.0), vec3(1.0));
}

vec3 toonTonemap(vec3 color){
const int levels = 8;
	return floor(color * levels) / levels; 
}

vec3 blur(vec3 color, int amount){
	for(int i =0; i< amount; i++){
		color += texture(hdrBuffer, textureCoords + i / 100).rgb;
	}
	return color /amount;
}
void main(){
	vec3 color = texture(hdrBuffer, textureCoords).rgb;

	if(applyPostProcessing == 0) {
		out_color = vec4(color, 1.0);
		return;
	}

	vec3 toneMapped = tonemap_aces(color);

	//toneMapped = toonTonemap(toneMapped);
	out_color = vec4(vignette(toneMapped, vec3(0), 0.3, 0.8), 1);

	//out_color = (out_color * (1.0 + (out_color / (0.5f/*change this*/)))) / (1.0 + out_color);
}