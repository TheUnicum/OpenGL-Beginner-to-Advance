#include "T01_Diffuse_irradiance_02.h"
#include "GLFW/glfw3.h"

namespace test {

	T01_Diffuse_irradiance_02::T01_Diffuse_irradiance_02()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_nrRows(7), m_nrColums(7), m_spacing(2.5),

		m_hdrEnvMapCubeWidth(512), m_hdrEnvMapCubeHeight(512),
		m_irradianceMapCubeWidth(32), m_irradianceMapCubeHeight(32),
		m_b_irradianceCubeMap(false), m_b_irradianceLighting(false), m_b_show_hdrEnvMapCube(false)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 30.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"


		// NEW CUBE mesh----------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : cube_Unitario_vertices_3v_3n_2t)
		{
			switch (i)
			{
			case 0:	// Position
				temp_pos.x = val;
				break;
			case 1:
				temp_pos.y = val;
				break;
			case 2:
				temp_pos.z = val;
				break;
			case 3:	// Normal
				temp_norm.x = val;
				break;
			case 4:
				temp_norm.y = val;
				break;
			case 5:
				temp_norm.z = val;
				break;
			case 6:	// TexCoords
				temp_texC.x = val;
				break;
			case 7:
				temp_texC.y = val;
				vertices_3v_3n_2t.emplace_back(temp_pos, temp_norm, temp_texC);
				i = -1; // Restart counter...
				break;
			default:
				break;
			}
			i++;
		}
		// To check the correct format of vertec:position/normal/textCoords
		//for (auto& ele : vertices_3v_3n_2t)
		//	std::cout << ele << std::endl;

