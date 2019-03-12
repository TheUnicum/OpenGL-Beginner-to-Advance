#include "T04_LightingMaps_02_Exercises.h"
#include "GLFW/glfw3.h"

namespace test {

	T04_LightingMaps_02_Exercises::T04_LightingMaps_02_Exercises()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),

		m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(true),
		m_b_light_move_active(false), m_b_cube_rotating_active(false), m_b_cube_scale_active(false),
		m_b_traspose_disable(false),
		m_b_invert_specular_map(false), m_b_colored_specular_map(false), m_b_active_color_light(false)
	{
		#include "E00_cube_vertices.h"

		m_camera = std::make_unique<Camera>(glm::vec3(-2.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.f, -20.f);

		m_VertexBuffer = std::make_unique<VertexBuffer>(pyramid_vertices_3v_3n_2t, 36 * (3 + 3 + 2) * sizeof(float));
		
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_lightVAO = std::make_unique<VertexArray>();
		m_lightVAO->AddBuffer(*m_VertexBuffer, layout);

		m_Shader = std::make_shared<Shader>("src/tests/02_Lighting/04_LightingMaps/S04_LightingMaps_02_Exercises.Shader");
		m_lightShader = std::make_shared<Shader>("src/tests/02_Lighting/04_LightingMaps/S00_Color_01a_Light.Shader");

		// Textures
		m_Texture0 = std::make_unique<Texture>("res/textures/container2.png");
		m_Texture1 = std::make_unique<Texture>("res/textures/container2_specular.png");
		m_Texture2 = std::make_unique<Texture>("res/textures/lighting_maps_specular_color.png");

		// Initialize camera
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);
	}

	T04_LightingMaps_02_Exercises::~T04_LightingMaps_02_Exercises()
	{
	}

	void T04_LightingMaps_02_Exercises::OnUpdate(float deltaTime)
	{
	}

	void T04_LightingMaps_02_Exercises::OnRender(GLFWwindow* window)
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

			if (!m_b_colored_specular_map)
				m_Texture1->Bind(1);
			else
				m_Texture2->Bind(1);
			m_Shader->SetUniform1i("material.specular", 1);

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

			// Light Color
			glm::vec3 ambientColor, diffuseColor, lightColor;
			if (m_b_active_color_light)
			{
				lightColor.x = (float)sin(glfwGetTime() * 2.0f);
				lightColor.y = (float)sin(glfwGetTime() * 0.7f);
				lightColor.z = (float)sin(glfwGetTime() * 1.3f);

				diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
				ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
			}
			else
			{
				lightColor = glm::vec3(1.0f);
				ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
				diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
			}

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_Shader->SetUniform3fv("light.ambient", ambientColor); //0.2f, 0.2f, 0.2f);
			m_Shader->SetUniform3fv("light.diffuse", diffuseColor); //0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_Shader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_Shader->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_Shader->SetUniform1i("u_b_ambient", m_b_ambient);
			m_Shader->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_Shader->SetUniform1i("u_b_specular", m_b_specular);

			// Exercises Lighting maps
			m_Shader->SetUniform1i("m_b_invert_specular_map", m_b_invert_specular_map);

			renderer.Draw(*m_VAO, 36, *m_Shader);

			// Shader Render for LIGHT
			m_lightShader->Bind();
			model = glm::mat4(1.0f);
			model = glm::translate(model, m_lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			mvp = proj * view * model;
			m_lightShader->SetUniformMat4f("u_mvp", mvp);
			m_lightShader->SetUniform3fv("u_lightColor", lightColor);
			renderer.Draw(*m_lightVAO, 36, *m_lightShader);
		}
	}

	void T04_LightingMaps_02_Exercises::OnImGuiRender()
	{
		ImGui::Text("Exercises...");
		
		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);

		ImGui::Text("Exercises A - motion options");
		ImGui::Checkbox("Light move active", &m_b_light_move_active);
		ImGui::Checkbox("Cube rotation active", &m_b_cube_rotating_active);
		ImGui::Checkbox("Cube scale active", &m_b_cube_scale_active);
		ImGui::Text("Exercises C - Phong Vs Gouraud");
		ImGui::Checkbox("Transpose matrix disable", &m_b_traspose_disable);

		ImGui::Text("Exercises");
		ImGui::Checkbox("Inverting specular map", &m_b_invert_specular_map);
		ImGui::Checkbox("Active COLORED spec. map", &m_b_colored_specular_map);
		ImGui::Checkbox("Active colored Light", &m_b_active_color_light);
	}

	void T04_LightingMaps_02_Exercises::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T04_LightingMaps_02_Exercises::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T04_LightingMaps_02_Exercises::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
