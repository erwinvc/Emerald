#version 400

in vec3 fsPos;
in vec4 fsColor;

out vec4 geoData[4];

void main(){
	geoData[0] = vec4(0, 1, 0, 0);
	geoData[1] = fsColor;
	//geoData[2] = vec4(0, 0, 0, 1);
	//geoData[3] = vec4(0, 0, 0, ); Ignore position for SSAO and stuff
}