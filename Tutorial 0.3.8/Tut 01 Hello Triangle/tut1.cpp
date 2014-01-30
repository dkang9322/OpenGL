//Copyright (C) 2010-2012 by Jason L. McKesson
//This file is licensed under the MIT License.



#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <glload/gl_3_2_comp.h>
#include <GL/freeglut.h>


GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch(eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for(size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint theProgram;

const std::string strVertexShader(
	"#version 330\n"
	"layout(location = 0) in vec4 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = position;\n"
	"}\n"
);

const std::string strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
	"}\n"
);

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

// This is the Clip-Space Coordinates (Normalized)
const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;


void InitializeVertexBuffer(){
  // Creates the Buffer Object, note pointer return
  glGenBuffers( 1, &positionBufferObject );

  // Binding and then setting of data, target (elt of Context)
  // is GL_ARRAY_BUFFER
  glBindBuffer( GL_ARRAY_BUFFER, positionBufferObject );

  // Allocates memory for the buffer bound to GL_ARRAY_BUFFER, size of
  // memory (in GPU) specified by sizeof()
  // Pt.2: Copying Data from vertexPositions in to a Buffer OBject
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW );
  // A Clean-up: remember 0 is the de-binding number? (or a nullptr
  // for OpenGL state machine)
  glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init(){
  InitializeProgram();// MAke Shader
  InitializeVertexBuffer();

  glGenVertexArrays( 1, &vao );
  glBindVertexArray( vao );
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display(){
  
  // Recall Bindings set state i
  // And function calls i_j
  // GL_Context StateMachine i_j setters
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glClear( GL_COLOR_BUFFER_BIT );
  // Clear Screen into Color set by glClearColor
  // GL_COLOR_BUFFER_BIT is the color buffer

  // Some Shader Business
  glUseProgram( theProgram );

  // Set States: i.e. set I by binding
  // Related to Memory, Buffer Object in control! and referenced
  // by GLuint pointer positionBufferObject
  
  glBindBuffer( GL_ARRAY_BUFFER, positionBufferObject );
  // Pass To Shader Array
  glEnableVertexAttribArray( 0 );
  // Deals with buffer objects, currently bound to a target in OpenGL
  // 3rd: BASE_TYPE of value (32 bit float)
  // 2nd: how many of values of BASE_TYPE represent a vertex
  // 5th: spacing between each set of vales in 2nd
  // 6th: byte offset from value in buffer objects(where to start)
  glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, 0 );
  // specifies how to pass data to shader

  // Above Code MUST precede rendering function

  // A Rendering Function
  // Generates a stream of vertices, on object bound to a target in OpenGL
  glDrawArrays( GL_TRIANGLES, 0, 3 );
  //2nd&3rd: start index&number of indices(=vertex) from bound object to read
  // 1st: tells 3 vertices cutt scheme

  glDisableVertexAttribArray( 0 );
  glUseProgram( 0 );

  // A FreeGLUT, an initialization system, command
  // switch currently shown Buffer with Rendered Buffer
  glutSwapBuffers();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape (int w, int h){
  	glViewport( 0, 0, (GLsizei) w, (GLsizei) h );
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:
		  glutLeaveMainLoop();
		  return;
	}
}


unsigned int defaults(unsigned int displayMode, int &width, int &height) {return displayMode;}
