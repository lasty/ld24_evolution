#ifndef _LASTY_LD24_PROGRAM_H_
#define _LASTY_LD24_PROGRAM_H_
#line 4 "program.h"

/*  program
 * 
 *  OpenGL GLSL Shaders, and programs
 *
 *  Created for Ludum Dare #24
 *
 *  Created on: 25/08/2012 time2:12:13 PM
 *      Author: Lasty
 */

#include "globals.h"
#include "opengl.h"

#include "image.h"
#include "primitives.h"

#include "glm.hpp"

#include <iostream>
#include <string>

class ShaderError : public GLError
{
public:
	std::string log;
	std::string fulllog;

	ShaderError(const char* what, const std::string &log, const char*file, int line)
	: GLError(what, file, line), log(log)
	{
		fulllog = std::string(what) + std::string("\n\n") + log;
	}

	const char* EName()
	{
		return "ShaderError";
	}

	const char* What()
	{
		return fulllog.c_str();
	}
};


class Shader
{
	friend class Program;

	GLenum type;
	GLuint shader_id;

public:
	Shader(GLenum type);
	virtual ~Shader();
	void Generate();
	void Delete();

	void LoadSource(const std::string& src);
	void Compile();
	std::string GetLog();
};

class Program
{
protected:
	GLuint program_id;
	std::string filename = "Unknown";

	Shader vs;
	Shader fs;

	//uniforms
	GLint projection_matrix;
	GLint modelview_matrix;
	GLint texture_diffuse;

	//attributes
	GLint vertex_position;
	GLint vertex_colour;
	GLint vertex_texcoords;

	Image* imgref;

public:
	Program();
	virtual ~Program();
	void Generate();
	void Delete();
	void Use();

	void LoadSource(const std::string &filename);
	void LoadSource(std::istream &in, bool with_lines);

	void Link();
	void Validate();
	std::string GetLog();

	virtual void DiscoverUniforms();
	void DiscoverAttributes();

	void SetAttributes(const Primitive &p);

	void SetCamera(const glm::mat4 &projection_matrix, const glm::mat4 &modelview_matrix);
	void SetProjectionMatrix(const glm::mat4 &projection_matrix);
	void SetModelViewMatrix(const glm::mat4 &modelview_matrix);

	void SetTexture(Image *img);
	void SetTexture(Image &img);
	void UseTexture(Image *img);


	void WarmUp(const Primitive &p);  //Set up arrays/buffers/textures but do not draw

	void Draw(const Primitive &p);
};


class ProgramColour : public Program
{
protected:
	GLint tint_colour = -1;

public:
	void DiscoverUniforms();

	void SetDrawColour(const glm::vec4 &colour);

};

#endif /* _LASTY_LD24_PROGRAM_H_ */
