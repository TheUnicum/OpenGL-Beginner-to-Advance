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

	class T03_Mesh_02_ibo : public Test
	{
	public:
		T03_Mesh_02_ibo();
		~T03_Mesh_02_ibo();

		void OnUpdate(float deltaTime) override;
		void OnRender(GLFWwindow* window) override;
		void OnImGuiRender() override;
		void OnProcessInput(GLFWwindow* window, float deltaTime) override;
		// callbacks functions
		void framebuffer_size_callback(GLFWwindow* window, int width, int height) override;
		void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	private:
		std::unique_ptr<VertexArray> m_VAO, m_lightVAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Texture> m_Texture0, m_Texture1, m_Texture2;
		std::shared_ptr<Shader> m_Shader, m_lightShader;
		std::unique_ptr<Renderer> m_Renderer;
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

		// lighting
		glm::vec3 m_lightPos;
		bool m_b_ambient, m_b_diffuse, m_b_specular, m_b_emission;

		// 
		bool m_b_light_move_active;
		bool m_b_cube_rotating_active;
		bool m_b_cube_scale_active;
		bool m_b_gouraud_shading_active;

		// MESH members , cube & quad
		std::unique_ptr<Mesh> m_mesh, m_mesh_quad;
		std::shared_ptr<Shader> m_ShaderMesh;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTexture1, msp_mTexture2;
		std::vector<std::shared_ptr<Texture>> msp_Textures;

		bool m_b_draw_object;
		bool m_b_draw_mesh;
		bool m_b_draw_quad;
		bool m_b_draw_quad_glDrawArrays;
	};

}
