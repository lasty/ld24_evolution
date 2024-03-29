#line 2 "program.cpp"
/*  program
 * 
 *  *  OpenGL GLSL Shaders, and programs
 *
 *  Created for Ludum Dare #24
 *
 *  Created on: 25/08/2012 time2:12:13 PM
 *      Author: Lasty
 */

#include "program.h"

#include "opengl.h"

#include "image.h"
#include "primitives.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Shader::Shader(GLenum type)
: type(type)
{
	Generate();
}

Shader::~Shader()
{
	Delete();
}

void Shader::Generate()
{
	shader_id = glCreateShader(type);
	GLERR();
}

void Shader::Delete()
{
	try
	{

		glDeleteShader(shader_id);
		GLERR();
	}
	catch (GLError &e)
	{
	}
}

void Shader::LoadSource(const std::string& src)
{
	const char* srclist[1] = {src.c_str()};
	glShaderSource(shader_id, 1, srclist, nullptr);
	GLERR();
}

void Shader::Compile()
{
	glCompileShader(shader_id);
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::string errlog = GetLog();
		throw ShaderError("GLSL Compile Error", errlog, __FILE__, __LINE__);
	}
}

std::string Shader::GetLog()
{
	GLint max_len;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_len);
	GLchar log[max_len];
	GLsizei len;
	glGetShaderInfoLog(shader_id, max_len, &len, log);
	return std::string(log);
}

Program::Program()
: vs(GL_VERTEX_SHADER), fs(GL_FRAGMENT_SHADER)

{
	Generate();
}

Program::~Program()
{
	Delete();
}

void Program::Generate()
{
	program_id = glCreateProgram();
	GLERR();
}

void Program::Delete()
{
	try
	{
		glDeleteProgram(program_id);
		GLERR();
	}
	catch (GLError &e)
	{
	}
}

void Program::Use()
{
	GLERR();
	glUseProgram(program_id);
	GLERR();
	Validate();
	GLERR();
}

void Program::LoadSource(const std::string &filename)
{
	//LOGf("filename is: '%s'", filename.c_str());
	this->filename = filename;

	try
	{
		//try with #line injection (easier to debug)
		std::ifstream in( (std::string(DATA_DIR) + filename).c_str() );
		LoadSource(in, true);
	}
	catch (ShaderError &e)
	{
		//try again without the #line injection
		std::ifstream in( (std::string(DATA_DIR) + filename).c_str() );
		LoadSource(in, false);
	}
}

void Program::LoadSource(std::istream &in, bool with_lines)
{

	std::stringstream vsrc;
	std::stringstream fsrc;

	std::string line;
	const std::string token = "%%";

	int lineno = 0;

	getline(in, line);
	lineno++;

	if (with_lines) vsrc << "#line " << lineno - 1 << " \"" << filename << "\"" << "\n";

	while ((in.good() and not in.eof()) and line != token)
	{
		vsrc << line << "\n";
		getline(in, line);
		lineno++;
	}

	getline(in, line);
	//lineno++;  //Validate log is off by one, but compile log is correct

	if (with_lines) fsrc << "#line " << lineno - 1 << " \"" << filename << "\"" << "\n";

	while ((in.good() and not in.eof()) and line != token)
	{
		fsrc << line << "\n";
		getline(in, line);
		lineno++;
	}

//	LOGf("VS Shader:::\n%s", vsrc.str().c_str());

	vs.LoadSource(vsrc.str());
	vs.Compile();
	GLERR();

	fs.LoadSource(fsrc.str());
	fs.Compile();
	GLERR();

	Link();
	GLERR();

	Use();
	DiscoverAttributes();

	DiscoverUniforms();

	GLERR();
}

void Program::Link()
{
	glAttachShader(program_id, vs.shader_id);
	glAttachShader(program_id, fs.shader_id);
	glLinkProgram(program_id);

	GLint status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::string errlog = GetLog();
		throw ShaderError("GLSL Link Error", errlog, __FILE__, __LINE__);
	}
}

