/*****************************
 * Computer Graphics PROJECT 1
 * Author: Naren Sundaravaradan
 * Date: 21-FEB-08
 *****************************/
#include <vector>
#include <iostream>
#include "stdio.h"
#include <GL/gl.h>
#include <GL/glut.h>
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define WINDOW_POSITION_X 100
#define WINDOW_POSITION_Y 100

using namespace std;

enum MODES {
	KEYFRAME_MODE,
	ANIMATE_MODE
} cur_mode;

enum EDIT_MENU {
	NEW_OBJECT,
	DELETE_OBJECT,
	PLAY_ANIMATION,
	KEY_FRAME_CHANGE,
	ADD_KEYFRAME,
	ROTATE_P,
	ROTATE_N,
	SCALE_X_P,
	SCALE_X_N,
	SCALE_Y_P,
	SCALE_Y_N,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
	COLOR_BLACK,
	INC_TIME,
	DEC_TIME
};

struct square {
	int x;
	int y;
	int rotate;
	float scalex;
	float scaley;
	float color_r;
	float color_g;
	float color_b;
}; 

/*global vars*/
int mainwindow, editwindow, infowindow;
int x_edit_r_down, y_edit_r_down;
vector<vector<square>*> objects;
int time_per_frame;
bool leftdown;
square* selected_sq;
int cur_frame;
int time_elapsed;
bool finish_animation;
int max_keyframes;
int total_time;

const int frames_per_second = 60;

void writeString(float x, float y, void* font, const char* string)
{
	glRasterPos2f(x, y);
	for (const char* c=string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}
}

void display_main()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glutSwapBuffers();
}

void drawSquare(float r, float g, float b)
{
	glColor3f(r, g, b);
	glBegin(GL_POLYGON);
		glVertex2i(-10, -10);
		glVertex2i(-10, 10);
		glVertex2i(10, 10);
		glVertex2i(10, -10);
	glEnd();
}

void drawGreySquare()
{
	glColor3f(0.5, 0.5, 0.5);
	glLineWidth(4);
	glBegin(GL_LINE_LOOP);
		glVertex2i(-11, -11);
		glVertex2i(-11, 11);
		glVertex2i(11, 11);
		glVertex2i(11, -11);
	glEnd();
}

void drawKeyFrame(int frameno)
{
	if (cur_mode == KEYFRAME_MODE)
	{
		if (selected_sq)
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			square* t = selected_sq;
			glTranslatef(t->x, t->y, 0);
			glRotatef(t->rotate, 0, 0, 1.0);
			glScalef(t->scalex, t->scaley, 0);
			drawGreySquare();
			glPopMatrix();
		}
		if (frameno < objects.size())
		{
			for (int i=0; i<objects[frameno]->size(); i++)
			{
				glMatrixMode(GL_MODELVIEW);
				square* t = &(*objects[frameno])[i];
				if (t-> x >= 0)
				{
					glPushMatrix();
					glTranslatef(t->x, t->y, 0);
					glRotatef(t->rotate, 0, 0, 1.0);
					glScalef(t->scalex, t->scaley, 0);
					drawSquare(t->color_r, t->color_g, t->color_b);
					glPopMatrix();
				}
			}
		}
	}
	else //ANIMATE MODE
	{
		for (int i=0; i<objects[frameno]->size(); i++)
		{
			glMatrixMode(GL_MODELVIEW);
			square* t1 = &(*objects[frameno])[i];
			square* t2 = &(*objects[frameno+1])[i];
			if (t1->x >= 0)
			{
				glPushMatrix();
				float ratio = (time_elapsed % time_per_frame)/
							(float)time_per_frame;
				float tx = (t1->x - 10) + ((t2->x - 10) - 
							(t1->x - 10))*ratio;
				float ty = (t1->y - 10) + ((t2->y - 10) - 
							(t1->y - 10))*ratio;
				glTranslatef(tx, ty, 0);
				glRotatef(t1->rotate + (t2->rotate - 
						t1->rotate)*ratio, 0, 0, 1.0);
				glScalef(t1->scalex + (t2->scalex - t1->scalex)*ratio, 
					t1->scaley + (t2->scaley - t1->scaley)*ratio, 0);
				drawSquare(t1->color_r + (t2->color_r - t1->color_r)*ratio, 
						t1->color_g + (t2->color_g - t1->color_g)*ratio, 
						t1->color_b + (t2->color_b - t1->color_b)*ratio);
				glPopMatrix();
			}
		}
	}
}

