
#include "render/program.hpp"

#include "utility/io.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <exception>

Program::Program(char const *vertexSourcePath, char const *fragmentSourcePath)
{
	id = glCreateProgram();
	addShader(io::read(vertexSourcePath)->c_str(), Program::Shader::VERTEX);
	addShader(io::read(fragmentSourcePath)->c_str(), Program::Shader::FRAGMENT);
	link();
}

void Program::addShader(const char *source, Shader type)
{
	// Compile
	unsigned int shader = glCreateShader( (type==Shader::VERTEX) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER );
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	// Debug
	int  success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << infoLog << std::endl;
		throw std::exception("Shader-loading exception");
	}

	// Attach and track
	glAttachShader(id, shader);
	shaders.push_front(shader);
}

void Program::link()
{
	// Link
	glLinkProgram(id);

	// Debug
	int success;
	char infoLog[512];
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, nullptr, infoLog);
		std::cout << infoLog << std::endl;
		throw std::exception("Program-linking exception");
	}

	// Clean up
	for (unsigned int shader : shaders)
	{
		glDeleteShader(shader);
	}
	shaders.clear();
}

void Program::setUniform1i(const char *name, int value)
{
	glUseProgram(id);
	glUniform1i(glGetUniformLocation(id, name), value);
}

void Program::setUniform3fv(const char *name, glm::vec3 const &vector)
{
	glUseProgram(id);
	glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vector));
}

void Program::setUniform4f(const char *name, float x, float y, float z, float w)
{
	glUseProgram(id);
	glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Program::setUniformMatrix4fv(const char *name, glm::mat4 const &matrix)
{
	glUseProgram(id);
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Program::use()
{
	glUseProgram(id);
}

Program::~Program()
{
	glDeleteProgram(id);
}
