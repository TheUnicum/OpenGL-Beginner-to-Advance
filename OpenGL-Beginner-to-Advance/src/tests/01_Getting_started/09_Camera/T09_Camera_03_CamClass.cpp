#include "T09_Camera_03_CamClass.h"
#include "GLFW/glfw3.h"
#include <iostream>
namespace test {

	T09_Camera_03_CamClass::T09_Camera_03_CamClass()
		: m_f_mixFactor(.5f), m_f_fov(45.0f),
		m_b_depth_test_active(false), m_b_depth_test_active_i_1(false)
	{
		#include "E00_cube_vertices.h"

		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

		m_VertexBuffer = std::make_unique<VertexBuffer>(cube_vertices, 36 * (3 + 2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/09_Camera/S09_Camera_03_CamClass.Shader");
		m_Shader->Bind();

		m_Texture0 = std::make_unique<Texture>("res/textures/awesomeface.png");
		m_Shader->SetUniform1i("u_Texture0", 0);
		m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
		m_Shader->SetUniform1i("u_Texture1", 1);

		m_Shader->Unbind();
	}

	T09_Camera_03_CamClass::~T09_Camera_03_CamClass()
	{
	}

	void T09_Camera_03_CamClass::OnUpdate(float deltaTime)
	{
	}

	void T09_Camera_03_CamClass::OnRender(GLFWwindow* window)
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

			std::cout << "Enable changed" << std::endl;
			m_mouse_disable_i_1 = m_mouse_disable;
		}

		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		float inv_ratio_aspect = 960.0f / 540.0f;
		m_Shader->Bind();
		m_Texture0->Bind(0);
		m_Texture1->Bind(1);
		m_Shader->SetUniform1f("u_mixFactor", m_f_mixFactor);

		if (!m_b_Cam_Active)
		{
			m_b_firstMouse = true;
		}
		else
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(+0.0f, +0.0f, +0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			view = m_camera->GetViewMatrix();

			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f); 

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, 36, *m_Shader);
		}
	}

	void T09_Camera_03_CamClass::OnImGuiRender()
	{
		ImGui::Text("Camera CLASS");
		ImGui::Text("Eulero : | Pitch  -- Yaw");
		
		ImGui::SliderFloat("Texture Mixing Factor", &m_f_mixFactor, 0.0f, 1.0f);
		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Checkbox("Cam Active/Disactive", &m_b_Cam_Active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);
	}

	void T09_Camera_03_CamClass::OnProcessInput(GLFWwindow * window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			m_camera->ProcessKeyboard(Camera::Camera_Movement::FORWARDS, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			m_camera->ProcessKeyboard(m_camera->BACKWARDS, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			m_camera->ProcessKeyboard(m_camera->LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			m_camera->ProcessKeyboard(m_camera->RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			m_camera->ProcessKeyboard(m_camera->UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			m_camera->ProcessKeyboard(m_camera->DOWN, deltaTime);
		// Mouse pressed
		if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		{
			if (!m_key_pressed)
				m_mouse_disable = !m_mouse_disable;
			m_key_pressed = true;
		}
		else //if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
			m_key_pressed = false;
	}

	void T09_Camera_03_CamClass::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T09_Camera_03_CamClass::mouse_callback(GLFWwindow * window, double xpos, double ypos)
	{
		//std::cout << xpos << " " << ypos << std::endl;

		if (m_b_firstMouse)
		{
			m_camera->ResetYawPitch();
			m_lastX = xpos;
			m_lastY = ypos;
			m_b_firstMouse = false;
		}

		float xoffset = xpos - m_lastX;
		float yoffset = -(ypos - m_lastY);
		m_lastX = xpos;
		m_lastY = ypos;
		m_camera->ProcessMouseMovement(xoffset, yoffset);
	}
}
