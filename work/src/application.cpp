
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "application.hpp"
#include "bounding_box.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "cgra/cgra_wavefront.hpp"


using namespace std;
using namespace cgra;
using namespace glm;


float randomFloat(float a, float b) {
	return a + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(b-a)));
}

Application::Application(GLFWwindow *window) : m_window(window) {
	
	// build the shader for the model
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_vert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	GLuint color_shader = color_sb.build();

	// build the mesh for the model
	mesh_builder teapot_mb = load_wavefront_data(CGRA_SRCDIR + std::string("//res//assets//sphere.obj"));
	gl_mesh teapot_mesh = teapot_mb.build();

	// put together an object
	m_model.shader = color_shader;
	m_model.mesh = teapot_mesh;
	m_model.color = glm::vec3(1, 0, 0);
	m_model.modelTransform = glm::mat4(1);


	m_voxelGrid = VoxelGrid(glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 10.f, 6.f, 5.f }, 1.f);
	ivec3 size = m_voxelGrid.GetGridSize();
	
	m_computeShader = ComputeShader(CGRA_SRCDIR + std::string("//res//shaders//Compute.glsl"));

	for (int i = 0; i < size.x; i++) {
		for (int j = 0; j < size.y; j++) {
			for (int k = 0; k < size.z/2; k++) {
				m_voxelGrid.UpdateVoxel({i, j, k}, VoxelGrid::TERRAIN);
			}
		}
	}
	m_voxelGrid.Print();

	float vertices[] = {
		-0.8f, -0.8f,  0.0f,  0.f, 0.f,
		 0.8f, -0.8f,  0.0f,  1.f, 0.f,
		-0.8f,  0.8f,  0.0f,  0.f, 1.f,
		 0.8f,  0.8f,  0.0f,  1.f, 1.f,
	};
	
	unsigned int indices[] = {
		0, 1, 2, 1, 2, 3
	};
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*) (3 * sizeof(GL_FLOAT)));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	index_count = 6;
	glBindVertexArray(0);

	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_PARTICLES * sizeof(particle), NULL, GL_STATIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ; // the invalidate makes a big difference when re-writing
	particle *points = (particle *) glMapBufferRange( GL_SHADER_STORAGE_BUFFER, 0, NUM_PARTICLES * sizeof(particle), bufMask );
	for( int i = 0; i < NUM_PARTICLES; i++ ) {
		// points[ i ].pos = vec4(i % 100, 10.f, int(i / 100), 1.0f);
		points[i].pos = vec4(randomFloat(-100.f, 100.f), randomFloat(1.f, 100.f), randomFloat(-100.f, 100.f), 1.0f);
		points[ i ].vel = vec4(0.f, 0.f, 0.f, 1.0f);

	}
 	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	/*
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, 
				GL_FLOAT, NULL);

	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY , GL_RGBA32F);
	*/
}


void Application::render() {
	
	// retrieve the window hieght
	int width, height;
	glfwGetFramebufferSize(m_window, &width, &height); 

	m_windowsize = vec2(width, height); // update window size
	glViewport(0, 0, width, height); // set the viewport to draw to the entire window

	// clear the back-buffer
	glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	
	// enable flags for normal/forward rendering
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	// calculate the projection and view matrix
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);
	// view matrix
	mat4 view = translate(mat4(1), vec3(0, 0, -m_distance))
		* rotate(mat4(1), m_pitch, vec3(1, 0, 0))
		* rotate(mat4(1), m_yaw,   vec3(0, 1, 0));
	// draw options
	if (m_show_grid) cgra::drawGrid(view, proj);
	if (m_show_axis) cgra::drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

	// draw the model
	// m_model.draw(view, proj);
	int drawCount = 1000 * 1000;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0 ,ssbo);
	glUseProgram(m_model.shader);
	glUniformMatrix4fv(glGetUniformLocation(m_model.shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniformMatrix4fv(glGetUniformLocation(m_model.shader, "uViewMatrix"), 1, false, value_ptr(view));
	glBindVertexArray(m_model.mesh.vao);
	glDrawElementsInstanced(m_model.mesh.mode, m_model.mesh.index_count, GL_UNSIGNED_INT, 0, drawCount);
	/*
	*/

	m_computeShader.use();
	// computeShader.setFloat("t", currentFrame);
	static float time = glfwGetTime();
	glUniform1f(glGetUniformLocation(m_computeShader.ID, "t"), glfwGetTime() - time);
	time = glfwGetTime();

	glDispatchCompute((unsigned int)drawCount/ 1000, 1, 1);

	// make sure writing to image has finished before read
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// glUseProgram(m_model.shader);
	// // glUniform1i(glGetUniformLocation(m_model.shader, "tex"), 0);
	// // glActiveTexture(GL_TEXTURE0);
	// // glBindTexture(GL_TEXTURE_2D, texture);
	// glBindVertexArray(vao);
	// glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
	
}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.1f");
	ImGui::SliderFloat3("Model Color", value_ptr(m_model.color), 0, 1, "%.2f");

	// extra drawing parameters
	ImGui::Checkbox("Show axis", &m_show_axis);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &m_show_grid);
	ImGui::Checkbox("Wireframe", &m_showWireframe);
	ImGui::SameLine();
	if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

	// finish creating window
	ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
	if (m_leftMouseDown) {
		vec2 whsize = m_windowsize / 2.0f;

		// clamp the pitch to [-pi/2, pi/2]
		m_pitch += float(acos(glm::clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f))
			- acos(glm::clamp((float(ypos) - whsize.y) / whsize.y, -1.0f, 1.0f)));
		m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

		// wrap the yaw to [-pi, pi]
		m_yaw += float(acos(glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f))
			- acos(glm::clamp((float(xpos) - whsize.x) / whsize.x, -1.0f, 1.0f)));
		if (m_yaw > pi<float>()) m_yaw -= float(2 * pi<float>());
		else if (m_yaw < -pi<float>()) m_yaw += float(2 * pi<float>());
	}

	// updated mouse position
	m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	(void)mods; // currently un-used
	// capture is left-mouse down
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void)xoffset; // currently un-used
	m_distance *= pow(1.1f, -yoffset);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}