#line 2 "app.cpp"
/*  app
 * 
 *  Created for Ludum Dare #24
 *
 *  Created on: 25/08/2012 time12:03:23 PM
 *      Author: Lasty
 */

#include "app.h"
#include "opengl.h"
#include "vertexbuffer.h"
#include "primitives.h"
#include "program.h"

#include <gtc/matrix_transform.hpp>

App::App()
:prim(vb)
{
	//vb.push_back( {0, 0, 0, 1, 1, 1, 0, 1});
	//vb.Update();

	prim.Begin(GL_QUADS);
	prim.Add( { -1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0} );
	prim.Add( { 1.0, -1.0, 0.0, 1.0, 1.0, 1.0, 4.0, 0.0} );
	prim.Add( { 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 4.0, 4.0} );
	prim.Add( { -1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 4.0} );
	prim.End();

	prog.LoadSource("basic.shader");
	i.LoadFile("tex1.png");
	//i.LoadFile("font.png");

	prog.SetTexture(i);

}

App::~App()
{

}

void App::Update(float dt)
{
	//LOGf("dt = %f", dt);
}

void App::Render()
{

	glClearColor(0.3, 0.4, 0.5, 1.0);

	glClear(GL_COLOR_BUFFER_BIT);
	GLERR();

	glm::mat4 p, m;
	prog.SetCamera(p, m);
	prog.Draw(prim);


	glm::mat4 ortho = glm::ortho(-20.0f, 20.0f, 10.0f, -10.0f);

	//font.program.SetCamera(ortho, m);

	font.Draw(ortho, m, "Hello World");

	font.Draw(ortho, -15, -3, 3, "12345678910, 11, 12");


}