		// Textures - Shared pointer
		msp_mTexture0 = std::make_shared<Texture>("res/textures/wood.png", TextureType::DIFFUSE); // ontainer2.png
		msp_Textures.push_back(msp_mTexture0);

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);

		m_Shader = std::make_unique<Shader>("src/tests/06_PBR/03_IBL/01_Diffuse_irradiance/S01_Diffuse_irradiance_02.Shader");
		m_ShaderEquirectangularToCubemap = std::make_unique<Shader>(
			"src/tests/06_PBR/03_IBL/01_Diffuse_irradiance/S01_Diffuse_irradiance_02_EquirectangularToCubemap.Shader");
		m_ShaderIrradianceToCubemap = std::make_unique<Shader>(
			"src/tests/06_PBR/03_IBL/01_Diffuse_irradiance/S01_Diffuse_irradiance_02_IrradianceCubemap.Shader");
		m_ShaderBackground = std::make_unique<Shader>("src/tests/06_PBR/03_IBL/01_Diffuse_irradiance/S01_Diffuse_irradiance_02_Background.Shader");


		// lights
		// ------
		m_lightPositions.push_back(glm::vec3(-10.0f, +10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(+10.0f, +10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(-10.0f, -10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(+10.0f, -10.0f, +10.f));

		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

		// Sphere
		//--------------------------------------------------------------
		fill_sphere_mesh_vertices();



		// pbr: setup framebuffer
		// ----------------------
		FramebufferSetup(m_hdrEnvMapCubeWidth, m_hdrEnvMapCubeHeight);

		// pbr: load the HDR environment map
		m_hdrTexture = std::make_shared<Texture>();
		m_hdrTexture->LoadHdr("res/textures/hdr/newport_loft.hdr");

		// pbr: setup cubemap to render to and attach to framebuffer
		m_envCubeMap = std::make_shared<Texture>();
		m_envCubeMap->InitializeCube(m_hdrEnvMapCubeWidth, m_hdrEnvMapCubeHeight, GL_RGB16F, GL_RGB,
			GL_LINEAR, GL_LINEAR,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale
		m_irradianceCubeMap = std::make_shared<Texture>();
		m_irradianceCubeMap->InitializeCube(m_irradianceMapCubeWidth, m_irradianceMapCubeHeight, GL_RGB16F, GL_RGB,
			GL_LINEAR, GL_LINEAR,
			GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// pbr: set up projection and view matrices for capturing data onto the 6 cubemap dace direcitons
		m_captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		if (m_captureViews.size() == 0)
		{
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_captureViews.push_back(glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
		}

		// pbr:  convert HDR equirectangular environment map to cubemap equivalent
		// -----------------------------------------------------------------------
		m_ShaderEquirectangularToCubemap->Bind();
		m_hdrTexture->Bind(1);
		m_ShaderEquirectangularToCubemap->SetUniform1i("equirectangularMap", 1);

		glViewport(0, 0, m_hdrEnvMapCubeWidth, m_hdrEnvMapCubeHeight);	// don't forget to configure the viewport to the capture dimensions.
		m_fbo.Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 mvp = m_captureProjection * m_captureViews[i] * model;
			m_ShaderEquirectangularToCubemap->SetUniformMat4f("u_mvp", mvp);
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubeMap->GetID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_mesh->Draw(m_ShaderEquirectangularToCubemap);
		}
		m_fbo.Unbind();

		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		// ----------------------------------------------------------------------------
		m_ShaderIrradianceToCubemap->Bind();
		m_envCubeMap->Bind(2);
		m_ShaderIrradianceToCubemap->SetUniform1i("environmentMap", 2);

		glViewport(0, 0, m_irradianceMapCubeWidth, m_irradianceMapCubeHeight);	// 0, 0, 32, 32
		
		// pbr: setup framebuffer
		// ----------------------
		FramebufferSetup(m_irradianceMapCubeWidth, m_irradianceMapCubeHeight);
		m_fbo.Bind();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glm::mat4 model = glm::mat4(1.0f);
			glm::mat4 mvp = m_captureProjection * m_captureViews[i] * model;
			m_ShaderIrradianceToCubemap->SetUniformMat4f("u_mvp", mvp);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceCubeMap->GetID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_mesh->Draw(m_ShaderIrradianceToCubemap);
		}
		m_fbo.Unbind();


		// then before rendering, configure the viewport to the original framebuffer's screen dimensions
		//glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
		glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T01_Diffuse_irradiance_02::~T01_Diffuse_irradiance_02()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T01_Diffuse_irradiance_02::OnUpdate(float deltaTime)
	{
	}

	void T01_Diffuse_irradiance_02::OnRender(GLFWwindow* window)
	{
		if (m_mouse_disable != m_mouse_disable_i_1)
		{
			if (m_mouse_disable)
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			//std::cout << "Enable changed" << std::endl;
			m_mouse_disable_i_1 = m_mouse_disable;
		}

		// VSync
		if (m_b_VSync_disabled != m_b_VSync_disabled_i_1)
		{
			glfwSwapInterval(m_b_VSync_disabled ? 0 : 1);
			m_b_VSync_disabled_i_1 = m_b_VSync_disabled;
		}


		// Depth test
		GLCall(glEnable(GL_DEPTH_TEST));

		// Disable blending for manual discard-----------------------------
		//GLCall(glDisable(GL_BLEND));

		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		float inv_ratio_aspect = (float)m_framebufferWidth / (float)m_framebufferHeight;

		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

		{
			m_Shader->Bind();

			//m_Shader->SetUniform3fv("u_albedo", glm::vec3(0.5, 0.0f, 0.0f));
			glm::vec3 albedo = glm::vec3(0.5, 0.0f, 0.0f);
			m_Shader->SetUniform3fv("u_albedo", albedo);
			m_Shader->SetUniform1f("u_ao", 1.0f);

			// View position
			m_Shader->SetUniform3fv("u_camPos", m_camera->GetCamPosition());

			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_Shader->SetUniform1i("u_b_irradianceLighting", m_b_irradianceLighting);
			if (m_b_irradianceLighting)
			{
				m_irradianceCubeMap->Bind(3);
				m_Shader->SetUniform1i("irradianceMap", 3);
			}

			// render rows* columns number of spheres with varying metallic/roughness values scaled by rows and columns respectively
			for (int row = 0; row < m_nrRows; ++row)
			{
				m_Shader->SetUniform1f("u_metallic", (float)row / (float)m_nrRows);
				for (int col = 0; col < m_nrColums; ++col)
				{
					// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surface (roughtness of 0.0) tend to look a bit off
					// on direct lighting
					m_Shader->SetUniform1f("u_roughtness", glm::clamp((float)col / (float)m_nrColums, 0.05f, 1.0f));
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (m_nrColums / 2)) * m_spacing,
						(row - (m_nrRows / 2)) * m_spacing,
						0.0f
					));
					mvp = proj * view * model;
					m_Shader->SetUniformMat4f("u_model", model);
					m_Shader->SetUniformMat4f("u_mvp", mvp);
					m_Shere_mesh->Draw(m_Shader, false, GL_TRIANGLE_STRIP);
				}
			}

			// render light source (simply re-render sphere at light positions)
			// this looks a bit off as we use the same shader, but it'll make their positions obvious and
			// keeps the codeprint small.
			for (unsigned int i = 0; i < m_lightPositions.size(); ++i)
			{
				glm::vec3 newPos = m_lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);
				newPos = m_lightPositions[i]; 
				m_Shader->SetUniform3fv("u_lightPositions[" + std::to_string(i) + "]", newPos);
				m_Shader->SetUniform3fv("u_lightColors[" + std::to_string(i) + "]", m_lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				mvp = proj * view * model;
				m_Shader->SetUniformMat4f("u_model", model);
				m_Shader->SetUniformMat4f("u_mvp", mvp);
				m_Shere_mesh->Draw(m_Shader, false, GL_TRIANGLE_STRIP);
			}

			if (m_b_show_hdrEnvMapCube)
			{
				// Render a cube with HDR image
				m_ShaderEquirectangularToCubemap->Bind();
				model = glm::mat4(1.0f);
				model = scale(model, glm::vec3(4.0f, 2.0f, 2.0f));
				mvp = proj * view * model;
				m_ShaderEquirectangularToCubemap->SetUniformMat4f("u_model", model);
				m_ShaderEquirectangularToCubemap->SetUniformMat4f("u_mvp", mvp);
				m_hdrTexture->Bind(1);
				m_ShaderEquirectangularToCubemap->SetUniform1i("equirectangularMap", 1);
				m_mesh->Draw(m_ShaderEquirectangularToCubemap);
			}

			// render skybox (render as last to prevent overdraw
			glDepthFunc(GL_LEQUAL);
			m_ShaderBackground->Bind();
			m_ShaderBackground->SetUniformMat4f("u_proj", proj);
			m_ShaderBackground->SetUniformMat4f("u_view", view);
			if (!m_b_irradianceCubeMap)
				m_envCubeMap->Bind(2);
			else
				m_irradianceCubeMap->Bind(2);
			m_ShaderBackground->SetUniform1i("u_environmentMap", 2);
			m_mesh->Draw(m_ShaderBackground);
		}
	}

	void T01_Diffuse_irradiance_02::OnImGuiRender()
	{
		ImGui::Text("GLSL's built-in variables");
		ImGui::Text("Uniform buffer objects");
		IMGUI_FPS;

		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Irradiance Map", &m_b_irradianceCubeMap);
		ImGui::Checkbox("Irradiance Lighting", &m_b_irradianceLighting);
		ImGui::Checkbox("Show HDR env.Map", &m_b_show_hdrEnvMapCube);

		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T01_Diffuse_irradiance_02::OnProcessInput(GLFWwindow * window, float deltaTime)
	{
		glm::vec3 direction(0.0f);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			direction.z -= 1;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			direction.z += 1;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			direction.x -= 1;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			direction.x += 1;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			direction.y += 1;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			direction.y -= 1;
		direction *= 5;
		m_camera->ProcessKeyboard(direction, deltaTime);

		// Mouse pressed
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			if (!m_key_pressed)
				m_mouse_disable = !m_mouse_disable;
			m_key_pressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		{
			if (!m_key_pressed)
				m_mouse_lock = !m_mouse_lock;
			m_key_pressed = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			if (!m_key_pressed)
				m_camera->SetFixToGround(!m_camera->GetFixToGround());
			m_key_pressed = true;
		}
		else
			m_key_pressed = false;
	}

	void T01_Diffuse_irradiance_02::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
		FramebufferSetup(m_hdrEnvMapCubeWidth, m_hdrEnvMapCubeHeight);
	}

	void T01_Diffuse_irradiance_02::mouse_callback(GLFWwindow * window, double xpos, double ypos)
	{
		//std::cout << xpos << " " << ypos << std::endl;

		if (m_b_firstMouse)
		{
			//m_camera->ResetYawPitch();
			m_lastX = xpos;
			m_lastY = ypos;
			m_b_firstMouse = false;
		}

		float xoffset = xpos - m_lastX;
		float yoffset = -(ypos - m_lastY);
		m_lastX = xpos;
		m_lastY = ypos;
		if (!m_mouse_lock)
			m_camera->ProcessMouseMovement(xoffset, yoffset);
	}

	void T01_Diffuse_irradiance_02::fill_sphere_mesh_vertices()
	{
		unsigned int indexCount;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;


		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();






		std::vector<Vertex> vertices_sphere_3v_3n_2t;
		for (int i = 0; i < positions.size(); i++)
			vertices_sphere_3v_3n_2t.emplace_back(positions[i], normals[i], uv[i]);

		m_Shere_mesh = std::make_unique<Mesh>(vertices_sphere_3v_3n_2t, indices, msp_Textures);
	}

	void T01_Diffuse_irradiance_02::FramebufferSetup(int width, int height)
	{
		if (!m_fbo.Initialize(width, height))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;
	}

}


