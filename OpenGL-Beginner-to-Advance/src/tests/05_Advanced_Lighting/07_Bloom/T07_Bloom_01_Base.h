#pragma once

#include "tests/Test.h"

#include <iostream>

#include "Shader.h"
#include "Texture.h"
#include "RenderBuffer.h"
#include "FrameBuffer.h"

#include <memory>

//#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"	

#include "Mesh.h"
#include "Model.h"

namespace test {

	class T07_Bloom_01_Base : public Test
	{
	public:
		T07_Bloom_01_Base();
		~T07_Bloom_01_Base();

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

		// MESH members
		//std::unique_ptr<Mesh> m_mesh, m_mesh_quad, m_mesh_grass, m_mesh_grass_Clamped;
		std::unique_ptr<Mesh> m_mesh_floor, m_mesh_cube;
		std::shared_ptr<Shader> m_ShaderMesh, m_ShaderLight;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTextureCube;
		std::vector<std::shared_ptr<Texture>> msp_Textures, msp_TextureCube;
		// Floor
		std::shared_ptr<Texture> msp_mTextureFloor;
		std::vector<std::shared_ptr<Texture>> msp_mTextures2;

		// MODEL
		//std::unique_ptr<Model> m_model;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		// Framebuffers
		std::unique_ptr<Shader> m_ShaderScreen;
		std::unique_ptr<VertexBuffer> m_ScreenVBO;
		std::unique_ptr<VertexArray> m_ScreenVAO;

	private:
		int m_framebufferWidth, m_framebufferHeight;
		void FramebufferSetup(int width, int height);

		// Abstracting
		Texture m_fbp_texture;
		RenderBuffer m_fbo_rbuffer;
		FrameBuffer m_fbo;


		int m_i_Tone_mapping_Type;
		float m_f_hdr_expose;
		bool m_b_disable_Floating_point_fb, m_b_disable_Floating_point_fb_i_1;

		void DrawCube(std::shared_ptr<Shader> shader, glm::mat4 proj, glm::mat4 view, glm::mat4 model);

	};

}

