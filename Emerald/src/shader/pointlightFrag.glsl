#version 400 core

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GPosition;

in vec4 fsPos;
in vec4 newPos;
in vec4 color;

out vec4 outColor;

//uniform float uLightRadius;
//uniform vec3 uLightPosition;
//uniform vec3 uLightColor;

uniform vec3 uCameraPos;
uniform float reflectivity;
uniform float shineDamper;

uniform float _Diffuse;
uniform float _Specular;

void main() {
float uLightRadius = newPos.w;
vec3 uLightPosition = newPos.xyz;
vec3 uLightColor = color.rgb;

// get screen-space position of light sphere
// (remember to do perspective division.)
  vec2 uv = (fsPos.xy / fsPos.w) * 0.5 + 0.5;

// now we can sample from the gbuffer for every fragment the light sphere covers.
  vec3 albedo = texture(_GAlbedo, uv).xyz;
  vec3 n = normalize(texture(_GNormal, uv).xyz);
  vec3 pos = texture(_GPosition, uv).xyz;

  vec3 lightToPosVector = pos.xyz - uLightPosition;
  float lightDist = length(lightToPosVector);  // position from light.
  vec3 l = -lightToPosVector / (lightDist);

// implement fake z-test. If too far from light center, then 0.
  float ztest = step(0.0, uLightRadius - lightDist);

// light attenuation.
  float d = lightDist / uLightRadius;
  float attenuation = 1.0 - d;
  vec3 v = normalize(uCameraPos - pos);
  vec3 h = normalize(l + v);

  vec3 color =
// diffuse
  _Diffuse * uLightColor * albedo.xyz * max(0.0, dot(n.xyz, l)) +
// specular
 _Specular * uLightColor * reflectivity * pow(max(0.0, dot(h, n)), shineDamper); 

// finally ztest and attenuation.
  color *= ztest * attenuation;

  outColor = vec4(color, 1.0);
}