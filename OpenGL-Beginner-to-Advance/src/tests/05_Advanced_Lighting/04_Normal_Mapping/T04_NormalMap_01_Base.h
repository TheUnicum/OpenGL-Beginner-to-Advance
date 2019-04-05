#pragma once

#include "tests/Test.h"

#include <iostream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include "Renderer.h"

#include <memory>

//#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"	

#include "Mesh.h"

namespace test {

	class T04_NormalMap_01_Base : public Test
	{
	public:
		T04_NormalMap_01_Base();
		~T04_NormalMap_01_Base();

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
		bool m_Fix_to_ground, m_Fix_to_ground_i_1;
		bool m_mouse_lock;
		bool m_key_pressed = false;

		int m_framebufferWidth, m_framebufferHeight;

		// lighting
		glm::vec3 m_lightPos;
		bool m_b_light_move_active;
		bool m_b_ambient, m_b_diffuse, m_b_specular;

		// VAO Light
		std::unique_ptr<VertexArray> m_VAO, m_lightVAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;

		// MESH members , cube & quad
		std::unique_ptr<Mesh> m_mesh;
		std::shared_ptr<Shader> m_ShaderMesh, m_ShaderLight;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTexture1, msp_mTexture2;
		std::vector<std::shared_ptr<Texture>> msp_Textures;

		int m_i_NormalMap_opt;
		bool m_b_NorMap_diffuse, m_b_NorMap_specular;

	};

}

