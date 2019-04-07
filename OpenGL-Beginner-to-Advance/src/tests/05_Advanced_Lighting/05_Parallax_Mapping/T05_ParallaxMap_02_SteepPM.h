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

	class T05_ParallaxMap_02_SteepPM : public Test
	{
	public:
		T05_ParallaxMap_02_SteepPM();
		~T05_ParallaxMap_02_SteepPM();

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
		std::shared_ptr<Shader> m_ShaderMesh, m_ShaderLight;

		std::unique_ptr<Mesh> m_mesh_Wall;
		std::shared_ptr<Texture> msp_mTexture0, msp_mTexture1, msp_mTexture2;
		std::vector<std::shared_ptr<Texture>> msp_Textures_Wall;

		std::unique_ptr<Mesh> m_mesh_Wood;
		std::shared_ptr<Texture> msp_mTexture_W0, msp_mTexture_W1, msp_mTexture_W2;
		std::vector<std::shared_ptr<Texture>> msp_Textures_Wood;

		int m_i_NormalMap_opt, m_i_NormalMap_rotation_opt;
		bool m_b_NorMap_diffuse, m_b_NorMap_specular;

		float* m_quadVertices_3p_3n_2t_3T_3B;
		void ManualCalculationTangentAndBitanget(float* ptr_quad);

		glm::vec3 m_quad_orientation;
		float m_f_heightScale;
		bool m_b_mesh_Wood, m_b_discardFragment_outside, m_b_OffsetLimit;

		// Steep Parallax Mapping
		bool m_b_steep_PM, m_b_numLayerAuto, m_b_linear_interpolation;
		int m_i_numLayer_PM;
	};

}

