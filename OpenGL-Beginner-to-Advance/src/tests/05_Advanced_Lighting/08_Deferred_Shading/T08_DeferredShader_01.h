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

	class T08_DeferredShader_01 : public Test
	{
	public:
		T08_DeferredShader_01();
		~T08_DeferredShader_01();

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
		std::unique_ptr<Mesh> m_mesh, m_mesh_quad, m_mesh_grass, m_mesh_grass_Clamped;
		std::shared_ptr<Shader> m_ShaderMesh;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTexture1, msp_mTexture2;
		std::vector<std::shared_ptr<Texture>> msp_Textures;
		// Floor
		std::shared_ptr<Texture> msp_mTextureFloor;
		std::vector<std::shared_ptr<Texture>> msp_mTextures2;

		// MODEL
		std::unique_ptr<Model> m_model;
		std::vector<glm::vec3> m_objectPositions;

		std::vector<glm::vec3> m_lightPositions;
		std::vector<glm::vec3> m_lightColors;
		unsigned int NR_LIGHTS = 32;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		// Framebuffers
		std::shared_ptr<Shader> m_ShaderScreen;
		std::shared_ptr<Shader> m_ShaderGeometryPass, m_ShaderLightingPass, m_ShaderLightBox;
		std::unique_ptr<VertexBuffer> m_ScreenVBO;
		std::unique_ptr<VertexArray> m_ScreenVAO;

		int m_i_postproc_effect;
	private:
		int m_framebufferWidth, m_framebufferHeight;
		void FramebufferSetup(int width, int height);

		// Abstracting
		Texture m_fbp_texture;
		RenderBuffer m_fbo_rbuffer;
		FrameBuffer m_fbo, m_fbo_gBuffer;

		int m_i_deferred_Shader_debug;

		void RenderFramebufferTexture(glm::vec3 scale, glm::vec3 position, int index, int color_attch_Nr);
	};

}

