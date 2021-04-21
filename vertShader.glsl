#version 420

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tc;

out vec4 vcolor;
out vec2 ttc;

uniform mat4 rot;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform sampler2D img;

mat4 buildtransl(const float x,const float y,const float z){
	mat4 trans = mat4(1.0,0.0,0.0,0.0,
	0.0,1.0,0.0,0.0,
	0.0,0.0,1.0,0.0,
	x,y,z,1.0);
	
	return trans;
}

void main(void) {
	mat4 loc = buildtransl(gl_InstanceID, gl_InstanceID, gl_InstanceID);
	loc *= model;
	gl_Position = proj*view*model*vec4(pos,1.0f);
	//gl_Position = vec4(pos,1.0f);
	vcolor = rot*vec4(pos,1.0f);
	ttc = tc;
}