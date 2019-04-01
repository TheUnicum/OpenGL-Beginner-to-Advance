#include "T10_Instancing_05_Asteroid_Field_Tests.h"
#include "GLFW/glfw3.h"

#include <string>
#include <sstream>

namespace test {

	T10_Instancing_05_Asteroid_Field_Tests::T10_Instancing_05_Asteroid_Field_Tests()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_instanced(true),
		m_amount(10000),
		m_radius(40.0f), m_offset(10.0f),
		m_key_velo(5)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 20.0f, 75.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		m_Shader = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/10_Instancing/S10_Instancing_03_Asteroid_Field_Base.Shader");
		m_ShaderInstance = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/10_Instancing/S10_Instancing_05_Asteroid_Field_Tests.Shader");

		// Models loading------------------------------------------------------------------------------------
		m_model_Planet = std::make_unique<Model>("res/objects/planet/planet.obj");
		m_model_Rock = std::make_unique<Model>("res/objects/rock/rock.obj");

		// ---------------------
		m_VertexBuffer = std::make_unique<VertexBuffer>(m_modelMatrices, sizeof(glm::mat4) * m_amount);
		fillModelMatrices();

		//  VSync / Enabel & Disable
		//glfwSwapInterval(1);
	}

	T10_Instancing_05_Asteroid_Field_Tests::~T10_Instancing_05_Asteroid_Field_Tests()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T10_Instancing_05_Asteroid_Field_Tests::OnUpdate(float deltaTime)
	{
	}

	void T10_Instancing_05_Asteroid_Field_Tests::OnRender(GLFWwindow* window)
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
		// GLCall(glDisable(GL_BLEND));

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
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 1000.0f);


		if ((m_amount != m_amount_i_1) || (m_radius != m_radius_i_1) || (m_offset != m_offset_i_1))
			fillModelMatrices();


		// draw Planet
		m_Shader->Bind();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		// MVP
		mvp = proj * view * model;
		m_Shader->SetUniformMat4f("u_mvp", mvp);
		m_model_Planet->Draw(m_Shader);

		// draw meteorites
		if (!m_instanced)
		{
			for (unsigned int i = 0; i < m_amount; i++)
			{
				m_Shader->Bind();
				// MVP
				mvp = proj * view * m_modelMatrices[i];
				m_Shader->SetUniformMat4f("u_mvp", mvp);
				m_model_Rock->Draw(m_Shader);
			}
		}

		if (m_instanced)
		{
			// draw meteorites
			m_ShaderInstance->Bind();
			m_ShaderInstance->SetUniformMat4f("u_view", view);
			m_ShaderInstance->SetUniformMat4f("u_proj", proj);

			m_model_Rock->Draw(m_ShaderInstance, (unsigned int)m_amount);
		}
	}

	void T10_Instancing_05_Asteroid_Field_Tests::OnImGuiRender()
	{
		ImGui::Text("GeometryShader - 03");
		IMGUI_FPS;

		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text(" ");
		ImGui::Checkbox("Instanced Active", &m_instanced);
		ImGui::SliderInt("Rocks Number", &m_amount, 10, 300000);
		ImGui::SliderFloat("Orbit Radius", &m_radius, 20.0f, 150.0f);
		ImGui::SliderFloat("Orbit width", &m_offset, 0.5f, 50.0f);
		ImGui::SliderInt("key velo:", &m_key_velo, 1, 15);
		ImGui::Text(" ");

		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T10_Instancing_05_Asteroid_Field_Tests::OnProcessInput(GLFWwindow * window, float deltaTime)
	{
		glm::vec3 direction(0.0f);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			direction.z -= m_key_velo;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			direction.z += m_key_velo;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			direction.x -= m_key_velo;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			direction.x += m_key_velo;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			direction.y += m_key_velo;
		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
			direction.y -= m_key_velo;
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

	void T10_Instancing_05_Asteroid_Field_Tests::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T10_Instancing_05_Asteroid_Field_Tests::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	void T10_Instancing_05_Asteroid_Field_Tests::fillModelMatrices()
	{
		//m_modelMatrices = (glm::mat4*)alloca(m_amount * sizeof(glm::mat4));
		srand(glfwGetTime()); // initialize random seed	
		//float radius = 50.0;
		//float offset = 2.5f;
		for (unsigned int i = 0; i < m_amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)m_amount * 360.0f;
			float displacement = (rand() % (int)(2 * m_offset * 100)) / 100.0f - m_offset;
			float x = sin(angle) * m_radius + displacement;
			displacement = (rand() % (int)(2 * m_offset * 100)) / 100.0f - m_offset;
			float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
			displacement = (rand() % (int)(2 * m_offset * 100)) / 100.0f - m_offset;
			float z = cos(angle) * m_radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (rand() % 20) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (rand() % 360);
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			m_modelMatrices[i] = model;
		}
		m_VertexBuffer->FillBuffer(m_modelMatrices, sizeof(glm::mat4) * m_amount);
		VertexBufferLayout layout2;
		layout2.Push<float>(4);
		layout2.Push<float>(4);
		layout2.Push<float>(4);
		layout2.Push<float>(4);
		m_model_Rock->AddBuffer(*m_VertexBuffer, layout2, 3);

		m_amount_i_1 = m_amount;
		m_radius_i_1 = m_radius;
		m_offset_i_1 = m_offset;
	}
}
