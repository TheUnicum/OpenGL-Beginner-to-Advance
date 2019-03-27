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

	class T06_Cubemaps_02_Reflection : public Test
	{
	public:
		T06_Cubemaps_02_Reflection();
		~T06_Cubemaps_02_Reflection();

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
		std::shared_ptr<Shader> m_ShaderMesh;
		std::shared_ptr<Texture> msp_mTexture0;
		std::vector<std::shared_ptr<Texture>> msp_Textures;

		// MODEL
		std::unique_ptr<Model> m_model;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		// Cubomap Texture
		Texture m_cubemapTexture;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<VertexArray> m_VAO;
		std::shared_ptr<Shader> m_ShaderSky;
		bool m_b_disable_cubemap_lock, m_b_optimization;
		bool m_b_cube_rotating_active, m_b_Model_rotating_active;
		bool m_b_cube_boxtexture;
		float m_f_model_scale;
		void DrawCube(glm::mat4 model, glm::mat4 proj, glm::mat4 view);
		void DrawModel(glm::mat4 model, glm::mat4 proj, glm::mat4 view);

		int m_framebufferWidth, m_framebufferHeight;
	};

}

