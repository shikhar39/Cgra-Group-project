
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
#include "VoxelGrid.hpp"


using namespace std;
using namespace cgra;
using namespace glm;


float randomFloat(float a, float b) {
	return a + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(b-a)));
}
void Application::load_sphere_lat_long(float radius, int latDiv, int longDiv ) {
	mesh_builder mb;
	
	for(int j = 0; j < longDiv + 1; j++) {
		float phi = j * glm::pi<GLfloat>()/longDiv;
		
		for(int i = 0; i < latDiv; i++ ) {
			float theta =  i * 2 * glm::pi<GLfloat>()/(latDiv - 1);
			
			glm::vec3 pos(glm::sin(theta) * glm::sin(phi), glm::cos(theta)* glm::sin(phi), glm::cos(phi));
			glm::vec2 tex_uv((1.0/latDiv)* i, (1.0/longDiv) * j);
			glm::vec3 tan(-glm::sin(theta), glm::cos(theta), 0);
			glm::vec3 bitan(glm::cos(theta) * glm::cos(phi), glm::sin(theta)* glm::cos(phi), -glm::sin(phi));

			mb.push_vertex(mesh_vertex{radius * pos, pos, tan, bitan, tex_uv});
			
		}
	}
	for (int j = 0; j < longDiv ; j++) {
		for(int i = 0; i < latDiv - 1; i++){
			mb.push_index(j * latDiv + i);
			mb.push_index(j * latDiv + i + 1);
			mb.push_index((j + 1) * latDiv + i);
			mb.push_index(j * latDiv + i + 1);
			mb.push_index((j + 1) * latDiv + i);
			mb.push_index((j + 1) * latDiv + i + 1);
		}

	}
	m_model.mesh = mb.build();


}


