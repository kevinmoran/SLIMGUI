#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include "DynArray.h"
#include "Shader.h"
#include "murmur3_32.h" //apparently good hash for speed, not sure but it'll do for now

#define slIMGUI_hash_seed 42
#define slIMGUI_hash(s) murmur3_32(s, strlen(s), slIMGUI_hash_seed)

static int slIMGUI_clicked_item; //ID of the item we've clicked
static int slIMGUI_hovered_item; //ID of the item we're hovered over
static GLuint slIMGUI_vao;
static Shader slIMGUI_shader;

const vec4 button_colour_on     = vec4(0.15f, 0.25f, 0.5f, 1);
const vec4 button_colour_off    = vec4(0.15f, 0.15f, 0.15f, 1);
const vec4 window_colour        = vec4(0.1f, 0.1f, 0.1f, 1);
const vec4 window_header_colour = vec4(0.3f, 0.3f, 0.3f, 1);
const vec4 window_border_colour = vec4(0.0f, 0.0f, 0.0f, 1);
const float window_header_height= 0.1f;
const float border_thickness    = 0.005f;
const vec4 hover_modifier       = vec4(0.02f, 0.02f, 0.05f, 0);
const vec4 click_modifier       = vec4(-0.02f, -0.02f, -0.03f, 0);

bool slIMGUI_init();
bool slIMGUI_button(const char* text, float x, float y, float w, float h, bool button_state=false);
bool slIMGUI_window(const char* text, float x, float y, float w, float h);
void slIMGUI_draw_rect(float x, float y, float w, float h, vec4 colour);
static GLuint slIMGUI_load_geometry();

bool slIMGUI_init(){
    slIMGUI_clicked_item = -1;
    slIMGUI_hovered_item = -1;
    slIMGUI_vao = slIMGUI_load_geometry();

    slIMGUI_shader = load_shader("UI.vert", "uniform_colour.frag");
    glUseProgram(slIMGUI_shader.id);
	mat4 P = orthographic(-gl_aspect_ratio, gl_aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
	glUniformMatrix4fv(slIMGUI_shader.P_loc, 1, GL_FALSE, P.m);
    return true;
}

bool slIMGUI_button(const char* text, float x, float y, float w, float h, bool button_state){
    int id = slIMGUI_hash(text);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = mouse_x/gl_width; //Map to 0->1 range
    mouse_y = mouse_y/gl_height;

    bool mouse_on = (mouse_x>x && mouse_y>y && mouse_x<(x+w) && mouse_y<(y+h));
    bool is_active = (id==slIMGUI_clicked_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    bool result = button_state;
    vec4 colour = button_state ? button_colour_on : button_colour_off; 

    if(is_active){
        colour = colour + click_modifier;
        if(!mouse_clicked){
            slIMGUI_clicked_item = -1;
            result = mouse_on^button_state;
        }
    }
    else if(slIMGUI_clicked_item==-1 && slIMGUI_hovered_item==id && mouse_on && mouse_clicked){
        slIMGUI_clicked_item = id;
    }
    if(mouse_on && !mouse_clicked){
        colour = colour + hover_modifier;
        slIMGUI_hovered_item = id;
    }
    if(slIMGUI_hovered_item==id && !mouse_on) slIMGUI_hovered_item = -1;

    //Draw
    slIMGUI_draw_rect(x, y, w, h, colour);

    return result;
}

bool slIMGUI_window(const char* text, float x, float y, float w, float h){
    int id = slIMGUI_hash(text);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = mouse_x/gl_width; //Map to 0->1 range
    mouse_y = mouse_y/gl_height;

    bool mouse_on = (mouse_x>x && mouse_y>y && mouse_x<(x+w) && mouse_y<(y+window_header_height));
    bool is_active = (id==slIMGUI_clicked_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    vec4 final_colour = window_header_colour; 
    bool result = false;

    if(is_active){
        final_colour = final_colour + click_modifier;
        if(!mouse_clicked){
            slIMGUI_clicked_item = -1;
            result = mouse_on;
        }
    }
    else if(slIMGUI_clicked_item==-1 && slIMGUI_hovered_item==id && mouse_on && mouse_clicked){
        slIMGUI_clicked_item = id;
    }
    if(mouse_on && !mouse_clicked){
        final_colour = final_colour + hover_modifier;
        slIMGUI_hovered_item = id;
    }
    if(slIMGUI_hovered_item==id && !mouse_on) slIMGUI_hovered_item = -1;

    //Draw
    //Border
    slIMGUI_draw_rect(x, y, w, h, window_border_colour);
    //Header
    slIMGUI_draw_rect(x+border_thickness, y+border_thickness, w-2*border_thickness, window_header_height-2*border_thickness, final_colour);
    //Body
    slIMGUI_draw_rect(x+border_thickness, y+window_header_height, w-2*border_thickness, h-window_header_height-border_thickness, window_colour);

    return result;
}

void slIMGUI_draw_rect(float x, float y, float w, float h, vec4 colour){
    //Map pos and size to screenspace coordinates
    x = 2*gl_aspect_ratio*x - gl_aspect_ratio; //from [0->1] to [(-aspect_ratio)->aspect_ratio]
    y = 1 - 2*y; //from [1->0] to [-1->1]
    w = 2*w*gl_aspect_ratio;
    h = 2*h;

    mat4 M = scale(identity_mat4(), vec3(w,h,1));
    M = translate(M, vec3(0,-h,0));
    M = translate(M, vec3(x,y,0));
    glUseProgram(slIMGUI_shader.id);
	GLuint colour_loc = glGetUniformLocation(slIMGUI_shader.id, "colour");
	glUniform4fv(colour_loc, 1, colour.v);
	glUniformMatrix4fv(slIMGUI_shader.M_loc, 1, GL_FALSE, M.m);
    glBindVertexArray(slIMGUI_vao);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}

//-------------------------------------------------------------------------------------//
//-------------------------BEHIND THE SCENES GRAPHICS STUFF----------------------------//
//-------------------------------------------------------------------------------------//

static GLuint slIMGUI_load_geometry(){
    GLfloat points[] = {
		0, 1,
		0, 0,
		1, 0,
		0, 1,
		1, 0,
		1, 1
	};

	GLuint vao;
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    return vao;
}
