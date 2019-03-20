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

/*
Notes:
Make option to outlining in the shader to active or disable.

*/

namespace test {

	class T02_StencilTesting : public Test
	{
	public:
		T02_StencilTesting();
		~T02_StencilTesting();

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
		std::shared_ptr<Mesh> m_mesh, m_mesh_quad;
		std::shared_ptr<Shader> m_ShaderMesh, m_ShaderSingleColor;
		std::shared_ptr<Shader> m_ShaderActive;
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

		// Stencil Testing
		bool m_b_Object_outlining;

	private:
		void RenderMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, glm::vec3 position, float scale);
	};

}