Application::Application(GLFWwindow *window) : m_window(window) {
	
	// build the shader for the model
	shader_builder color_sb;
	color_sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//HeightMapVert.glsl"));
	color_sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//default_frag.glsl"));
	GLuint color_shader = color_sb.build();

	// build the mesh for the model
	// mesh_builder teapot_mb = load_wavefront_data(CGRA_SRCDIR + std::string("//res//assets//sphere.obj"));
	// gl_mesh teapot_mesh = teapot_mb.build();


	load_sphere_lat_long(m_sp_rad, m_latDivision, m_longDivision);
	// put together an object
	m_model.shader = color_shader;
	// m_model.mesh = teapot_mesh;
	m_model.color = glm::vec3(1, 0, 0);
	m_model.modelTransform = glm::mat4(1);


	m_heightMap = HeightMap(glm::vec3{ 0.f, 0.f, 0.f }, glm::vec3{ 30.f, 15.f, 5.f }, 0.1f);
	for (int i = 200; i < 240; i++) {
		for (int j = 80; j < 120; j++) {
			float a = (i - 200) > (240 - i) ? (240 - i) : (i - 200);
			float b = 20 - abs(220 - i) + abs(100 - j);
			m_heightMap.SetTerrainHeight({i,j}, 10 + 0.1 * a);
		}
	}
	m_computeShader1 = ComputeShader(CGRA_SRCDIR + std::string("//res//shaders//HeightMapCompute1.glsl"));
	m_computeShader2 = ComputeShader(CGRA_SRCDIR + std::string("//res//shaders//HeightMapCompute2.glsl"));
	


	glGenBuffers(1, &vertexCountBuffer);


	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, vertexCountBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_STREAM_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	

	glGenBuffers(1, &ssbo2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_heightMap.GetGridTotalCount() * sizeof(waterMeshVertex), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);





	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_heightMap.GetGridTotalCount() * sizeof(cellParams), NULL, GL_DYNAMIC_DRAW);

	GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT ; // the invalidate makes a big difference when re-writing
	
	glm::ivec2 gridDim = m_heightMap.GetGridSize();
	cellParams* cellParam = (cellParams *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_heightMap.GetGridTotalCount() * sizeof(cellParams), bufMask);
	/*
	*/
	for (int i = 0; i < m_heightMap.GetGridTotalCount(); i++) {
		uint x, y;
		// Linear idx = y * X_size + X
		y = i / gridDim.x;
		x = i % gridDim.x;

		cellParam[i] = m_heightMap.Get(x, y);
	}

 	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// ############################ calculate the projection and view matrix ##########################
	mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);
	
	if (m_isWPressed) m_cam_pos += m_cam_dir * m_cam_speed;
	if (m_isSPressed) m_cam_pos -= m_cam_dir * m_cam_speed;
	if (m_isAPressed) m_cam_pos -= normalize(cross(m_cam_dir, m_cam_up)) * m_cam_speed;
	if (m_isDPressed) m_cam_pos += normalize(cross(m_cam_dir, m_cam_up)) * m_cam_speed;
	if (m_isUpPressed) m_cam_pos += normalize(vec3(m_cam_dir.x, 0.0f, m_cam_dir.z)) * m_cam_speed;
	if (m_isDownPressed) m_cam_pos -= normalize(vec3(m_cam_dir.x, 0.0f, m_cam_dir.z)) * m_cam_speed;
	if (m_isLeftPressed) m_cam_pos -= normalize(cross(m_cam_dir, m_cam_up)) * m_cam_speed;
	if (m_isRightPressed) m_cam_pos += normalize(cross(m_cam_dir, m_cam_up)) * m_cam_speed;


	mat4 view = lookAt(m_cam_pos, m_cam_dir + m_cam_pos, m_cam_up);
	// ################################################################################################


	// ################### draw options ###########################
	if (m_show_grid) cgra::drawGrid(view, proj);
	if (m_show_axis) cgra::drawAxis(view, proj);
	glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);
	// ############################################################


	// ####################################### Compute Shader call ############################################
	m_computeShader1.use();
	
	// Reset atomic counter
	GLuint zero = 0;
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, vertexCountBuffer);
	glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);
	// glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	// 

	// Set Binding points for all buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0 ,ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1 ,ssbo2);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, vertexCountBuffer);
	//

	// Set Uniforms
	static float time = glfwGetTime();
	glUniform1f(glGetUniformLocation(m_computeShader1.ID, "time"), glfwGetTime());
	glUniform1f(glGetUniformLocation(m_computeShader1.ID, "gridWidth"), m_heightMap.GetGridWidth());
	glUniform1i(glGetUniformLocation(m_computeShader1.ID, "gridCountX"), m_heightMap.GetGridSize().x);
	glUniform1i(glGetUniformLocation(m_computeShader1.ID, "gridCountY"), m_heightMap.GetGridSize().y);
	glUniform1i(glGetUniformLocation(m_computeShader1.ID, "Pulse"), (int)m_simPulse);
	//

	// Compute Shader Dispatch
	auto dispatchSize = (unsigned int)m_heightMap.GetGridSize().x / 10;
	glDispatchCompute((unsigned int)(m_heightMap.GetGridSize().x / 10), (unsigned int)(m_heightMap.GetGridSize().y / 10), 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);


	m_computeShader2.use();
	
	glUniform1f(glGetUniformLocation(m_computeShader2.ID, "time"), glfwGetTime());
	glUniform1f(glGetUniformLocation(m_computeShader2.ID, "gridWidth"), m_heightMap.GetGridWidth());
	glUniform1i(glGetUniformLocation(m_computeShader2.ID, "gridCountX"), m_heightMap.GetGridSize().x);
	glUniform1i(glGetUniformLocation(m_computeShader2.ID, "gridCountY"), m_heightMap.GetGridSize().y);
	glUniform1i(glGetUniformLocation(m_computeShader2.ID, "Pulse"), (int)m_simPulse);
	glUniform1f(glGetUniformLocation(m_computeShader2.ID, "Dampening"), m_dampening);

	//

	// Compute Shader Dispatch
	// auto dispatchSize = (unsigned int)m_heightMap.GetGridSize().x / 10;
	glDispatchCompute((unsigned int)(m_heightMap.GetGridSize().x / 10), (unsigned int)(m_heightMap.GetGridSize().y / 10), 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);
	


	time = glfwGetTime(); // Setting up time here to be passed as a uniform later
	m_simPulse = false;
	// Read atomic counter value. This is more for  debugging. not needed actually. remove later
	/*
	*/
	// GLuint counterValue = 0;
	// // glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, vertexCountBuffer);
	// glGetBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &counterValue);
	// glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	// ########################################################################################################

	// ###################################### Drawing, The graphics Pipeline #############################
	glUseProgram(m_model.shader);
	glUniformMatrix4fv(glGetUniformLocation(m_model.shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
	glUniform3fv(glGetUniformLocation(m_model.shader, "ucamPos"), 1, value_ptr(m_cam_pos));
	glUniformMatrix4fv(glGetUniformLocation(m_model.shader, "uViewMatrix"), 1, false, value_ptr(view));
	glUniform1i(glGetUniformLocation(m_model.shader, "gridCountX"), m_heightMap.GetGridSize().x);
	glUniform1i(glGetUniformLocation(m_model.shader, "gridCountY"), m_heightMap.GetGridSize().y);
	glBindVertexArray(m_model.mesh.vao);
	glDrawElementsInstanced(m_model.mesh.mode, m_model.mesh.index_count, GL_UNSIGNED_INT, 0, m_heightMap.GetGridTotalCount());
	// ###################################################################################################

}


void Application::renderGUI() {

	// setup window
	ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiSetCond_Once);
	ImGui::Begin("Camera", 0);

	// display current camera parameters
	ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::SliderFloat("Cam speed", &m_cam_speed, 0, 0.5, "%.3f");
	ImGui::SliderFloat("Cam Rot speed", &m_cam_rotation_scale, 0, 0.5, "%.3f");
	if (ImGui::Button("Pulse")) m_simPulse = !m_simPulse;
	ImGui::SliderFloat("Wave Dampening", &m_dampening, 0.95, 1.0, "%.3f");

	// extra drawing parameters
	ImGui::SliderInt("N Render", &drawCount, 1, num_particles);
	int prevLatDiv = m_latDivision;
	int prevLongDiv = m_longDivision;
	float prevRad = m_sp_rad;
	ImGui::SliderFloat("Radius", &m_sp_rad, 0.05f, 1.0f);
	ImGui::SliderInt("Lat Divisions", &m_latDivision, 2, 30);
	ImGui::SliderInt("Long Divisions", &m_longDivision, 2, 30);
	if (prevLatDiv != m_latDivision || prevLongDiv != m_longDivision || prevRad != m_sp_rad){
		load_sphere_lat_long(m_sp_rad, m_latDivision, m_longDivision);
	}
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
	if (m_isMouse1Pressed) {
		vec2 current_pos((float)xpos, (float)ypos) ;
		vec2 mouse_drag_len = current_pos - m_mouse_drag_old;
		m_mouse_drag_old = current_pos;
		//cout << "Mouse was dragged X: " << mouse_drag_len.x << " Y:" << mouse_drag_len.y << endl;
		// Vertical Mouse drag (Pitch control)causing a rotation aroud an axis perpendicular to up and cam look direction
		float pitch_angle = glm::radians(mouse_drag_len.y * m_cam_rotation_scale);
		float yaw_angle = glm::radians(mouse_drag_len.x * m_cam_rotation_scale);

		glm::mat4 yaw_rotation_matrix = glm::rotate(glm::mat4(1.0f), yaw_angle, m_cam_up);
		m_cam_dir = normalize(glm::vec3(yaw_rotation_matrix * glm::vec4(m_cam_dir, 1.0f)));

		vec3 pitch_axis = normalize(glm::cross(m_cam_dir, m_cam_up));
		glm::mat4 pitch_rotation_matrix = glm::rotate(glm::mat4(1.0f), pitch_angle, pitch_axis);
		m_cam_dir = normalize(glm::vec3(pitch_rotation_matrix * glm::vec4(m_cam_dir, 1.0f)));
		//m_cam_up = normalize(glm::cross(pitch_axis, m_cam_dir));
	}
}


