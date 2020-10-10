#ifndef VIEWER_ARCROLL_H
#define VIEWER_ARCROLL_H

#include<Viewer_Interpreter/Viewer_OpenGL_Interpreter.h>
//#include<Viewer_World_Manager/Viewer_World_Manager.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct Viewer_Arcroll
{
    float* old_mouse_coord;
    //坐标轴的边
    Viewer_Something* vs;
	Viewer_Camera*mc;
    Viewer_World* vw;
    Viewer_Opengl_Interpreter* voi;
    Node* disappear_somethings;
}Viewer_Arcroll;
void Viewer_Arcroll_init(Viewer_Arcroll*);
void viewer_Arcroll_cursor_position_callback(Viewer_Intera*);
void viewer_Arcroll_mouse_button_callback(Viewer_Intera*);
void viewer_Arcroll_framebuffer_size_callback(Viewer_Intera*);
void viewer_Arcroll_key_callback(Viewer_Intera*);
void viewer_Arcroll_scroll_callback(Viewer_Intera*,double,double);
void viewer_Arcroll_drop_callback(Viewer_Intera*);

///void _Read_(Mesh*mesh,char const * filename);
#ifdef __cplusplus
}
#endif

#endif
