#pragma once

#include "tests/Test.h"

#include <iostream>

#include "Shader.h"
#include "Texture.h"

#include <memory>

//#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"	

#include "Mesh.h"
#include "Model.h"

namespace test {

	class T08_Advanced_GLSL_03_UniformBuff : public Test
	{
	public:
		T08_Advanced_GLSL_03_UniformBuff();
		~T08_Advanced_GLSL_03_UniformBuff();

		void OnUpdate(float deltaTime) override;
		void OnRender(GLFWwindow* window) override;
		void OnImGuiRender() override;
		void OnProcessInput(GLFWwindow* window, float deltaTime) override;
		// callbacks functions
		void framebuffer_size_callback(GLFWwindow* window, int width, int height) override;
		void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	private:
		float m_f_fov;

		bool m_b_firstMouse;
		float m_lastX, m_lastY;
		std::unique_ptr<Camera> m_camera;
		// keys pressed options
		bool m_mouse_disable, m_mouse_disable_i_1;
		bool m_mouse_lock;
		bool m_key_pressed = false;

		// MESH members
		std::unique_ptr<Mesh> m_mesh;
		std::shared_ptr<Shader> m_ShaderMeshRed, m_ShaderMeshGreen, m_ShaderMeshBlue, m_ShaderMeshYellow;
		std::vector<std::shared_ptr<Texture>> msp_Textures;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		void DrawCube(std::shared_ptr<Shader> shader, glm::vec3 position);
		int m_framebufferWidth, m_framebufferHeight;

		// Uniform Buffers object
		unsigned int m_uboMatrices;

	};

}

