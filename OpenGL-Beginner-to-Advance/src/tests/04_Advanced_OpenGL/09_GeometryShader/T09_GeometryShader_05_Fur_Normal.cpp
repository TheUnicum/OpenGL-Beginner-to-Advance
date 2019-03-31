#include "T09_GeometryShader_05_Fur_Normal.h"
#include "GLFW/glfw3.h"

namespace test {

	T09_GeometryShader_05_Fur_Normal::T09_GeometryShader_05_Fur_Normal()
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
		
		m_Shader = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/09_GeometryShader/S09_GeometryShader_04_Exploding.Shader");
		m_ShaderFur = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/09_GeometryShader/S09_GeometryShader_05_Fur_Normal.Shader");
		
		// NEW Model----------------------------------------------------------------------------------------
		m_model = std::make_unique<Model>("res/objects/nanosuit/nanosuit.obj");

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T09_GeometryShader_05_Fur_Normal::~T09_GeometryShader_05_Fur_Normal()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T09_GeometryShader_05_Fur_Normal::OnUpdate(float deltaTime)
	{
	}

	void T09_GeometryShader_05_Fur_Normal::OnRender(GLFWwindow* window)
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
			model = glm::mat4(1.f);
			m_Shader->Bind();

			// MVP
			mvp = proj * view * model;
			m_Shader->SetUniformMat4f("u_mvp", mvp);
			m_Shader->SetUniform1f("u_time", glm::radians(-90.0f)); // set Zero 0

			m_model->Draw(m_Shader);
		}

		// Model // fury
		{
			model = glm::mat4(1.f);
			m_ShaderFur->Bind();

			// MVP
			mvp = proj * view * model;

			m_ShaderFur->SetUniformMat4f("u_proj", proj);
			m_ShaderFur->SetUniformMat4f("u_view", view);
			m_ShaderFur->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			m_ShaderFur->SetUniformMat4f("u_mvp", mvp);

			m_model->Draw(m_ShaderFur);
		}
	}

	void T09_GeometryShader_05_Fur_Normal::OnImGuiRender()
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
		ImGui::Text("Remember:");
		ImGui::Text("S09_GeometryShader_03_House_X");
		ImGui::Text("is a pass-through geometry shader");
	}

	void T09_GeometryShader_05_Fur_Normal::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T09_GeometryShader_05_Fur_Normal::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T09_GeometryShader_05_Fur_Normal::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