void display_edit()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (cur_mode == ANIMATE_MODE)
	{
		int next_frame = time_elapsed % time_per_frame == 0 ? ++cur_frame : cur_frame;
		if (next_frame < objects.size() - 1)
		{
			drawKeyFrame(next_frame);
		}
		else
		{
			finish_animation = true;
		}
	}
	else if (cur_mode == KEYFRAME_MODE)
		drawKeyFrame(cur_frame);

	glutSwapBuffers();
}

void myKeyboardCB(unsigned char key, int x, int y)
{
	if (selected_sq)
	{
		switch (key)
		{
			case 'j': //scale-Y +5
				selected_sq->scaley += 0.1;
				break;
			case 'k': //scale-Y -5
				selected_sq->scaley -= (selected_sq->scaley >= 
								0.1 ? 0.1 : 0);
				break;
			case 'h': //scale-X -5
				selected_sq->scalex -= (selected_sq->scalex >= 
								0.1 ? 0.1 : 0);
				break;
			case 'l': //scale-X +5
				selected_sq->scalex += 0.1;
				break;
			case 'n': //rotate +5
				selected_sq->rotate = (selected_sq->rotate + 5) % 360;
				break;
			case 'b': //rotate -5
				selected_sq->rotate = (selected_sq->rotate - 5) % 360;
				break;
		}
		glutPostRedisplay();
	}
}

void display_info()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	char buffer[50];
	sprintf(buffer, "Editing Objects in Keyframe: %d", cur_frame);
	writeString(10, 150, GLUT_BITMAP_HELVETICA_18, buffer);

	sprintf(buffer, "Total Animation Time: %d seconds", total_time);
	writeString(10, 120, GLUT_BITMAP_HELVETICA_18, buffer);

	if (cur_mode == ANIMATE_MODE)
	{
		sprintf(buffer, "Elapsed Time: %d.%d seconds", 
			time_elapsed/100, time_elapsed % 100);	
	}
	else
	{
		sprintf(buffer, "Elapsed Time: %d.%d seconds", 
			cur_frame == 0 ? 0 : total_time/objects.size(), 
			(total_time*100/objects.size()) % 100);
	}
	writeString(10, 90, GLUT_BITMAP_HELVETICA_18, buffer);
	writeString(700, 170, GLUT_BITMAP_HELVETICA_12, "j - Scale Y +");
	writeString(700, 150, GLUT_BITMAP_HELVETICA_12, "k - Scale Y -");
	writeString(700, 130, GLUT_BITMAP_HELVETICA_12, "h - Scale X -");
	writeString(700, 110, GLUT_BITMAP_HELVETICA_12, "l - Scale X +");
	writeString(700, 90, GLUT_BITMAP_HELVETICA_12, "b - Rotate +");
	writeString(700, 70, GLUT_BITMAP_HELVETICA_12, "n - Rotate -");
	glutSwapBuffers();
}

void timer(int value)
{
	time_elapsed = value;
	if (!finish_animation)
	{
		glutPostRedisplay();
		glutSetWindow(infowindow);
		glutPostRedisplay();
		glutSetWindow(editwindow);
		glutTimerFunc(1000/100, timer, ++value);
	}
	else
	{
		cur_mode = KEYFRAME_MODE;
		cur_frame = 0;
		glutPostRedisplay();
		glutSetWindow(infowindow);
		glutPostRedisplay();
		glutSetWindow(editwindow);
	}
}

void edit_mouse_motion(int x, int y)
{
	if (leftdown)
	{
		int ry = 3*WINDOW_HEIGHT/4 - y;
		selected_sq->x = x;
		selected_sq->y = ry;
		glutPostRedisplay();
	}
}

