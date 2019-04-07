#include "T04_NormalMap_05_Model.h"
#include "GLFW/glfw3.h"

namespace test {

	T04_NormalMap_05_Model::T04_NormalMap_05_Model()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_lightPos(glm::vec3(0.5f, +1.0f, 1.3f)),
		m_b_light_move_active(false),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(true),
		m_i_NormalMap_opt(0), m_i_NormalMap_rotation_opt(0),
		m_b_NorMap_diffuse(true), m_b_NorMap_specular(true)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.f, 0.f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, 0.f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);


		// Object Build from Vertex--------------------------------------------------------------------------
		#include "E00_cube_vertices.h"

		// Light
		m_VertexBuffer = std::make_unique<VertexBuffer>(cube_vertices_3v_3n_2t, 36 * (3 + 3 + 2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_lightVAO = std::make_unique<VertexArray>();
		m_lightVAO->AddBuffer(*m_VertexBuffer, layout);


		m_ShaderMesh = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/04_Normal_Mapping/S04_NormalMap_05_Model.Shader");
		m_ShaderLight = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/04_Normal_Mapping/S00_Color_01a_Light.Shader");


		// NEW Model----------------------------------------------------------------------------------------
		//m_model = std::make_unique<Model>("res/objects/nanosuit_reflection/nanosuit.obj");
		m_model = std::make_unique<Model>("res/objects/cyborg/cyborg.obj");
	}

	T04_NormalMap_05_Model::~T04_NormalMap_05_Model()
	{
	}

	void T04_NormalMap_05_Model::OnUpdate(float deltaTime)
	{
	}

	void T04_NormalMap_05_Model::OnRender(GLFWwindow* window)
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
		float inv_ratio_aspect = (float)m_framebufferWidth / (float)m_framebufferHeight;


		// Mesh Quad -----------------------------
		if (true)
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

			if (m_i_NormalMap_rotation_opt == 1)
				model = glm::rotate(model, glm::radians(-45.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			else if (m_i_NormalMap_rotation_opt == 2)
				model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
			else if (m_i_NormalMap_rotation_opt == 3)
				model = glm::rotate(model, glm::radians(-60.0f), glm::normalize(glm::vec3(1.0, 1.0, 0.0)));
			else if (m_i_NormalMap_rotation_opt == 4)
				model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			else if (m_i_NormalMap_rotation_opt == 5)
			{
				model = glm::rotate(model, glm::radians(m_quad_orientation.x), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
				model = glm::rotate(model, glm::radians(m_quad_orientation.y), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
				model = glm::rotate(model, glm::radians(m_quad_orientation.z), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
			}
			model = glm::scale(model, glm::vec3(.3));


			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);

			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// Material
			m_ShaderMesh->SetUniform1f("material.shininess", 32.0f);


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
			m_ShaderMesh->SetUniform3f("light.ambient", 0.1f, 0.1f, 0.1f);
			m_ShaderMesh->SetUniform3f("light.diffuse", .7f, .7f, .7f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 0.3f, 0.3f, 0.3f);

			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);

			m_ShaderMesh->SetUniform1i("u_i_NormalMap_opt", m_i_NormalMap_opt);
			m_ShaderMesh->SetUniform1i("u_i_half_Width", int(m_framebufferWidth / 2));

			m_ShaderMesh->SetUniform1i("u_b_NorMap_diffuse", m_b_NorMap_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_NorMap_specular", m_b_NorMap_specular);

			// Bypassed MESH
			// Draw MESH
			//m_mesh->Draw(m_ShaderMesh);
			m_model->Draw(m_ShaderMesh);



			// Shader Render for LIGHT
			m_ShaderLight->Bind();
			model = glm::mat4(1.0f);
			model = glm::translate(model, m_lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
			mvp = proj * view * model;
			glm::vec3 lightColor = glm::vec3(1.0f);
			m_ShaderLight->SetUniform3fv("u_lightColor", lightColor);
			m_ShaderLight->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_lightVAO, 36, *m_ShaderLight);
		}

	}

	void T04_NormalMap_05_Model::OnImGuiRender()
	{
		ImGui::Text("Normal Mapping 03 - Mesh2 & VertexTB");
		IMGUI_FPS;

		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);

		ImGui::Text("0 : None");
		ImGui::Text("1 : Normal Map");
		ImGui::Text("2 : Normal Map / None");
		ImGui::SliderInt("NM opt:", &m_i_NormalMap_opt, 0, 2);
		ImGui::Checkbox("Light move active", &m_b_light_move_active);
		ImGui::Checkbox("Nor.Map. diffuse", &m_b_NorMap_diffuse);
		ImGui::Checkbox("Nor.Map. specular", &m_b_NorMap_specular);

		ImGui::SliderInt("Normal rotation opt", &m_i_NormalMap_rotation_opt, 0, 5);

		ImGui::SliderFloat3("Quad orientation", &m_quad_orientation[0], -45, 45);
	}

	void T04_NormalMap_05_Model::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T04_NormalMap_05_Model::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T04_NormalMap_05_Model::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
