#include "T03_Model_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T03_Model_01::T03_Model_01()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),

		m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(true), m_b_emission(true),
		m_b_light_move_active(false), m_b_cube_rotating_active(false), m_b_cube_scale_active(false),
		m_b_traspose_disable(false)
	{
		#include "E00_cube_vertices.h"

		m_camera = std::make_unique<Camera>(glm::vec3(-2.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.f, -20.f);

		m_VertexBuffer = std::make_unique<VertexBuffer>(cube_vertices_3v_3n_2t, 36 * (3 + 3 + 2) * sizeof(float));
		
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_lightVAO = std::make_unique<VertexArray>();
		m_lightVAO->AddBuffer(*m_VertexBuffer, layout);

		m_Shader = std::make_shared<Shader>("src/tests/03_Model_Loading/03_Model/S03_Model_01.Shader");
		m_lightShader = std::make_shared<Shader>("src/tests/03_Model_Loading/03_Model/S00_Color_01a_Light.Shader");

		// Textures
		m_Texture0 = std::make_unique<Texture>("res/textures/container2.png");
		m_Texture1 = std::make_unique<Texture>("res/textures/container2_specular.png");
		m_Texture2 = std::make_unique<Texture>("res/textures/matrix.jpg");

		// Initialize camera
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);


		// NEW mesh----------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : cube_vertices_3v_3n_2t)
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
		msp_mTexture0 = std::make_shared<Texture>("res/textures/container2.png", TextureType::DIFFUSE);
		msp_mTexture1 = std::make_shared<Texture>("res/textures/container2_specular.png", TextureType::SPECULAR);
		msp_mTexture2 = std::make_shared<Texture>("res/textures/matrix_2.jpg", TextureType::EMISSION);
		msp_Textures.push_back(msp_mTexture0);
		msp_Textures.push_back(msp_mTexture1);
		msp_Textures.push_back(msp_mTexture2);

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		m_ShaderMesh = std::make_unique<Shader>("src/tests/03_Model_Loading/03_Model/S03_Model_01_Mesh.Shader");
	}

	T03_Model_01::~T03_Model_01()
	{
	}

	void T03_Model_01::OnUpdate(float deltaTime)
	{
	}

	void T03_Model_01::OnRender(GLFWwindow* window)
	{
		if (m_b_depth_test_active != m_b_depth_test_active_i_1)
		{
			if (m_b_depth_test_active)
			{
				GLCall(glEnable(GL_DEPTH_TEST));
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

		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		float inv_ratio_aspect = 960.0f / 540.0f;
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);
			glm::mat4 mvp;
			Renderer renderer;

			view = m_camera->GetViewMatrix();
			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

			m_Shader->Bind();
			model = glm::mat4(1.0f);

			if (m_b_cube_rotating_active)
			{
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			if (m_b_cube_scale_active)
				model = glm::scale(model, glm::vec3(1.0f, 0.2f, 2.3f));

			mvp = proj * view * model;
			m_Shader->SetUniformMat4f("u_mvp", mvp);
			m_Shader->SetUniformMat4f("u_model", model);

			if (!m_b_traspose_disable)
				m_Shader->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
			else
				m_Shader->SetUniformMat3f("u_transInvers_model", model);

			// Material
			m_Texture0->Bind(0);
			m_Shader->SetUniform1i("material.diffuse", 0);
			m_Texture1->Bind(1);
			m_Shader->SetUniform1i("material.specular", 1);
			m_Texture2->Bind(2);
			m_Shader->SetUniform1i("material.emission", 2);
			//m_Shader->SetUniform3f("material.ambient", 1.0f, 0.5f, 0.31f);
			//m_Shader->SetUniform3f("material.diffuse", 1.0f, 0.5f, 0.31f);
			//m_Shader->SetUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
			m_Shader->SetUniform1f("material.shininess", 32.0f);

			// Light
			// change the light's position values over time (can be done anywhere in the render loop actually, 
			// but try to do it at least before using the light source positions)
			if (m_b_light_move_active)
			{
				m_lightPos.x = (1.0f + (float)sin(glfwGetTime()) * 2.0f);
				m_lightPos.y = (float)sin(glfwGetTime() / 2.0f) * 1.0f;
			}

			m_Shader->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_Shader->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			m_Shader->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_Shader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_Shader->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_Shader->SetUniform1i("u_b_ambient", m_b_ambient);
			m_Shader->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_Shader->SetUniform1i("u_b_specular", m_b_specular);
			m_Shader->SetUniform1i("u_b_emission", m_b_emission);

			renderer.Draw(*m_VAO, 36, *m_Shader);

			// Shader Render for LIGHT
			m_lightShader->Bind();
			model = glm::mat4(1.0f);
			model = glm::translate(model, m_lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			mvp = proj * view * model;
			glm::vec3 lightColor = glm::vec3(1.0f);
			m_lightShader->SetUniform3fv("u_lightColor", lightColor);
			m_lightShader->SetUniformMat4f("u_mvp", mvp);
			renderer.Draw(*m_lightVAO, 36, *m_lightShader);
		}

		// Mesh
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);
			glm::mat4 mvp;
			//Renderer renderer;

			view = m_camera->GetViewMatrix();
			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

			m_ShaderMesh->Bind();
			model = glm::mat4(1.0f);

			model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));

			if (m_b_cube_rotating_active)
			{
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}

			if (m_b_cube_scale_active)
				model = glm::scale(model, glm::vec3(1.0f, 0.2f, 2.3f));

			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);

			if (!m_b_traspose_disable)
				m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
			else
				m_ShaderMesh->SetUniformMat3f("u_transInvers_model", model);

			// Material
			m_ShaderMesh->SetUniform1f("material.shininess", 2.0f);

			// Light
			// change the light's position values over time (can be done anywhere in the render loop actually, 
			// but try to do it at least before using the light source positions)
			if (m_b_light_move_active)
			{
				m_lightPos.x = (1.0f + (float)sin(glfwGetTime()) * 2.0f);
				m_lightPos.y = (float)sin(glfwGetTime() / 2.0f) * 1.0f;
			}

			m_ShaderMesh->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_ShaderMesh->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			m_ShaderMesh->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);
			m_ShaderMesh->SetUniform1i("u_b_emission", m_b_emission);

			// Draw MESH
			m_mesh->Draw(m_ShaderMesh);
		}
	}

	void T03_Model_01::OnImGuiRender()
	{
		ImGui::Text("Emission maps");
		
		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);
		ImGui::Checkbox("Emission Light", &m_b_emission);

		ImGui::Text("Exercises A - motion options");
		ImGui::Checkbox("Light move active", &m_b_light_move_active);
		ImGui::Checkbox("Cube rotation active", &m_b_cube_rotating_active);
		ImGui::Checkbox("Cube scale active", &m_b_cube_scale_active);
		ImGui::Text("Exercises C - Phong Vs Gouraud");
		ImGui::Checkbox("Transpose matrix disable", &m_b_traspose_disable);
	}

	void T03_Model_01::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T03_Model_01::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T03_Model_01::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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


