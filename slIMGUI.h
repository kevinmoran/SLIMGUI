#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "DynArray.h"

static int slIMGUI_hot_item; //ID of the item we've clicked
static DynArray slIMGUI_verts;
static GLuint slIMGUI_vao, slIMGUI_vbo;

bool slIMGUI_init();
bool slIMGUI_button(const char* text, float x, float y, float w, float h);
static void add_rect_to_vbo(float x, float y, float w, float h);
void slIMGUI_draw();

bool slIMGUI_init(){
    slIMGUI_hot_item = -1;
    glGenBuffers(1, &slIMGUI_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, slIMGUI_vbo);
	glBufferData(GL_ARRAY_BUFFER, 64*sizeof(float), NULL, GL_STATIC_DRAW); //TODO

	glGenVertexArrays(1, &slIMGUI_vao);
	glBindVertexArray(slIMGUI_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, slIMGUI_vbo);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    return true;
}

bool slIMGUI_button(const char* text, float x, float y, float w, float h){
    //Add box/text to renderlist
    add_rect_to_vbo(x,y,w,h);

    int id = 1; //TODO: generate a hash for this button

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = 2*gl_aspect_ratio*mouse_x/gl_width - gl_aspect_ratio; //(-aspect_ratio)->aspect_ratio
    mouse_y = 1 - (2*mouse_y)/gl_height; //(-1)->1 (up)

    bool mouse_on = (mouse_x>x && mouse_y>y && mouse_x<(x+w) && mouse_y<(y+h));
    bool is_hot = (id==slIMGUI_hot_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if(is_hot){
        if(!mouse_clicked){
            slIMGUI_hot_item = -1;
            return mouse_on;
        }
    }
    else if(slIMGUI_hot_item==-1 && mouse_on && mouse_clicked){
        slIMGUI_hot_item = id;
    }
    return false;
}

static void add_rect_to_vbo(float x, float y, float w, float h){
    float verts[] = {
        x,   y,
        x+w, y,
        x+w, y+h,
        x,   y,
        x+w, y+h,
        x,   y+h
    };

    for(int i=0; i<12; i++){
        slIMGUI_verts.push(verts[i]);
    }
}

void slIMGUI_draw(){
    glBindBuffer(GL_ARRAY_BUFFER, slIMGUI_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, slIMGUI_verts.size*sizeof(float), slIMGUI_verts.data);
    glBindVertexArray(slIMGUI_vao);
    glDrawArrays(GL_TRIANGLES, 0, slIMGUI_verts.size);
    slIMGUI_verts.empty();
}
