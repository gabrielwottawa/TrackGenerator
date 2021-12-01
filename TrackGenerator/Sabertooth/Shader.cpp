#include "Shader.h"

Shader::~Shader() {}

void Shader::UseTexture( std::string textureName )
{
	glActiveTexture( GL_TEXTURE0 + textures[textureName].GetTextureNum() );
	glBindTexture( GL_TEXTURE_2D, textures[textureName].GetTextureId() );
}

void Shader::LoadTexture( char* path, char* textureUniformName, std::string textureName )
{
	Texture tempTexture;
	tempTexture.Load( path, textureUniformName, program, textureQtd );
	textures[textureName] = tempTexture;
	textureQtd += 1;
}

void Shader::setMatrix4fv(const std::string& name, glm::mat4& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(this->program, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec3(const std::string& name, glm::vec3& vec3)
{
	glUniform3fv(glGetUniformLocation(this->program, name.c_str()), 1, glm::value_ptr(vec3));
}