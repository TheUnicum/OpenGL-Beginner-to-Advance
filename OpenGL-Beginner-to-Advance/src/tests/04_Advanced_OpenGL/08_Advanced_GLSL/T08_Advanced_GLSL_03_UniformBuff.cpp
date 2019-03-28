#include "T08_Advanced_GLSL_03_UniformBuff.h"
#include "GLFW/glfw3.h"

namespace test {

	T08_Advanced_GLSL_03_UniformBuff::T08_Advanced_GLSL_03_UniformBuff()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW mesh----------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : cube_vertices_FC_3v_3n_2t)
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
			case 3:	// TexCoords
				temp_texC.x = val;
				break;
			case 4:
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

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		m_ShaderMeshRed = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/08_Advanced_GLSL/S08_Advanced_GLSL_03_UniformBuff_R.Shader");
		m_ShaderMeshGreen = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/08_Advanced_GLSL/S08_Advanced_GLSL_03_UniformBuff_G.Shader");
		m_ShaderMeshBlue = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/08_Advanced_GLSL/S08_Advanced_GLSL_03_UniformBuff_B.Shader");
		m_ShaderMeshYellow = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/08_Advanced_GLSL/S08_Advanced_GLSL_03_UniformBuff_Y.Shader");

		// Set the uniform block of the vertex shaders equal to binding point 0.
		unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(m_ShaderMeshRed->GetID(), "Matrices");
		unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(m_ShaderMeshGreen->GetID(), "Matrices");
		unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(m_ShaderMeshBlue->GetID(), "Matrices");
		unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(m_ShaderMeshYellow->GetID(), "Matrices");

		glUniformBlockBinding(m_ShaderMeshRed->GetID(), uniformBlockIndexRed, 0);
		glUniformBlockBinding(m_ShaderMeshGreen->GetID(), uniformBlockIndexGreen, 0);
		glUniformBlockBinding(m_ShaderMeshBlue->GetID(), uniformBlockIndexBlue, 0);
		glUniformBlockBinding(m_ShaderMeshYellow->GetID(), uniformBlockIndexYellow, 0);

		// We create the actual buffer object and also bind he buffer to binding point 0
		//unsigned int uboMatrices;
		glGenBuffers(1, &m_uboMatrices);

		glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));


		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T08_Advanced_GLSL_03_UniformBuff::~T08_Advanced_GLSL_03_UniformBuff()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T08_Advanced_GLSL_03_UniformBuff::OnUpdate(float deltaTime)
	{
	}

	void T08_Advanced_GLSL_03_UniformBuff::OnRender(GLFWwindow* window)
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

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

		//Now all that's left to do is actually fill the buffer
		glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &proj[0][0]);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view[0][0]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		//DrawCube(glm::vec3(-0.75f, +0.75f, +0.0f), proj, view);
		DrawCube(m_ShaderMeshRed, glm::vec3(-0.75f, +0.75f, +0.0f));
		DrawCube(m_ShaderMeshGreen, glm::vec3(+0.75f, +0.75f, +0.0f));
		DrawCube(m_ShaderMeshBlue, glm::vec3(-0.75f, -0.75f, +0.0f));
		DrawCube(m_ShaderMeshYellow, glm::vec3(0.75f, -0.75f, +0.0f));


	}

	void T08_Advanced_GLSL_03_UniformBuff::OnImGuiRender()
	{
		ImGui::Text("GLSL's built-in variables");
		ImGui::Text("Uniform buffer objects");
		IMGUI_FPS;

		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T08_Advanced_GLSL_03_UniformBuff::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T08_Advanced_GLSL_03_UniformBuff::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T08_Advanced_GLSL_03_UniformBuff::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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


	void T08_Advanced_GLSL_03_UniformBuff::DrawCube(std::shared_ptr<Shader> shader, glm::vec3 position)
	{
		shader->Bind();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		shader->SetUniformMat4f("u_model", model);
		m_mesh->Draw(shader);
	}
}


