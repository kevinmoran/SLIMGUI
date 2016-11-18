#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
//#include "input.h"
#define check_gl_error() checkOglError(__FILE__, __LINE__)
static int checkOglError(const char *file, int line);

// void APIENTRY openglCallbackFunction(GLenum source,
//                                            GLenum type,
//                                            GLuint id,
//                                            GLenum severity,
//                                            GLsizei length,
//                                            const GLchar* message,
//                                            void* userParam){
 
//     printf("---------------------opengl-callback-start------------");;
//     printf("message: %s\n", message);
//     printf("type: ");
//     switch (type) {
//     case GL_DEBUG_TYPE_ERROR:
//         printf("ERROR");
//         break;
//     case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
//         printf("DEPRECATED_BEHAVIOR");
//         break;
//     case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
//         printf("UNDEFINED_BEHAVIOR");
//         break;
//     case GL_DEBUG_TYPE_PORTABILITY:
//         printf("PORTABILITY");
//         break;
//     case GL_DEBUG_TYPE_PERFORMANCE:
//         printf("PERFORMANCE");
//         break;
//     case GL_DEBUG_TYPE_OTHER:
//         printf("OTHER");
//         break;
//     }
//     printf("\n");
 
//     printf("id: %u", id);
//     printf("severity: ");
//     switch (severity){
//     case GL_DEBUG_SEVERITY_LOW:
//         printf("LOW");
//         break;
//     case GL_DEBUG_SEVERITY_MEDIUM:
//         printf("MEDIUM");
//         break;
//     case GL_DEBUG_SEVERITY_HIGH:
//         printf("HIGH");
//         break;
//     }
//     printf("\n");
//     printf("---------------------opengl-callback-end--------------");
// }

bool init_gl(GLFWwindow* &window, int window_width, const int window_height) {

	/* start GL context and O/S window using the GLFW helper library */
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		getchar();
		return false;
	}

	#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
	
	window = glfwCreateWindow(window_width, window_height, "slIMGUI", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		getchar();
		return false;
	}
	glfwMakeContextCurrent(window);

	//glfwSetKeyCallback(window, key_callback);

	/* start GLEW extension handler */
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);
	printf("GLSL version supported: %s\n", glsl_version);

	// glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	// glDebugMessageCallback(openglCallbackFunction, (void*)NULL);
	// GLuint unusedIds = 0;
	// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(41/255.0f, 128/255.0f, 185/255.0f, 0.0f);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	return true;
}

static int checkOglError(const char *file, int line){
    GLenum glErr;
    int retCode = 0;

    glErr = glGetError();
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d:\n%d - ", file, line, glErr);
		switch(glErr) {
			case GL_INVALID_OPERATION:				printf("INVALID_OPERATION\n");				break;
			case GL_INVALID_ENUM:					printf("INVALID_ENUM\n");					break;
			case GL_INVALID_VALUE:					printf("INVALID_VALUE\n");      			break;
			case GL_OUT_OF_MEMORY:					printf("OUT_OF_MEMORY\n");      			break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:	printf("INVALID_FRAMEBUFFER_OPERATION\n");	break;
        }
        retCode = 1;
    }
    return retCode;
}
