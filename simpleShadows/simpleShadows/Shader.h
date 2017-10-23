#pragma once

#ifndef SHADER_H
#define SHADER_H

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif // !GLEW_STATIC

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{

public:
	unsigned int ID;

	Shader() {
	}

	Shader(const char *vertex_path, const char *fragment_path, const char *geometry_path = nullptr)
	{
		std::string vertex_code;
		std::string fragment_code;
		std::string geometry_code;
		std::ifstream vertex_shader_file;
		std::ifstream fragment_shader_file;
		std::ifstream geometry_shader_file;

		/*vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometry_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);*/

		vertex_shader_file.open(vertex_path);
		fragment_shader_file.open(fragment_path);

		if (!(vertex_shader_file.good() && vertex_shader_file.is_open()))
		{
			std::cout << "ERROR::SHADER_FILE_READ :" << "vertex_shader" << std::endl;
			throw 1;
		}

		if (!(fragment_shader_file.good() && fragment_shader_file.is_open()))
		{
			std::cout << "ERROR::SHADER_FILE_READ :" << "fragment_shader" << std::endl;
			throw 1;
		}

		std::stringstream vertex_shader_stream;
		std::stringstream fragment_shader_stream;
		vertex_shader_stream << vertex_shader_file.rdbuf();
		fragment_shader_stream << fragment_shader_file.rdbuf();
		vertex_shader_file.close();
		fragment_shader_file.close();
		vertex_code = vertex_shader_stream.str();
		fragment_code = fragment_shader_stream.str();
		//std::cout << vertex_code << std::endl;
		if (geometry_path != nullptr)
		{
			geometry_shader_file.open(geometry_path);
			if (!(geometry_shader_file.good() && geometry_shader_file.is_open()))
			{
				std::cout << "ERROR::SHADER_FILE_READ :" << "geometry_shader" << std::endl;
				throw 1;
			}
			std::stringstream geometry_shader_stream;
			geometry_shader_stream << geometry_shader_file.rdbuf();
			geometry_shader_file.close();
			geometry_code = geometry_shader_stream.str();
		}

		const char *vertex_shader_code = vertex_code.c_str();
		const char *fragment_shader_code = fragment_code.c_str();

		unsigned int vertex, fragment;
		int success;
		char info_log[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		unsigned int geometry;
		if (geometry_path != nullptr)
		{
			const char *geometry_shader_code = geometry_code.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &geometry_shader_code, nullptr);
			glCompileShader(geometry);
			check_compile_errors(geometry, "GEOMETRY");
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometry_path != nullptr)
		{
			glAttachShader(ID, geometry);
		}
		glLinkProgram(ID);
		check_compile_errors(ID, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometry_path != nullptr)
		{
			glDeleteShader(geometry);
		}

	}

	void use()
	{
		glUseProgram(ID);
	}

	void set_bool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}

	void set_int(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}

	void set_float(const std::string &name, float value) const
	{
		std::cout <<  " float  "+ name + " : ";
		std::cout << glGetUniformLocation(ID, name.c_str()) << std::endl;
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

	void set_vec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void set_vec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}

	void set_vec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void set_vec3(const std::string &name, float x, float y, float z) const
	{
		std::cout << name + " : ";
		std::cout << glGetUniformLocation(ID, name.c_str()) << std::endl;
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}

	void set_vec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void set_vec4(const std::string &name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}

	void set_mat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void set_mat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void set_mat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:

	void check_compile_errors(GLuint shader, std::string type)
	{
		GLint success;
		GLchar info_log[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log <<
					"\n--------------------------------------------------------------------------" << std::endl;
			}
		}
		else
		{
			glGetShaderiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log <<
					"\n--------------------------------------------------------------------------" << std::endl;
			}
		}
	}
};

#endif