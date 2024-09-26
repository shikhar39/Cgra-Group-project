
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"
#include "basic_model.hpp"
#include "VoxelGrid.hpp"
#include "ComputeShader.hpp"
#define NUM_PARTICLES 100000 * 1000
// Main application class
//
class Application {
private:
	struct particle {
		glm::vec4 pos; // positions
		glm::vec4 vel;
	};

	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_pitch = .86;
	float m_yaw = -.86;
	float m_distance = 20.0;

	// last input
	bool m_leftMouseDown = false;
	glm::vec2 m_mousePosition;

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;

	// basic model
	// contains a shader, a model transform
	// a mesh, and other model information (color etc.)
	basic_model m_model;
	VoxelGrid m_voxelGrid;
	ComputeShader m_computeShader;
	const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;
	unsigned int texture;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ibo = 0;
	GLenum mode = 0; // mode to draw in, eg: GL_TRIANGLES
	int index_count = 0; // how many indicies to draw (no primitives)
	GLuint ssbo = 0;

public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();

	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};