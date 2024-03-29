#version 330
in vec2 fsUV;

out vec3 outColor[2];

uniform sampler2D _GMisc;
uniform sampler2D _GAlbedo;
uniform sampler2D _GNormal;
uniform sampler2D _GPosition;

uniform float _BloomFactor;


void main(){
	vec3 albedo = texture(_GAlbedo, fsUV).xyz;
	float emission = texture(_GMisc, fsUV).z * 2;
	if(emission == 0) discard;

	outColor[0] = vec3(albedo * emission);
	outColor[1] = max(outColor[0] - _BloomFactor, 0.0f);
}

//void main()
//{
//	vec3 misc = texture(_GMisc, FSUV).xyz;
//	vec3 albedo = texture(_GAlbedo, FSUV).xyz;
//	vec3 normal = normalize(texture(_GNormal, FSUV).xyz);
//	vec3 position = texture(_GPosition, FSUV).xyz;
//	float ssao = texture(_SSAO, FSUV).x;
//
//	float specular = misc.x;
//	float lightInfluence = misc.y;
//
//	vec3 l = normalize(_Directional);
//	vec3 v = normalize(_CameraPosition - position);
//	vec3 h = normalize(l + v);
//
//	vec3 color = 
//	_Diffuse * albedo.xyz * max(0.0, dot(normal.xyz, l)) +
//	specular * pow(max(0.0, dot(h, normal)), 32.0) +
//	_Ambient * albedo.xyz;
//
//	vec3 finalColor = mix(color * _Color.rgb * (_SSAOEnabled ? ssao : 1), albedo, misc.y);
//	//vec3 finalColor = mix(color * _Color.rgb, albedo, misc.y);
//	OutColor = vec4(finalColor, 1.0);
//}
