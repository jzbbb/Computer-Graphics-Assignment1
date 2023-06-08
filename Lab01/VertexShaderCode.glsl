#version 330 core
layout (location = 0) in vec3 aPos;	  // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aColor; // 颜色变量的属性位置值为 1


out vec3 ourColor; // 向片段着色器输出一个颜色

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 incolor;

void main(){
	gl_Position=projection * view * model *vec4(aPos, 1.0);
	if(aColor!=vec3(0.0)){
		ourColor = aColor  ;		
	}
	else{
		ourColor = incolor;
	}	
}