void Application::mouseButtonCallback(int button, int action, int mods) {
	
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
		m_isMouse1Pressed = true;
		double x, y;
		glfwGetCursorPos(m_window, &x, &y);
		m_mouse_drag_old = vec2(x, y);
		//cout << "MB1 was pressed , updated mdOld" << m_mouse_drag_old.x << " " << m_mouse_drag_old.y << endl;
	}
	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
		m_isMouse1Pressed = false;
	}
}


void Application::scrollCallback(double xoffset, double yoffset) {
	(void)xoffset; // currently un-used
	m_cam_pos = m_cam_pos + m_cam_dir * (float)yoffset;
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
	(void)key, (void)scancode, (void)action, (void)mods; // currently un-used
	if (key == GLFW_KEY_W && action == GLFW_PRESS) m_isWPressed = true;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) m_isWPressed = false;
	if (key == GLFW_KEY_S && action == GLFW_PRESS) m_isSPressed = true;
	if (key == GLFW_KEY_S && action == GLFW_RELEASE) m_isSPressed = false;
	if (key == GLFW_KEY_A && action == GLFW_PRESS) m_isAPressed = true;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) m_isAPressed = false;
	if (key == GLFW_KEY_D && action == GLFW_PRESS) m_isDPressed = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) m_isDPressed = false;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) m_isUpPressed = true;
	if (key == GLFW_KEY_UP && action == GLFW_RELEASE) m_isUpPressed = false;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) m_isDownPressed = true;
	if (key == GLFW_KEY_DOWN && action == GLFW_RELEASE) m_isDownPressed = false;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) m_isLeftPressed = true;
	if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) m_isLeftPressed = false;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) m_isRightPressed = true;
	if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE) m_isRightPressed = false;

}


void Application::charCallback(unsigned int c) {
	(void)c; // currently un-used
}