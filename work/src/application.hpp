
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"
#include "basic_model.hpp"
#include "VoxelGrid.hpp"
#include "ComputeShader.hpp"

// Main application class
//
class Application {
private:
	struct particle {
		glm::vec4 pos; // positions
		glm::vec4 vel;
	};

	struct waterMeshVertex {
		glm::vec4 pos;
		glm::vec4 normal;
		glm::vec4 color;
	};

	// window
	glm::vec2 m_windowsize;
	GLFWwindow *m_window;

	// oribital camera
	float m_cam_rotation_scale = 0.05f;
	float m_cam_speed = 0.09f;

	float m_pitch = .86;
	float m_yaw = -.86;
	float m_distance = 20.0;
	glm::vec3 m_cam_pos = glm::vec3(0.0f, 5.0f, 5.0f);
	glm::vec3 m_cam_dir = glm::vec3(0.0f, -1.0f, -1.0f);
	glm::vec3 m_cam_up = glm::vec3(0.0f, 0.0f, 1.0f);

	//Keyboard and Mouse input states
	bool m_isMouse1Pressed = false;
	bool m_isWPressed = false;
	bool m_isAPressed = false;
	bool m_isSPressed = false;
	bool m_isDPressed = false;
	bool m_isUpPressed = false;
	bool m_isDownPressed = false;
	bool m_isLeftPressed = false;
	bool m_isRightPressed = false;
	glm::vec2 m_mouse_drag_old = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_mouse_drag_new = glm::vec2(0.0f, 0.0f);

	// last input
	bool m_leftMouseDown = false;
	glm::vec2 m_mousePosition;

	// drawing flags
	bool m_show_axis = false;
	bool m_show_grid = false;
	bool m_showWireframe = false;

	//Shape properties
	float m_sp_rad = 0.05f;
	int m_latDivision = 8;
	int m_longDivision = 8;

	// basic model
	// contains a shader, a model transform
	// a mesh, and other model information (color etc.)
	basic_model m_model; 
	HeightMap m_heightMap;
	ComputeShader m_computeShader;

	glm::uint num_particles = 1000;
	int drawCount = 100; 

	const size_t MAX_VERTICES = 1000000;
	GLuint vertexCountBuffer;
	GLuint ssbo = 0;
	GLuint ssbo2 = 0;
	
	bool m_simPulse = false;


public:
	// setup
	Application(GLFWwindow *);

	// disable copy constructors (for safety)
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// rendering callbacks (every frame)
	void render();
	void renderGUI();
	void load_sphere_lat_long(float radius, int latDiv, int longDiv);
	// input callbacks
	void cursorPosCallback(double xpos, double ypos);
	void mouseButtonCallback(int button, int action, int mods);
	void scrollCallback(double xoffset, double yoffset);
	void keyCallback(int key, int scancode, int action, int mods);
	void charCallback(unsigned int c);
};