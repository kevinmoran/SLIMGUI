#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

GLFWwindow* window = NULL;
int gl_width = 360;
int gl_height = 240;
float gl_aspect_ratio = (float)gl_width/gl_height;

#include "init_gl.h"
#include "maths_funcs.h"
#include "Shader.h"
#include "DynArray.h"
#include "slIMGUI.h"

int main() {
	if (!init_gl(window, gl_width, gl_height)){ return 1; }

	slIMGUI_init();

	//Load shader
	Shader pass_shader("pass.vert", "pass.frag");
	glUseProgram(pass_shader.prog_id);
	GLuint colour_loc = glGetUniformLocation(pass_shader.prog_id, "quad_colour");
	glUniform4fv(colour_loc, 1, vec4(0.7f,0,0,1).v);
	mat4 P = orthographic(-gl_aspect_ratio, gl_aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
	glUniformMatrix4fv(pass_shader.P_loc, 1, GL_FALSE, P.m);

	double curr_time = glfwGetTime(), prev_time, dt;
	//-------------------------------------------------------------------------------------//
	//-------------------------------------MAIN LOOP---------------------------------------//
	//-------------------------------------------------------------------------------------//
	while (!glfwWindowShouldClose(window)) {
		//Get dt
		prev_time = curr_time;
		curr_time = glfwGetTime();
		dt = curr_time - prev_time;
		if (dt > 0.1) dt = 0.1;

		// static float fps_timer = 0.0f;
		// fps_timer+=dt;
		// if(fps_timer>0.2f){
		// 	char title_string[64];
		// 	sprintf(title_string, "slIMGUI - %.2fHz", 1/dt);
		// 	glfwSetWindowTitle(window, title_string);
		// 	fps_timer = 0.0f;
		// }

		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
			continue;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(pass_shader.prog_id);
		if(slIMGUI_button("Button", -0.5f,-0.25f,1,0.5f)){
			static bool toggle_button = false;
			if(toggle_button) glUniform4fv(colour_loc, 1, vec4(0.7f,0,0,1).v);
			else glUniform4fv(colour_loc, 1, vec4(0,0.7,0,1).v);
			toggle_button = !toggle_button;
		}
		slIMGUI_draw();

		glfwSwapBuffers(window);
	}//end main loop

	return 0;
}
