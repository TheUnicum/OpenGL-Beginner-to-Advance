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

	class T01_DepthTesting : public Test
	{
	public:
		T01_DepthTesting();
		~T01_DepthTesting();

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

		// lighting
		glm::vec3 m_lightPos;
		bool m_b_ambient, m_b_diffuse, m_b_specular, m_b_emission;

		//
		bool m_b_light_move_active;
		bool m_b_cube_rotating_active;
		bool m_b_cube_scale_active;

		// MESH members
		std::unique_ptr<Mesh> m_mesh, m_mesh_quad;
		std::shared_ptr<Shader> m_ShaderMesh;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTexture1, msp_mTexture2;
		std::vector<std::shared_ptr<Texture>> msp_Textures;
		// Floor
		std::shared_ptr<Texture> msp_mTextureFloor;
		std::vector<std::shared_ptr<Texture>> msp_mTextures2;

		// MODEL
		std::unique_ptr<Model> m_model;

		// Depht Testing
		bool m_b_Depht_GL_Always_active, m_b_Depht_GL_Always_active_i_1;
		int m_i_DP_Visualizing_opt;

	};

}

