#version 330 core
layout (location = 0) in vec3 aPos;	  // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec3 aColor; // ��ɫ����������λ��ֵΪ 1


out vec3 ourColor; // ��Ƭ����ɫ�����һ����ɫ

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