void edit_passive_motion(int x, int y)
{
	x_edit_r_down = x;
	y_edit_r_down = 3*WINDOW_HEIGHT/4 - y;
}

inline bool contains(const square* sq, int x, int y)
{
	if (x >= sq->x - 10*sq->scalex && 
		x <= sq->x + 10*sq->scalex && 
		sq->y >= y - 10*sq->scaley && 
		sq->y <= y + 10*sq->scaley)
		return true;
	return false;
}

square* find_object_under(int x, int y)
{
	for (int i=0; cur_frame < objects.size() && 
			i<objects[cur_frame]->size(); i++)
	{
		if (contains(&(*objects[cur_frame])[i], x, y))
		{
			return &(*objects[cur_frame])[i];
		}
	}
	return 0;
}

void edit_mouse_callback(int button, int state, int x, int y)
{
	int ry = 3*WINDOW_HEIGHT/4 - y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if ( (selected_sq = find_object_under(x, ry)) )
		{
			leftdown = true;
			glutPostRedisplay();
		}
		else
		{
			selected_sq = NULL;
			glutPostRedisplay();
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		leftdown = false;
	}
}

void trans_menu(int option)
{
	if (selected_sq)
	{
		switch (option)
		{
		case ROTATE_P:
			selected_sq->rotate = (selected_sq->rotate + 5) % 360;
			break;
		case ROTATE_N:
			selected_sq->rotate = (selected_sq->rotate - 5) % 360;
			break;
		case SCALE_X_P:
			selected_sq->scalex += 0.1;
			break;
		case SCALE_X_N:
			selected_sq->scalex -= (selected_sq->scalex >= 0.1 ? 0.1 : 0);
			break;
		case SCALE_Y_P:
			selected_sq->scaley += 0.1;
			break;
		case SCALE_Y_N:
			selected_sq->scaley -= (selected_sq->scaley >= 0.1 ? 0.1 : 0);
			break;
		}
		glutPostRedisplay();
	}
}

void edit_menu_callback(int option)
{
	if (cur_mode == KEYFRAME_MODE && option == NEW_OBJECT)
	{
		/*create the frame if no objects in frame*/
		if (cur_frame >= objects.size())
		{
			objects.push_back(new vector<square>());
		}
		square s;
		s.x = x_edit_r_down;
		s.y = y_edit_r_down;
		s.rotate = 0;
		s.scalex = 1.0;
		s.scaley = 1.0;
		s.color_r= 0.0;
		s.color_g = 0.0;
		s.color_b = 0.0;
		/*add to every frame from this one to the last one*/
		for (int i=cur_frame; i < objects.size(); i++)
		{
			objects[i]->push_back(s);
		}
		glutPostRedisplay();
	}
	else if (option == DELETE_OBJECT)
	{
		if (selected_sq)
		{
			selected_sq->x = -1;
			selected_sq = NULL;
			glutPostRedisplay();
		}
	}
	else if (option == KEY_FRAME_CHANGE)
	{
		cur_frame = (cur_frame + 1) % max_keyframes;
		if (cur_frame >= objects.size())
		{
			if (cur_frame != 0)
			{
				vector<square>* vec = new vector<square>
						(*(objects[cur_frame - 1]));
				objects.push_back(vec);
			}
		}
		selected_sq = NULL;
		glutPostRedisplay();
		glutSetWindow(infowindow);
		glutPostRedisplay();
		glutSetWindow(editwindow);
	}
	else if (option == PLAY_ANIMATION)
	{
		cur_mode = ANIMATE_MODE;
		selected_sq = NULL;
		cur_frame = 0;
		finish_animation = false;
		time_per_frame = total_time*100 / objects.size();
		timer(1);
	}
	else if (option == ADD_KEYFRAME)
	{
		max_keyframes++;
	}
}

