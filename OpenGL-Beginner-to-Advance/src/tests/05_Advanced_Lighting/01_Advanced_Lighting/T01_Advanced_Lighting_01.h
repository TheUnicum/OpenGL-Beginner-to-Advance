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

	class T01_Advanced_Lighting_01 : public Test
	{
	public:
		T01_Advanced_Lighting_01();
		~T01_Advanced_Lighting_01();

		void OnUpdate(float deltaTime) override;
		void OnRender(GLFWwindow* window) override;
		void OnImGuiRender() override;
		void OnProcessInput(GLFWwindow* window, float deltaTime) override;
		// callbacks functions
		void framebuffer_size_callback(GLFWwindow* window, int width, int height) override;
		void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	private:
		float m_f_fov;
		bool m_b_depth_test_active, m_b_depth_test_active_i_1;

		bool m_b_firstMouse;
		float m_lastX, m_lastY;
		std::unique_ptr<Camera> m_camera;
		// keys pressed options
		bool m_mouse_disable, m_mouse_disable_i_1;
		bool m_mouse_lock;
		bool m_key_pressed = false;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		// MESH members
		std::unique_ptr<Mesh> m_mesh_quad;
		std::shared_ptr<Shader> m_ShaderMesh;

		std::shared_ptr<Texture> msp_mTextureFloor;
		std::vector<std::shared_ptr<Texture>> msp_mTextures1;
		bool m_b_blinn, m_b_disable_adapt_shininess, m_b_specular_texture;

		// lighting
		glm::vec3 m_lightPos;
		bool m_b_ambient, m_b_diffuse, m_b_specular, m_b_emission;
	};

}

