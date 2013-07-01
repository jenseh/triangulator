#include <stdio.h>
#include <vector>
#include <GL/glfw.h>
#include <glm/glm.hpp>
using namespace glm;

#include "happah/triangulators/PlanarGraphTriangulatorSeidel.h"

int width = 1280;
int height = 720;

std::vector<std::vector<vec2>> verticesLists;

PlanarGraphTriangulatorSeidel triangulator;
TriangleMesh2D* triangles = NULL;
PlanarGraphTriangulatorSeidel::TrapezoidMesh2D* trapezoidulation = NULL;

vec3 colors[] = {vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f),
				 vec3(1.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f), 
				 vec3(1.0f, 0.5f, 0.0f), vec3(1.0f, 0.0f, 0.5f), vec3(0.5f, 1.0f, 0.0f),
				 vec3(0.0f, 1.0f, 0.5f), vec3(0.5f, 0.0f, 1.0f), vec3(0.0f, 0.5f, 1.0f)};
int nColors = 12;

void onResize(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

void GLFWCALL onClick(int button, int action) {
	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		ivec2 pos;
		glfwGetMousePos(&pos.x, &pos.y);
		verticesLists.back().push_back(vec2(2.0f / width, -2.0f / height) * vec2(pos) + vec2(-1.0f, 1.0f));
		delete triangles;
		triangles = NULL;
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		verticesLists.emplace_back();
		ivec2 pos;
		glfwGetMousePos(&pos.x, &pos.y);
		verticesLists.back().push_back(vec2(2.0f / width, -2.0f / height) * vec2(pos) + vec2(-1.0f, 1.0f));
		delete triangles;
		triangles = NULL;
	}
}

void GLFWCALL onKey(int key, int action) {
	if(action == GLFW_PRESS) {
		std::vector<SegmentEndpoints2D> segments;// vordeklarieren da im switch nicht möglich
		switch(key) {
			case GLFW_KEY_BACKSPACE:
				if(verticesLists.back().size() > 0) {
					verticesLists.back().pop_back();
					if(verticesLists.back().size() == 0 && verticesLists.size() > 1) {
						verticesLists.pop_back();
					}
				}
				delete trapezoidulation;
				trapezoidulation = NULL;
				delete triangles;
				triangles = NULL;
				break;
			case GLFW_KEY_DEL:
				verticesLists.clear();
				verticesLists.emplace_back();
				delete trapezoidulation;
				trapezoidulation = NULL;
				delete triangles;
				triangles = NULL;
				break;
			case GLFW_KEY_SPACE:
				for(auto verticesList : verticesLists) {
					if(verticesList.size() > 2) {
						segments.emplace_back(verticesList.back(), verticesList.front());
						for(auto it = verticesList.begin() + 1; it != verticesList.end(); it++) {
							segments.emplace_back(*(it - 1), *it);
						}
					}
				}
				delete trapezoidulation;
				trapezoidulation = triangulator.trapezoidulate(segments.begin(), segments.end());
				delete triangles;
				//triangles = triangulator.triangulate(segments.begin(), segments.end());
				break;
			case GLFW_KEY_ESC:
				glfwCloseWindow();
				break;
		}
	}
}

int GLFWCALL main(int argc, char **argv) {
	
	glfwInit();
	glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW);
	glfwSetWindowSizeCallback(onResize);
	glfwSetMouseButtonCallback(onClick);
	glfwSetKeyCallback(onKey);
	
	glPointSize(5.0f);
	glEnable(GL_POINT_SMOOTH);
	glLineWidth(3.0f);
	
	verticesLists.emplace_back();
	
	while(glfwGetWindowParam(GLFW_OPENED)) {
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		if(triangles != NULL) {
			glBegin(GL_TRIANGLES);
			int i = 0;
			for(hpuint index : *triangles->getIndices()) {
				glColor3fv((float*) &colors[i / 3]);
				glVertex2fv((float*) &(*triangles->getVerticesAndNormals())[2 * index]);
				i = (i + 1) % (3 * nColors);
			}
			glEnd();
		}
		
		for(std::vector<vec2> verticesList : verticesLists) {
			glBegin(GL_LINE_LOOP);
			glColor3f(1.0f, 1.0f, 1.0f);
			for(vec2 vertex : verticesList) {
				glVertex2fv((float*)&vertex);
			}
			glEnd();
		}
		
		if(verticesLists.size() > 0 && verticesLists.back().size() > 1) {
			glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2fv((float*)&verticesLists.back()[0]);
			glVertex2fv((float*)&verticesLists.back()[verticesLists.back().size() - 1]);
			glColor3f(1.0f, 1.0f, 0.0f);
			glVertex2fv((float*)&verticesLists.back()[verticesLists.back().size() - 1]);
			glVertex2fv((float*)&verticesLists.back()[verticesLists.back().size() - 2]);
			glEnd();
		}
		
		glBegin(GL_POINTS);
		glColor3f(0.0f, 0.0f, 1.0f);
		for(std::vector<vec2> verticesList : verticesLists) {
			for(vec2 vertex : verticesList) {
				glVertex2fv((float*)&vertex);
			}
		}
		glEnd();
		
		if(trapezoidulation) {
			trapezoidulation->drawDebug();
		}
		
		glfwSwapBuffers();
		
		
	};
	
	glfwTerminate();
	return 0;
}
