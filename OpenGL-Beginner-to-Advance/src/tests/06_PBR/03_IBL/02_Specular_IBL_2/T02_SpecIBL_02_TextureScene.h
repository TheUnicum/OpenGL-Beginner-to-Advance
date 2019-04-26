#pragma once

#include "tests/Test.h"

#include <iostream>

#include "Shader.h"
#include "Texture.h"

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

	class T02_SpecIBL_02_TextureScene : public Test
	{
	public:
		T02_SpecIBL_02_TextureScene();
		~T02_SpecIBL_02_TextureScene();

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
		std::unique_ptr<Mesh> m_mesh, m_Shere_mesh;
		std::shared_ptr<Shader> m_Shader;
		std::vector<std::shared_ptr<Texture>> msp_Textures;

		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		// PBR 
		// fill m_Shere_mesh
		void fill_sphere_mesh_vertices();

		int m_nrRows, m_nrColums;
		float m_spacing;
		std::vector<glm::vec3> m_lightPositions, m_lightColors;

		// pbr: HDR environment map
		std::shared_ptr<Texture> m_hdrTexture, m_envCubeMap;
		std::shared_ptr<Texture> msp_mTexture0; // test cube mesh
		std::shared_ptr<Shader> m_ShaderEquirectangularToCubemap;
		std::shared_ptr<Shader> m_ShaderBackground;

		// Framebuffer
		FrameBuffer m_fbo;// , m_irradianceFbo;
		int m_framebufferWidth, m_framebufferHeight;
		int m_hdrEnvMapCubeWidth, m_hdrEnvMapCubeHeight;
		void FramebufferSetup(int width, int height);

		glm::mat4 m_captureProjection;
		std::vector<glm::mat4> m_captureViews;

		// Irradiance CubeMap
		std::shared_ptr<Shader> m_ShaderIrradianceToCubemap;
		std::shared_ptr<Texture> m_irradianceCubeMap;
		int m_irradianceMapCubeWidth, m_irradianceMapCubeHeight;
		bool m_b_show_hdrEnvMapCube;	//m_b_irradianceLighting

		// Pre-filtered Cubemap
		std::shared_ptr<Shader> m_ShaderPreFilterToCubemap;
		std::shared_ptr<Texture> m_prefilteredCubeMap;
		int m_preFilterMapCubeWidth, m_preFilterMapCubeHeight;
		float m_f_preFilterMipMap_level;

		// Pre-computing the BRDF
		std::shared_ptr<Shader> m_ShaderbrdfToTexture2D;
		std::shared_ptr<Texture> m_brdfLUTTexture2D;
		int m_brdfLUTTMap2DWidth, m_brdfLUTTMap2DHeight;

		int m_i_select_Cube_Map;
		int m_i_sel_irradianceLighting;

		std::shared_ptr<Texture> m_TexAlbedo, m_TexNormal, m_TexMetallic, m_TexRoughness, m_TexAO;

		std::shared_ptr<Mesh> m_Shere_mesh_rusted_iron;
		std::shared_ptr<Mesh> m_Shere_mesh_gold;
		std::shared_ptr<Mesh> m_Shere_mesh_grass;
		std::shared_ptr<Mesh> m_Shere_mesh_plastic;
		std::shared_ptr<Mesh> m_Shere_mesh_wall;

	public:
		std::vector<std::shared_ptr<Texture>> return_mesh_sp_texture(const std::string& path);
	};
}