void color_menu(int option)
{
	if (selected_sq)
	{
		switch( option )
		{
			case COLOR_RED:
				selected_sq->color_r = 1.0;
				selected_sq->color_g = 0.0;
				selected_sq->color_b = 0.0;
				break;
			case COLOR_BLUE:
				selected_sq->color_r = 0.0;
				selected_sq->color_g = 0.0;
				selected_sq->color_b = 1.0;
				break;
			case COLOR_GREEN:
				selected_sq->color_r = 0.0;
				selected_sq->color_g = 1.0;
				selected_sq->color_b = 0.0;
				break;
			case COLOR_BLACK:
				selected_sq->color_r = 0.0;
				selected_sq->color_g = 0.0;
				selected_sq->color_b = 0.0;
				break;
		}
		glutPostRedisplay();
	}
}

void time_menu(int option)
{
	switch (option)
	{
	case INC_TIME:
		total_time++;
		break;
	case DEC_TIME:
		total_time -= total_time == 0 ? 0 : 1;
		break;
	}
	glutSetWindow(infowindow);
	glutPostRedisplay();
	glutSetWindow(editwindow);
}

void init_edit_menu()
{
	int trans_sub_menu = glutCreateMenu(trans_menu);
	glutAddMenuEntry("Rotate +", ROTATE_P);
	glutAddMenuEntry("Rotate -", ROTATE_N);
	glutAddMenuEntry("ScaleX +", SCALE_X_P);
	glutAddMenuEntry("ScaleX -", SCALE_X_N);
	glutAddMenuEntry("ScaleY +", SCALE_Y_P);
	glutAddMenuEntry("ScaleY -", SCALE_Y_N);

	int color_sub_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", COLOR_RED);
	glutAddMenuEntry("Green", COLOR_GREEN);
	glutAddMenuEntry("Blue", COLOR_BLUE);
	glutAddMenuEntry("Black", COLOR_BLACK);

	int time_sub_menu = glutCreateMenu(time_menu);
	glutAddMenuEntry("Increase", INC_TIME);
	glutAddMenuEntry("Decrease", INC_TIME);

	int edit_menu = glutCreateMenu(edit_menu_callback);
	glutAddMenuEntry("New Object", NEW_OBJECT);
	glutAddMenuEntry("Delete Object", DELETE_OBJECT);
	glutAddMenuEntry("Play Animation", PLAY_ANIMATION);
	glutAddMenuEntry("Change Keyframe", KEY_FRAME_CHANGE);
	glutAddMenuEntry("Add Keyframe at end", ADD_KEYFRAME);
	glutAddSubMenu("Transformations", trans_sub_menu);
	glutAddSubMenu("Color", color_sub_menu);
	glutAddSubMenu("Total Time", time_sub_menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	
	/*set up the main window*/
	mainwindow = glutCreateWindow("Homework 2 - Key Frame Animation");
	//glutDisplayFunc(display_main);

	/*set up the edit window: the animation window that is*/
	editwindow = glutCreateSubWindow(mainwindow, 0, 0, WINDOW_WIDTH, 
							3*WINDOW_HEIGHT/4);
	init_edit_menu();
	glutDisplayFunc(display_edit);
	glutMouseFunc(edit_mouse_callback);
	glutPassiveMotionFunc(edit_passive_motion);
	glutMotionFunc(edit_mouse_motion);
	glutKeyboardFunc(myKeyboardCB);
	gluOrtho2D(0, WINDOW_WIDTH, 0, 3*WINDOW_HEIGHT/4);

	/*set up the info window below edit window*/
	infowindow = glutCreateSubWindow(mainwindow, 0, 
					3*WINDOW_HEIGHT/4, 
					WINDOW_WIDTH, 
					WINDOW_HEIGHT - 3*WINDOW_HEIGHT/4);
	glutDisplayFunc(display_info);
	gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT - 3*WINDOW_HEIGHT/4);

	/*set up global vars*/
	leftdown = false;
	selected_sq = NULL;
	cur_frame = 0;
	cur_mode = KEYFRAME_MODE;
	max_keyframes = 5;
	total_time = 1;
	objects.push_back(new vector<square>());

	/*The main loop*/
	glutMainLoop();
	return 0;
}
