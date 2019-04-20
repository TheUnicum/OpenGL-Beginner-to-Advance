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

	class T02_PM_01_Base : public Test
	{
	public:
		T02_PM_01_Base();
		~T02_PM_01_Base();

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
		std::unique_ptr<Mesh> m_mesh_quad, m_mesh_cube;
		std::shared_ptr<Shader> m_ShaderMesh;
		std::shared_ptr<Shader> m_ShaderB_DepthMap, m_ShaderB2_DepthCubeMap;

		std::shared_ptr<Texture> msp_mTextureFloor, msp_mTextureCube;
		std::vector<std::shared_ptr<Texture>> msp_mTextures1, msp_mTextures2;
		bool m_b_blinn, m_b_disable_adapt_shininess, m_b_specular_texture;

		// lighting
		glm::vec3 m_lightPos, m_lightDirection;
		bool m_b_ambient, m_b_diffuse, m_b_specular, m_b_emission;
		bool m_b_rotating;

		void SetLighting(std::shared_ptr<Shader> shader);
		void DrawCube(std::shared_ptr<Shader> shader, glm::mat4& model, glm::mat4& view, glm::mat4& proj, bool rotating, bool depthmap = false);

		// Framebuffer
		FrameBuffer m_fbo, m_fboCube;
		int m_framebufferWidth, m_framebufferHeight;
		void FramebufferSetup(int width, int height);

		std::shared_ptr<Shader> m_ShaderScreen, m_ShaderFinal, m_ShaderCubeFinal, m_ShaderLight;
		std::unique_ptr<VertexBuffer> m_ScreenVBO;
		std::unique_ptr<VertexArray> m_ScreenVAO;

		unsigned int m_SHADOW_WIDTH, m_SHADOW_HEIGHT;
		int m_i_DP_Visualizing_opt;

		bool m_b_gamma;
		float m_f_gamma;

		// Point Shadows
		void RenderScene(std::shared_ptr<Shader> shader, glm::mat4& view, glm::mat4& proj, bool only_model = false);
		void RenderScene(std::shared_ptr<Shader> shader);
		int m_i_viewShadow_index;
		glm::mat4 m_shadowProj;
		std::vector<glm::mat4> m_shadowView;
		float m_far_plane;
		int m_i_PCF_type;

		// VAO Light
		std::unique_ptr<VertexArray> m_VAO, m_lightVAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;

	};

}

