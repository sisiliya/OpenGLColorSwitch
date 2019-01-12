#version 330							
layout (location = 0) in vec3 pos;							
						
out vec4 vCol;													
														
														
uniform mat4 model;											
uniform mat4 projection;
uniform mat4 view;

uniform vec3 objColor;
														   
void main()													
{															
	gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);   
	vCol = vec4(objColor, 1.0f);
}										 				