void Program::Validate()
{
	glValidateProgram(program_id);

	GLint status;
	glGetProgramiv(program_id, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::string errlog = GetLog();
		throw ShaderError("GLSL Validate Error", errlog, __FILE__, __LINE__);
	}
}

std::string Program::GetLog()
{
	GLint max_len;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &max_len);
	GLchar log[max_len];
	GLsizei len;
	glGetProgramInfoLog(program_id, max_len, &len, log);
	return std::string(log);
}

void Program::DiscoverAttributes()
{
	vertex_position = glGetAttribLocation(program_id, "vertex_position");
	vertex_colour = glGetAttribLocation(program_id, "vertex_colour");
	vertex_texcoords = glGetAttribLocation(program_id, "vertex_texcoords");
	GLERR();
	//ASSERT(vertex_position != -1 and vertex_colour != -1 and vertex_texcoords != -1, "GLSL Attributes Lookup failed");
	ASSERT(vertex_position != -1 and vertex_colour != -1, "GLSL Attributes Lookup failed");
}

void Program::DiscoverUniforms()
{
	projection_matrix = glGetUniformLocation(program_id, "projection_matrix");
	modelview_matrix = glGetUniformLocation(program_id, "modelview_matrix");
	texture_diffuse = glGetUniformLocation(program_id, "texture_diffuse");
	GLERR();
	//ASSERT(projection_matrix != -1 and modelview_matrix != -1 and texture_diffuse != -1, "GLSL Uniforms Lookup failed");
	ASSERT(projection_matrix != -1 and modelview_matrix != -1, "GLSL Uniforms Lookup failed");
}



void Program::SetCamera(const glm::mat4 &projection_matrix, const glm::mat4 &modelview_matrix)
{
	Use();

	SetProjectionMatrix(projection_matrix);
	SetModelViewMatrix(modelview_matrix);
}

void Program::SetProjectionMatrix(const glm::mat4 &proj)
{
	glUniformMatrix4fv(projection_matrix, 1, GL_FALSE, glm::value_ptr(proj));
	GLERR();
}

void Program::SetModelViewMatrix(const glm::mat4 &modelview)
{
	glUniformMatrix4fv(modelview_matrix, 1, GL_FALSE, glm::value_ptr(modelview));
	GLERR();
}


void Program::SetTexture(Image *img)
{
	imgref = img;
}

void Program::SetTexture(Image &img)
{
	imgref = &img;
}

void Program::UseTexture(Image *img)
{
	img->Use();
	GLERR();

	glUniform1i(texture_diffuse, 0);
	GLERR();
}

void Program::SetAttributes(const Primitive &p)
{
	p.Use();  //select in the buffer

	const auto stride = sizeof(Vertex);

	glVertexAttribPointer(vertex_position, 3, GL_FLOAT, GL_FALSE, stride, (void *) offsetof(Vertex, x));
	glEnableVertexAttribArray(vertex_position);

	glVertexAttribPointer(vertex_colour, 3, GL_FLOAT, GL_FALSE, stride, (void *) offsetof(Vertex, r));
	glEnableVertexAttribArray(vertex_colour);

	if (vertex_texcoords != -1)
	{
		glVertexAttribPointer(vertex_texcoords, 2, GL_FLOAT, GL_FALSE, stride, (void *) offsetof(Vertex, u));
		glEnableVertexAttribArray(vertex_texcoords);
	}
}

void Program::WarmUp(const Primitive &p)
{
	Use();
	if (imgref) UseTexture(imgref);

	SetAttributes(p);
}

void Program::Draw(const Primitive &p)
{
	WarmUp(p);

	p.Draw();

}


void ProgramColour::DiscoverUniforms()
{
	Program::DiscoverUniforms();

	tint_colour = glGetUniformLocation(program_id, "tint_colour");
	ASSERT(tint_colour != -1, "ProgramColour requires uniform 'tint_colour'");
}

void ProgramColour::SetDrawColour(const glm::vec4 &colour)
{
	glUniform4fv(tint_colour, 1, glm::value_ptr(colour));
	//glUniform4f(tint_colour, colour.r, colour.g, colour.b, colour.a);

}



