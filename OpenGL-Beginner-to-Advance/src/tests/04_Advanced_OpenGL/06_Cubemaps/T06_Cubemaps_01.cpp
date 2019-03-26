#include "T06_Cubemaps_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T06_Cubemaps_01::T06_Cubemaps_01()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_b_disable_cubemap_lock(false), m_b_optimization(false)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
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

		// Textures - Shared pointer
		msp_mTexture0 = std::make_shared<Texture>("res/textures/container.jpg", TextureType::DIFFUSE); // ontainer2.png
		msp_Textures.push_back(msp_mTexture0);

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		m_ShaderMesh = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/06_Cubemaps/S06_Cubemaps_01.Shader");

		
		// Skybox-------------------------------------------------------------------------
		std::vector<std::string> faces
		{
			"right.jpg",
			"left.jpg",
			"top.jpg",
			"bottom.jpg",
			"front.jpg",
			"back.jpg"
		};
		m_cubemapTexture.LoadCubemap(faces, "res/textures/skybox/Lake01/");


		m_VertexBuffer = std::make_unique<VertexBuffer>(skyboxVertices, 36 * (3) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_ShaderSky = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/06_Cubemaps/S06_Cubemaps_01_Sky.Shader");




		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T06_Cubemaps_01::~T06_Cubemaps_01()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T06_Cubemaps_01::OnUpdate(float deltaTime)
	{
	}

	void T06_Cubemaps_01::OnRender(GLFWwindow* window)
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
		float inv_ratio_aspect = 960.0f / 540.0f;

		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);


		// 1st method
		// First draw the cubemap with depth write disable
		// (+) easy to make
		// (-) have to draw all the cubemap interely every time
		if (!m_b_optimization)
		{	// Skybox first, disabling depth writing
			glDepthMask(GL_FALSE);
			m_ShaderSky->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			// MVP
			if (m_b_disable_cubemap_lock)
				mvp = proj * view * model;
			else
				mvp = proj * glm::mat4(glm::mat3(view)) * model;
			m_ShaderSky->SetUniformMat4f("u_mvp", mvp);
			m_ShaderSky->SetUniform1i("u_b_optimization", m_b_optimization);

			m_VAO->Bind();
			m_cubemapTexture.Bind();
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthMask(GL_TRUE);
		}

		DrawCube(glm::vec3(-1.0f, 0.01f, -1.0f), proj, view);
		DrawCube(glm::vec3(2.0f, 0.01f, 0.0f), proj, view);

		// 1st method
		// Draw the cubemap at the end!
		// Change in the shader 
		// ------------------------
		// gl_Position = pos.xyww;
		// ------------------------
		if (m_b_optimization)
		{	// Skybox first, disabling depth writing
			glDepthFunc(GL_LEQUAL);
			m_ShaderSky->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			// MVP
			if (m_b_disable_cubemap_lock)
				mvp = proj * view * model;
			else
				mvp = proj * glm::mat4(glm::mat3(view)) * model;
			m_ShaderSky->SetUniformMat4f("u_mvp", mvp);
			m_ShaderSky->SetUniform1i("u_b_optimization", m_b_optimization);

			m_VAO->Bind();
			m_cubemapTexture.Bind();

			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthFunc(GL_LESS);
		}
	}

	void T06_Cubemaps_01::OnImGuiRender()
	{
		ImGui::Text("CubeMaps - Skycube");
		IMGUI_FPS;

		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
		
		ImGui::Text("Cubemap options:");
		ImGui::Checkbox("Disable cubemap lock", &m_b_disable_cubemap_lock);
		ImGui::Checkbox("An optimization", &m_b_optimization);
	}

	void T06_Cubemaps_01::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T06_Cubemaps_01::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T06_Cubemaps_01::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T06_Cubemaps_01::DrawCube(glm::vec3 position, glm::mat4 proj, glm::mat4 view)
	{
		m_ShaderMesh->Bind();
		glm::mat4 model = glm::mat4(1.0f);

		// Model Matrix
		model = glm::translate(model, position);

		// MVP
		glm::mat4 mvp = proj * view * model;
		m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);

		// Draw MESH
		m_mesh->Draw(m_ShaderMesh);
	}
}


