#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include "DynArray.h"
#include "maths_funcs.h"
#include "Shader.h"
#include "load_texture.h"
#include "murmur3_32.h" //apparently good hash for speed, not sure but it'll do for now

#define slIMGUI_hash_seed 42
#define slIMGUI_hash(s) murmur3_32(s, strlen(s), slIMGUI_hash_seed)

static int slIMGUI_clicked_item; //ID of the item we've clicked
static int slIMGUI_hovered_item; //ID of the item we're hovered over
static GLuint slIMGUI_vao, text_vbo, text_vao, font_texture;
static Shader slIMGUI_shader, font_shader;

const vec4 button_colour_on     = vec4(155/255.0f, 89/255.0f, 182/255.0f, 0.8f);
const vec4 button_colour_off    = vec4(90/255.0f, 40/255.0f, 110/255.0f, 0.8f);
const vec4 window_colour        = vec4(80/255.0f, 40/255.0f, 80/255.0f, 0.8f);
const vec4 window_header_colour = vec4(142/255.0f, 68/255.0f, 173/255.0f, 0.8f);
const vec4 window_border_colour = vec4(0,0,0,1);
const float window_header_height= 0.1f;
const float border_thickness_y  = 0.005f;
const float border_thickness_x  = border_thickness_y/gl_aspect_ratio;
const vec4 hover_modifier       = vec4(0.02f, 0.02f, 0.05f, 0);
const vec4 click_modifier       = vec4(-0.02f, -0.02f, -0.03f, 0);

bool slIMGUI_init();
bool slIMGUI_button(const char* text, float x, float y, float w, float h, bool button_state=false);
bool slIMGUI_window(const char* text, float x, float y, float w, float h);
bool slIMGUI_window(const char* text, float* x, float* y, float w, float h); //movable
void slIMGUI_draw_rect(float x, float y, float w, float h, vec4 colour);
static GLuint slIMGUI_load_geometry();
static void slIMGUI_init_text();
void slIMGUI_text(const char* text, float x, float y, float w, float h);

