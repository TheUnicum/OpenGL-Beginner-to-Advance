#include "T02_Gamma_Correction_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T02_Gamma_Correction_01::T02_Gamma_Correction_01()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),

		m_lightPos(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(true), m_b_emission(true),
		m_b_blinn(false), m_b_disable_adapt_shininess(false), m_b_specular_texture(false),
		m_b_gamma(false), m_b_quadratic(false), m_f_gamma(2.2)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.f, 0.f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, +0.0f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW mesh-  index quad----------------------------------------------------------------------------------
		std::vector<Vertex> vertices_quad_3v_3n_2t;
		std::vector<unsigned int> vec_quad_indices;

		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : planeVerticesAL_3v_3n_2t)
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
				vertices_quad_3v_3n_2t.emplace_back(temp_pos, temp_norm, temp_texC);
				i = -1; // Restart counter...
				break;
			default:
				break;
			}
			i++;
		}
		
		msp_mTextureFloor = std::make_shared<Texture>("res/textures/wood.png", TextureType::DIFFUSE);
		msp_mTextures1.push_back(msp_mTextureFloor);
		m_mesh_quad = std::make_unique<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_mTextures1);

		m_ShaderMesh = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/02_Gamma_Correction/S02_Gamma_Correction_01.Shader");


		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T02_Gamma_Correction_01::~T02_Gamma_Correction_01()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T02_Gamma_Correction_01::OnUpdate(float deltaTime)
	{
	}

	void T02_Gamma_Correction_01::OnRender(GLFWwindow* window)
	{
		if (m_b_depth_test_active != m_b_depth_test_active_i_1)
		{
			if (m_b_depth_test_active)
			{
				// Remember to glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
				GLCall(glEnable(GL_DEPTH_TEST));
				//GLCall(glDepthMask(GL_FALSE)); //OpenGL allows us to disable writing to the depth buffer by setting its depth mask to GL_FALSE
			}
			else
			{
				GLCall(glDisable(GL_DEPTH_TEST));
			}
			m_b_depth_test_active_i_1 = m_b_depth_test_active;
		}


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

		// Disable blending for manual discard-----------------------------
		GLCall(glDisable(GL_BLEND));

		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		float inv_ratio_aspect = 960.0f / 540.0f;

		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

		{	// Floor
			m_ShaderMesh->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);

			// lighting info
			// -------------
			glm::vec3 lightPositions[] = {
				glm::vec3(-3.0f, 0.0f, 0.0f),
				glm::vec3(-1.0f, 0.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(3.0f, 0.0f, 0.0f)
			};

			glm::vec3 lightAmbient[] = {
				glm::vec3(0.01f),
				glm::vec3(0.01f),
				glm::vec3(0.01f),
				glm::vec3(0.01f)
			};
			glm::vec3 lightDiffuse[] = {
				glm::vec3(1.0f),
				glm::vec3(1.0f),
				glm::vec3(1.0f),
				glm::vec3(1.0f)
			};
			glm::vec3 lightSpecular[] = {
				glm::vec3(1.0f),
				glm::vec3(1.0f),
				glm::vec3(1.0f),
				glm::vec3(1.0f)
			};

			// Alternative mode: updated Shader class:
			//m_ShaderMesh->SetUniform3fv("lightPositions", 4, lightPositions[0]);
			//m_ShaderMesh->SetUniform3fv("lightAmbient", 4, lightAmbient[0]);


			for (int i = 0; i < 4; i++)
			{
				m_ShaderMesh->SetUniform3fv("light[" + std::to_string(i) + "].position", lightPositions[i]);
				m_ShaderMesh->SetUniform3fv("light[" + std::to_string(i) + "].ambient", lightAmbient[i]);
				m_ShaderMesh->SetUniform3fv("light[" + std::to_string(i) + "].diffuse", lightDiffuse[i]);
				m_ShaderMesh->SetUniform3fv("light[" + std::to_string(i) + "].specular", lightSpecular[i]);
			}

			if (m_b_blinn && !m_b_disable_adapt_shininess)
				m_ShaderMesh->SetUniform1f("material.shininess", 64.0f);
			else
				m_ShaderMesh->SetUniform1f("material.shininess", 32.0f);

			m_ShaderMesh->SetUniform1i("u_b_gamma", m_b_gamma);
			m_ShaderMesh->SetUniform1f("u_f_gamma", m_f_gamma);
			m_ShaderMesh->SetUniform1i("u_b_quadratic", m_b_quadratic);


			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);

			m_ShaderMesh->SetUniform1i("u_b_specular_texture", m_b_specular_texture);

			m_ShaderMesh->SetUniform1i("u_b_blinn", m_b_blinn);

			//MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);

			m_mesh_quad->Draw(m_ShaderMesh);
		}
	}

	void T02_Gamma_Correction_01::OnImGuiRender()
	{
		ImGui::Text("Face culling!");
		IMGUI_FPS;

		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text("");
		ImGui::Checkbox("Blinn-Phong Light", &m_b_blinn);
		ImGui::Checkbox("Disable adapt shininess", &m_b_disable_adapt_shininess);
		ImGui::Checkbox("Active Specular texture", &m_b_specular_texture);

		ImGui::Text("");
		ImGui::Checkbox("Gamma correction", &m_b_gamma);
		ImGui::SliderFloat("val:", &m_f_gamma, 1.5f, 3.0f);
		ImGui::Checkbox("Attenuatic Quadratic", &m_b_quadratic);
		ImGui::Text("");

		ImGui::Text("");
		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);
		//ImGui::Checkbox("Emission Light", &m_b_emission);

		ImGui::Text("");
		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T02_Gamma_Correction_01::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T02_Gamma_Correction_01::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T02_Gamma_Correction_01::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
}


