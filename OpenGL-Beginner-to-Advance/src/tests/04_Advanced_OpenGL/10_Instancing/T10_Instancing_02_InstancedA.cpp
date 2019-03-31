#include "T10_Instancing_02_InstancedA.h"
#include "GLFW/glfw3.h"

#include <string>
#include <sstream>

namespace test {

	T10_Instancing_02_InstancedA::T10_Instancing_02_InstancedA()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 14.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"
		
		m_VertexBuffer = std::make_unique<VertexBuffer>(quadVertices_2v_3c, 6 * (2 + 3) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);	// position in plane
		layout.Push<float>(3);	// color
		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		glm::vec2 translations[100];
		int index = 0;
		float offset = 0.1f;
		for (int y = -10; y < 10; y += 2)
		{
			for (int x = -10; x < 10; x += 2)
			{
				glm::vec2 translation;
				translation.x = (float)x / 10.0f + offset;
				translation.y = (float)y / 10.0f + offset;
				translations[index++] = translation;
			}
		}  
		m_VertexBuffer_2 = std::make_unique<VertexBuffer>(translations, sizeof(glm::vec2) * 100);
		VertexBufferLayout layout2;
		layout2.Push<float>(2);	// color
		m_VAO->AddBuffer(*m_VertexBuffer_2, layout2, 2);
		GLCall(glVertexAttribDivisor(2, 1));


		m_Shader = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/10_Instancing/S10_Instancing_02_InstancedA.Shader");

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T10_Instancing_02_InstancedA::~T10_Instancing_02_InstancedA()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T10_Instancing_02_InstancedA::OnUpdate(float deltaTime)
	{
	}

	void T10_Instancing_02_InstancedA::OnRender(GLFWwindow* window)
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


		// Model
		{

			m_Shader->Bind();
			m_VAO->Bind();
			GLCall(glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100));// m_v_count
		}
	}

	void T10_Instancing_02_InstancedA::OnImGuiRender()
	{
		ImGui::Text("GeometryShader - 03");
		IMGUI_FPS;

		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text(" ");
		ImGui::Text("glDrawArrays(GL_POINTS, 0, 4)");
		ImGui::Text(" ");

		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);

		ImGui::Text(" ");
		ImGui::Text("<glDrawArraysInstanced>");
		ImGui::Text("...(GL_TRIANGLES, 0, 6, 100)");

	}

	void T10_Instancing_02_InstancedA::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T10_Instancing_02_InstancedA::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T10_Instancing_02_InstancedA::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