bool slIMGUI_init(){
    slIMGUI_clicked_item = -1;
    slIMGUI_hovered_item = -1;
    slIMGUI_vao = slIMGUI_load_geometry();
    slIMGUI_init_text();

    slIMGUI_shader = load_shader("UI.vert", "uniform_colour.frag");
	mat4 P = orthographic(-gl_aspect_ratio, gl_aspect_ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    glUseProgram(slIMGUI_shader.id);
    glUniformMatrix4fv(slIMGUI_shader.P_loc, 1, GL_FALSE, P.m);
    glUseProgram(font_shader.id);
    glUniformMatrix4fv(font_shader.P_loc, 1, GL_FALSE, P.m);
    glUniformMatrix4fv(font_shader.M_loc, 1, GL_FALSE, identity_mat4().m);
    return true;
}

bool slIMGUI_button(const char* text, float x, float y, float w, float h, bool button_state){
    int id = slIMGUI_hash(text);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = mouse_x/gl_width; //Map to 0->1 range
    mouse_y = mouse_y/gl_height;

    bool mouse_on = (mouse_x>x && mouse_y>y && mouse_x<(x+w) && mouse_y<(y+h));
    bool is_clicked = (id==slIMGUI_clicked_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    bool result = button_state;
    vec4 colour = button_state ? button_colour_on : button_colour_off; 

    if(is_clicked){
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

    slIMGUI_text(text, x, y, w, h);

    return result;
}

bool slIMGUI_window(const char* text, float x, float y, float w, float h){
    int id = slIMGUI_hash(text);

    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = mouse_x/gl_width; //Map to 0->1 range
    mouse_y = mouse_y/gl_height;

    bool mouse_on = (mouse_x>x && mouse_y>y && mouse_x<(x+w) && mouse_y<(y+window_header_height));
    bool is_clicked = (id==slIMGUI_clicked_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    vec4 final_colour = window_header_colour; 
    bool result = false;

    if(is_clicked){
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
    slIMGUI_draw_rect(x+border_thickness_x, y+border_thickness_y, w-2*border_thickness_x, window_header_height-2*border_thickness_y, final_colour);
    //Body
    slIMGUI_draw_rect(x+border_thickness_x, y+window_header_height, w-2*border_thickness_x, h-window_header_height-border_thickness_y, window_colour);

    slIMGUI_text(text, x, y+border_thickness_y, w, window_header_height-2*border_thickness_y);

    return result;
}

//Draggable window
bool slIMGUI_window(const char* text, float* x, float* y, float w, float h){
    int id = slIMGUI_hash(text);

    static double mouse_x, mouse_y;
    static double prev_mouse_x = mouse_x;
    static double prev_mouse_y = mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y); //0->width, 0->height (down)
    mouse_x = mouse_x/gl_width; //Map to 0->1 range
    mouse_y = mouse_y/gl_height;

    bool mouse_on = (mouse_x>*x && mouse_y>*y && mouse_x<(*x+w) && mouse_y<(*y+window_header_height));
    bool is_clicked = (id==slIMGUI_clicked_item);
    bool mouse_clicked = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    vec4 final_colour = window_header_colour; 
    bool result = false;

    if(is_clicked){
        final_colour = final_colour + click_modifier;
        if(!mouse_clicked){
            slIMGUI_clicked_item = -1;
            result = mouse_on;
        }
        float dx = mouse_x-prev_mouse_x;
        float dy = mouse_y-prev_mouse_y;
        *x += dx;
        *y += dy;
        *x = MIN(MAX(*x,-0.95f*w),0.95f); //Don't allow windows to be dragged irretrievably off-screen
        *y = MIN(MAX(*y,-0.95f*h),0.95f);
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
    slIMGUI_draw_rect(*x, *y, w, h, window_border_colour);
    //Header
    slIMGUI_draw_rect(*x+border_thickness_x, *y+border_thickness_y, w-2*border_thickness_x, window_header_height-2*border_thickness_y, final_colour);
    //Body
    slIMGUI_draw_rect(*x+border_thickness_x, *y+window_header_height, w-2*border_thickness_x, h-window_header_height-border_thickness_y, window_colour);

    slIMGUI_text(text, *x, *y+border_thickness_y, w, window_header_height-2*border_thickness_y);

    prev_mouse_x = mouse_x;
    prev_mouse_y = mouse_y;

    return result;
}

void slIMGUI_draw_rect(float x, float y, float w, float h, vec4 colour){
    //Map pos and size to screenspace coordinates
    x = 2*gl_aspect_ratio*x - gl_aspect_ratio; //from [0->1] to [(-aspect_ratio)->aspect_ratio]
    y = 1 - 2*y; //from [1->0] to [-1->1]
    w = 2*gl_aspect_ratio*w;
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

//------------------------------------------//
// Font stuff
//------------------------------------------//
#define SLIMGUI_MAX_NUM_CHARS 32

struct Glyph {
  int code_point, x, y, w, h, origin_x, origin_y, advance;
};

struct Font {
  const char *name;
  int size, width, height, glyph_count;
  Glyph *glyphs;
};

static Glyph glyphs_Optima[] = {
  {' ', 563, 210, 3, 3, 1, 1, 23},
  {'!', 235, 86, 14, 62, -7, 60, 28},
  {'"', 320, 210, 21, 27, -3, 60, 28},
  {'#', 420, 149, 38, 58, -4, 57, 46},
  {'$', 95, 0, 38, 75, -4, 64, 46},
  {'%', 597, 0, 71, 64, -1, 60, 74},
  {'&', 514, 86, 54, 61, -4, 58, 60},
  {'\'', 359, 210, 9, 27, -7, 60, 23},
  {'(', 155, 0, 19, 72, -3, 60, 23},
  {')', 174, 0, 19, 72, -1, 60, 23},
  {'*', 287, 210, 33, 33, -2, 59, 37},
  {'+', 650, 149, 46, 45, -2, 44, 50},
  {',', 341, 210, 18, 27, 0, 11, 23},
  {'-', 492, 210, 25, 9, -1, 24, 28},
  {'.', 479, 210, 13, 12, -5, 10, 23},
  {'/', 0, 86, 31, 63, 4, 60, 23},
  {'0', 857, 86, 43, 61, -2, 58, 46},
  {'1', 261, 149, 23, 61, -6, 59, 46},
  {'2', 297, 149, 39, 60, 0, 58, 46},
  {'3', 120, 149, 37, 61, -2, 58, 46},
  {'4', 336, 149, 44, 59, 0, 57, 46},
  {'5', 82, 149, 38, 61, -2, 58, 46},
  {'6', 942, 86, 41, 61, -3, 58, 46},
  {'7', 380, 149, 40, 59, -4, 57, 46},
  {'8', 41, 149, 41, 61, -3, 58, 46},
  {'9', 983, 86, 41, 61, -2, 58, 46},
  {':', 226, 210, 13, 43, -5, 41, 23},
  {';', 458, 149, 18, 57, 0, 41, 23},
  {'<', 558, 149, 46, 47, -2, 45, 50},
  {'=', 368, 210, 46, 24, -2, 33, 50},
  {'>', 604, 149, 46, 47, -2, 45, 50},
  {'?', 210, 86, 25, 62, -5, 60, 32},
  {'@', 476, 149, 56, 55, -5, 56, 66},
  {'A', 101, 86, 56, 62, 0, 60, 55},
  {'B', 900, 86, 42, 61, -5, 59, 51},
  {'C', 834, 0, 54, 63, -1, 60, 55},
  {'D', 347, 86, 57, 61, -5, 59, 65},
  {'E', 227, 149, 34, 61, -5, 59, 42},
  {'F', 192, 149, 35, 61, -5, 59, 42},
  {'G', 776, 0, 58, 63, -3, 60, 65},
  {'H', 460, 86, 54, 61, -5, 59, 65},
  {'I', 284, 149, 13, 61, -5, 59, 23},
  {'J', 133, 0, 22, 72, 4, 59, 23},
  {'K', 718, 86, 47, 61, -5, 59, 51},
  {'L', 157, 149, 35, 61, -5, 59, 42},
  {'M', 31, 86, 70, 62, -2, 60, 74},
  {'N', 888, 0, 53, 63, -6, 60, 65},
  {'O', 709, 0, 67, 63, -1, 60, 69},
  {'P', 0, 149, 41, 61, -5, 59, 46},
  {'Q', 28, 0, 67, 79, -1, 60, 69},
  {'R', 765, 86, 46, 61, -5, 59, 51},
  {'S', 941, 0, 38, 63, -2, 60, 42},
  {'T', 811, 86, 46, 61, 0, 59, 46},
  {'U', 157, 86, 53, 62, -6, 59, 65},
  {'V', 404, 86, 56, 61, 0, 59, 55},
  {'W', 262, 86, 85, 61, 1, 59, 83},
  {'X', 670, 86, 48, 61, -1, 59, 51},
  {'Y', 568, 86, 51, 61, 0, 59, 51},
  {'Z', 619, 86, 51, 61, 0, 59, 51},
  {'[', 255, 0, 17, 70, -9, 59, 28},
  {'\\', 979, 0, 31, 63, -5, 60, 42},
  {']', 237, 0, 18, 70, -1, 59, 28},
  {'^', 239, 210, 48, 41, -1, 56, 50},
  {'_', 517, 210, 46, 8, 2, -4, 42},
  {'`', 414, 210, 23, 18, 1, 63, 28},
  {'a', 801, 149, 40, 44, -1, 42, 42},
  {'b', 300, 0, 42, 66, -3, 64, 46},
  {'c', 881, 149, 39, 44, -1, 42, 42},
  {'d', 342, 0, 42, 66, -1, 64, 46},
  {'e', 841, 149, 40, 44, -1, 42, 42},
  {'f', 272, 0, 28, 67, 1, 65, 23},
  {'g', 384, 0, 41, 66, -1, 42, 42},
  {'h', 464, 0, 38, 66, -4, 64, 46},
  {'i', 249, 86, 13, 62, -5, 60, 23},
  {'j', 8, 0, 20, 84, 2, 60, 23},
  {'k', 425, 0, 39, 66, -4, 64, 42},
  {'l', 502, 0, 11, 66, -6, 64, 23},
  {'m', 696, 149, 61, 44, -4, 42, 69},
  {'n', 920, 149, 38, 44, -4, 42, 46},
  {'o', 757, 149, 44, 44, -1, 42, 46},
  {'p', 555, 0, 42, 65, -3, 42, 46},
  {'q', 513, 0, 42, 65, -1, 42, 46},
  {'r', 988, 149, 24, 44, -4, 42, 28},
  {'s', 958, 149, 30, 44, -1, 42, 32},
  {'t', 532, 149, 26, 54, 1, 52, 23},
  {'u', 188, 210, 38, 43, -4, 41, 46},
  {'v', 65, 210, 42, 43, 0, 41, 42},
  {'w', 0, 210, 65, 43, 0, 41, 65},
  {'x', 107, 210, 41, 43, 0, 41, 42},
  {'y', 668, 0, 41, 64, 0, 41, 42},
  {'z', 148, 210, 40, 43, -1, 41, 42},
  {'{', 193, 0, 22, 70, -3, 59, 28},
  {'|', 0, 0, 8, 86, -10, 64, 28},
  {'}', 215, 0, 22, 70, -3, 59, 28},
  {'~', 437, 210, 42, 15, -4, 29, 50},
};

static Font font_Optima = {"Optima", 83, 1024, 256, 95, glyphs_Optima};

static void slIMGUI_init_text(){
    //Allocate a vbo with space for SLIMGUI_MAX_NUM_CHARS chars of text geometry
    //We'll fill it with data on the fly
	glGenBuffers(1, &text_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
	glBufferData(GL_ARRAY_BUFFER, SLIMGUI_MAX_NUM_CHARS*6*4*sizeof(float), NULL, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &text_vao);
	glBindVertexArray(text_vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (GLvoid*)(2*sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	load_texture("font.png", &font_texture);
	font_shader = load_shader("Textured.vert", "Textured.frag");
}

void slIMGUI_text(const char* text, float x, float y, float w, float h){
    //Map pos and size to screenspace coordinates
    x = 2*gl_aspect_ratio*x - gl_aspect_ratio; //from [0->1] to [(-aspect_ratio)->aspect_ratio]
    y = 1 - 2*y; //from [1->0] to [-1->1]
    w = 2*gl_aspect_ratio*w;
    h = 2*h;

    float scale = 0.8f*h/((float)font_Optima.size/gl_height); //0.8 is just to add some vert padding
    
    //Calculate width of text
    float text_width = 0.0f;
    const char* temp = text;
    while(*temp){
        int i = (int)(*temp) - glyphs_Optima[0].code_point;
        text_width += scale*(float)glyphs_Optima[i].advance/gl_width;
        temp++;
    }
    //text_width *= 2*gl_aspect_ratio; //convert to screenspace coords //why is this not needed?

    if(text_width>w){
        scale*= 0.9f*w/text_width;
        text_width = 0.9f*w;
    }

    float cursor_pos = x + (w-text_width)/2;
    int num_glyphs = 0;
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);

    float vert_data[SLIMGUI_MAX_NUM_CHARS*6*4];
    while(*text){
        int i = (int)(*text) - glyphs_Optima[0].code_point;
        Glyph glyph = glyphs_Optima[i];

        //Calculate data for quad
        float x_l = cursor_pos - (scale*(float)glyph.origin_x / gl_width);
        float x_r = cursor_pos + (scale*(float)(glyph.w-glyph.origin_x) / gl_width);
        float y_t = y - 0.75f*h + (scale*(float)glyph.origin_y / gl_height); //0.75=magic, just works
        float y_b = y - 0.75f*h + (scale*((float)glyph.origin_y-glyph.h) / gl_height); //0.75=magic, just works
        float u_l = (float)glyph.x / font_Optima.width;
        float u_r = (float)(glyph.x+glyph.w) / font_Optima.width;
        float v_t = 1 - ((float)glyph.y / font_Optima.height);
        float v_b = 1 - ((float)(glyph.y+glyph.h) / font_Optima.height);

        //Add triangles to array:
        //topleft
        vert_data[24*num_glyphs   ] = x_l;
        vert_data[24*num_glyphs+ 1] = y_t;
        vert_data[24*num_glyphs+ 2] = u_l;
        vert_data[24*num_glyphs+ 3] = v_t;
        //bottomleft
        vert_data[24*num_glyphs+ 4] = x_l;
        vert_data[24*num_glyphs+ 5] = y_b;
        vert_data[24*num_glyphs+ 6] = u_l;
        vert_data[24*num_glyphs+ 7] = v_b;
        //bottomright
        vert_data[24*num_glyphs+ 8] = x_r;
        vert_data[24*num_glyphs+ 9] = y_b;
        vert_data[24*num_glyphs+10] = u_r;
        vert_data[24*num_glyphs+11] = v_b;
        //topleft
        vert_data[24*num_glyphs+12] = x_l;
        vert_data[24*num_glyphs+13] = y_t;
        vert_data[24*num_glyphs+14] = u_l;
        vert_data[24*num_glyphs+15] = v_t;
        //bottomright
        vert_data[24*num_glyphs+16] = x_r;
        vert_data[24*num_glyphs+17] = y_b;
        vert_data[24*num_glyphs+18] = u_r;
        vert_data[24*num_glyphs+19] = v_b;
        //topright
        vert_data[24*num_glyphs+20] = x_r;
        vert_data[24*num_glyphs+21] = y_t;
        vert_data[24*num_glyphs+22] = u_r;
        vert_data[24*num_glyphs+23] = v_t;

        cursor_pos+= scale*(float)glyph.advance/gl_width;
        text++;
        num_glyphs++;
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, num_glyphs*24*sizeof(float), vert_data);

    glUseProgram(font_shader.id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, font_texture);
    glBindVertexArray(text_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6*num_glyphs);
}
