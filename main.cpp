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

		//Calculated UI dimensions
		float panel_x = 0.1f; float panel_y = 0.1f;
		float panel_w = 0.5f; float panel_h = 0.8f;
		float panel_padding_x = 0.025f; float panel_padding_y = 0.05f;
		int num_buttons = 2;
		float first_button_y = panel_y+panel_header_height+panel_padding_y;
		float button_width = panel_w -2*(panel_padding_x+border_thickness);
		float button_height = (panel_h-panel_header_height - panel_padding_y*(num_buttons+1)-border_thickness)/num_buttons;

		//Panel
		slIMGUI_panel("Panel1", panel_x,panel_y,panel_w,panel_h);

		//Buttons
		static bool toggle = false;
		toggle = slIMGUI_button("Toggle Button", panel_x+panel_padding_x+border_thickness,
								first_button_y,
								button_width, button_height, toggle);
		if(toggle){
			printf("BUTTON1\n");
		}
		if(slIMGUI_button("Button2", panel_x+panel_padding_x+border_thickness,
							first_button_y+button_height+panel_padding_y,
							button_width,button_height)){
			printf("BUTTON2\n");
		}

		glfwSwapBuffers(window);
	}//end main loop

	return 0;
}
