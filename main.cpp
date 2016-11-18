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
	
	check_gl_error(); //check for errors after setup
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

		//Calculate UI dimensions
		static float window_x = 0.1f; static float window_y = 0.1f;
		float window_w = 0.5f; float window_h = 0.8f;
		float window_padding_x = 0.025f; float window_padding_y = 0.05f;
		int num_buttons = 2;
		float first_button_y = window_y+window_header_height+window_padding_y;
		float button_width = window_w -2*(window_padding_x+border_thickness_x);
		float button_height = (window_h-window_header_height - window_padding_y*(num_buttons+1)-border_thickness_y)/num_buttons;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//Window
		slIMGUI_window("Look! A menu!", &window_x,&window_y,window_w,window_h);

		//Buttons
		static bool toggle = false;
		toggle = slIMGUI_button("Thingamajig", window_x+window_padding_x+border_thickness_x, first_button_y,
								button_width, button_height, toggle);
		if(toggle){
			slIMGUI_text("Thingamajig Engaged!", 0.6f, 0, 0.4f, 0.1f);
		}
		const static float thing_x = 0.6f;
		static float thing_y = 1.2f;
		if(slIMGUI_button("Do the thing!", window_x+window_padding_x+border_thickness_x, 
											first_button_y+button_height+window_padding_y,
											button_width, button_height)){
			thing_y = 0.1f;
		}
		slIMGUI_text("The thing!", thing_x, thing_y, 0.4f, 0.1f);
		thing_y +=dt;

		glfwSwapBuffers(window);
	}//end main loop

	return 0;
}
