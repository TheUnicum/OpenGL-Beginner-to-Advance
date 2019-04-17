#include "T02_PBR_Lighting_02_Textures.h"
#include "GLFW/glfw3.h"

namespace test {

	T02_PBR_Lighting_02_Textures::T02_PBR_Lighting_02_Textures()
		: m_f_fov(45.0f),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_nrRows(7), m_nrColums(7), m_spacing(2.5)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(-10.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -35.0f, 0.0f);
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
		m_Shader = std::make_unique<Shader>("src/tests/06_PBR/02_PBR_Lighting/S02_PBR_Lighting_02_Textures.Shader");


		// lights
		// ------
		m_lightPositions.push_back(glm::vec3(-10.0f, +10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(+10.0f, +10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(-10.0f, -10.0f, +10.f));
		m_lightPositions.push_back(glm::vec3(+10.0f, -10.0f, +10.f));

		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));
		m_lightColors.push_back(glm::vec3(300.0f, 300.0f, 300.0f));

		// Sphere
		//--------------------------------------------------------------
		fill_sphere_mesh_vertices();


		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T02_PBR_Lighting_02_Textures::~T02_PBR_Lighting_02_Textures()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T02_PBR_Lighting_02_Textures::OnUpdate(float deltaTime)
	{
	}

	void T02_PBR_Lighting_02_Textures::OnRender(GLFWwindow* window)
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

		{
			m_Shader->Bind();

			// View position
			m_Shader->SetUniform3fv("u_camPos", m_camera->GetCamPosition());

			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// render rows* columns number of spheres with varying metallic/roughness values scaled by rows and columns respectively
			for (int row = 0; row < m_nrRows; ++row)
			{
				for (int col = 0; col < m_nrColums; ++col)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (m_nrColums / 2)) * m_spacing,
						(row - (m_nrRows / 2)) * m_spacing,
						0.0f
					));
					mvp = proj * view * model;
					m_Shader->SetUniformMat4f("u_model", model);
					m_Shader->SetUniformMat4f("u_mvp", mvp);
					m_Shere_mesh->Draw(m_Shader, false, GL_TRIANGLE_STRIP);
				}
			}

			// render light source (simply re-render sphere at light positions)
			// this looks a bit off as we use the same shader, but it'll make their positions obvious and
			// keeps the codeprint small.
			for (unsigned int i = 0; i < m_lightPositions.size(); ++i)
			{
				glm::vec3 newPos = m_lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0f) * 5.0f, 0.0f, 0.0f);
				newPos = m_lightPositions[i]; 
				m_Shader->SetUniform3fv("u_lightPositions[" + std::to_string(i) + "]", newPos);
				m_Shader->SetUniform3fv("u_lightColors[" + std::to_string(i) + "]", m_lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, newPos);
				model = glm::scale(model, glm::vec3(0.5f));
				mvp = proj * view * model;
				m_Shader->SetUniformMat4f("u_model", model);
				m_Shader->SetUniformMat4f("u_mvp", mvp);
				m_Shere_mesh->Draw(m_Shader, false, GL_TRIANGLE_STRIP);
			}
		}


	}

	void T02_PBR_Lighting_02_Textures::OnImGuiRender()
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

	void T02_PBR_Lighting_02_Textures::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T02_PBR_Lighting_02_Textures::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T02_PBR_Lighting_02_Textures::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T02_PBR_Lighting_02_Textures::fill_sphere_mesh_vertices()
	{
		unsigned int indexCount;

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;


		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<Vertex> vertices_sphere_3v_3n_2t;
		for (int i = 0; i < positions.size(); i++)
			vertices_sphere_3v_3n_2t.emplace_back(positions[i], normals[i], uv[i]);

		m_TexAlbedo = std::make_shared<Texture>("res/textures/pbr/rusted_iron/albedo.png", TextureType::ALBEDO);
		m_TexNormal = std::make_shared<Texture>("res/textures/pbr/rusted_iron/normal.png", TextureType::NORMAL);
		m_TexMetallic = std::make_shared<Texture>("res/textures/pbr/rusted_iron/metallic.png", TextureType::METALLIC);
		m_TexRoughness = std::make_shared<Texture>("res/textures/pbr/rusted_iron/roughness.png", TextureType::ROUGHNESS);
		m_TexAO = std::make_shared<Texture>("res/textures/pbr/rusted_iron/ao.png", TextureType::AO);
		msp_Textures.push_back(m_TexAlbedo);
		msp_Textures.push_back(m_TexNormal);
		msp_Textures.push_back(m_TexMetallic);
		msp_Textures.push_back(m_TexRoughness);
		msp_Textures.push_back(m_TexAO);

		m_Shere_mesh = std::make_unique<Mesh>(vertices_sphere_3v_3n_2t, indices, msp_Textures);
	}

}


