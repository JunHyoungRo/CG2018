#pragma once
#include <LinearMath/btIDebugDraw.h>
#include "glm/glm.hpp"
#include "Mesh.hpp"
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>


#include "btBulletDynamicsCommon.h"
#include "LinearMath/btIDebugDraw.h"


using namespace std;

class GL_btDebugDraw : public btIDebugDraw
{
public:
	int m_debugMode;
	GLuint vertexbuffer, VertexArrayID;
	GL_btDebugDraw();
	virtual ~GL_btDebugDraw();
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}

	virtual void reportErrorWarning(const char* warningString) {};

	virtual void draw3dText(const btVector3& location, const char* textString) {};

	virtual void setDebugMode(int debugMode) {
		m_debugMode = debugMode;
	};

	virtual int	 getDebugMode() const { return 0; };
	
};


GL_btDebugDraw::GL_btDebugDraw() 
{
}

GL_btDebugDraw::~GL_btDebugDraw()
{
}


void GL_btDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	//cout << "calling debuger " << endl;
	//GLfloat vertices[] = {
	//	to.x(), to.y(), to.z(),
	//	from.x(), from.y(), from.z()
	//};
	//vertices_ptr = vertices;
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glBindVertexArray(vao);

	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	//	3,                  // size
	//	GL_FLOAT,           // type
	//	GL_FALSE,           // normalized?
	//	0,                  // stride
	//	(void*)0            // array buffer offset
	//);
	//glDrawArrays(GL_LINE_LOOP, 0, 2); // 6 vertices
	//glBindVertexArray(0);
	////glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Vertex data
	GLfloat points[12];

	points[0] = from.x();
	points[1] = from.y();
	points[2] = from.z();
	points[3] = color.x();
	points[4] = color.y();
	points[5] = color.z();

	points[6] = to.x();
	points[7] = to.y();
	points[8] = to.z();
	points[9] = color.x();
	points[10] = color.y();
	points[11] = color.z();

	glGenBuffers(1, &vertexbuffer);
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glLineWidth(3);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}
