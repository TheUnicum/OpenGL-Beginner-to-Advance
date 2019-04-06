#include "T04_NormalMap_02_TangentSpace.h"
#include "GLFW/glfw3.h"

namespace test {

	T04_NormalMap_02_TangentSpace::T04_NormalMap_02_TangentSpace()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_lightPos(glm::vec3(0.5f, +1.0f, .3f)),
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


		// NEW mesh-  index quad----------------------------------------------------------------------------------
		m_quadVertices_3p_3n_2t_3T_3B = (float*)alloca(6 * (3 + 3 + 2 + 3 + 3) * sizeof(float));
		// (3) position			// (3) normal	  // texcoords  // (3) tangent						// (3) bitangent
		ManualCalculationTangentAndBitanget(m_quadVertices_3p_3n_2t_3T_3B);
		
		// Quad 
		m_Quad_VertexBuffer = std::make_unique<VertexBuffer>(m_quadVertices_3p_3n_2t_3T_3B, 6 * (3 + 3 + 2 + 3 + 3) * sizeof(float));

		VertexBufferLayout layout2;
		layout2.Push<float>(3);
		layout2.Push<float>(3);
		layout2.Push<float>(2);
		layout2.Push<float>(3);
		layout2.Push<float>(3);
		m_Quad_VAO = std::make_unique<VertexArray>();
		m_Quad_VAO->AddBuffer(*m_Quad_VertexBuffer, layout2);


		/*
		// NEW mesh-  index quad----------------------------------------------------------------------------------
		float positions_quad[] = {
			// positions          // normals           // texture coords	// index
			-0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,			// 0
			 0.5f, -0.5f,  0.0f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,			// 1
			 0.5f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,			// 2
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,			// 3
		};

		unsigned int uint_quad_indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		std::vector<Vertex> vertices_quad_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		std::vector<unsigned int> vec_quad_indices;
		int i = 0;
		for (float val : positions_quad)
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
				vertices_quad_3v_3n_2t.emplace_back(temp_pos, temp_norm, temp_texC);
				i = -1; // Restart counter...
				break;
			default:
				break;
			}
			i++;
		}
		
		for (unsigned int val : uint_quad_indices)
			vec_quad_indices.push_back(val);
		*/

		// Textures - Shared pointer
		msp_mTexture0 = std::make_shared<Texture>("res/textures/brickwall.jpg", TextureType::DIFFUSE, false);
		msp_mTexture1 = std::make_shared<Texture>("res/textures/brickwall_normal.jpg", TextureType::NORMAL, false);
		msp_Textures.push_back(msp_mTexture0);
		msp_Textures.push_back(msp_mTexture1);

		//m_mesh = std::make_unique<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_Textures);
		m_ShaderMesh = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/04_Normal_Mapping/S04_NormalMap_02_TangentSpace.Shader");
		m_ShaderLight = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/04_Normal_Mapping/S00_Color_01a_Light.Shader");
	}

	T04_NormalMap_02_TangentSpace::~T04_NormalMap_02_TangentSpace()
	{
	}

	void T04_NormalMap_02_TangentSpace::OnUpdate(float deltaTime)
	{
	}

	void T04_NormalMap_02_TangentSpace::OnRender(GLFWwindow* window)
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
			m_ShaderMesh->SetUniform3f("light.diffuse", 1.0f, 1.0f, 1.0f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 0.2f, 0.2f, 0.2f);

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

			msp_mTexture0->Bind(0);
			msp_mTexture1->Bind(1);
			m_ShaderMesh->SetUniform1i("material.diffuse1", 0);
			m_ShaderMesh->SetUniform1i("material.normal1", 1);

			Renderer renderer1;
			renderer1.Draw(*m_Quad_VAO, 6, *m_ShaderMesh);



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

	void T04_NormalMap_02_TangentSpace::OnImGuiRender()
	{
		ImGui::Text("Normal Mapping 02 - Tangent Space");
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
		ImGui::Text("3 : Normal Map / Nor.Map Tangent Space");
		ImGui::Text("4 : Nor.Map Tangent Space");
		ImGui::SliderInt("NM opt:", &m_i_NormalMap_opt, 0, 4);
		ImGui::Checkbox("Light move active", &m_b_light_move_active);
		ImGui::Checkbox("Nor.Map. diffuse", &m_b_NorMap_diffuse);
		ImGui::Checkbox("Nor.Map. specular", &m_b_NorMap_specular);
		
		ImGui::SliderInt("Normal rotation opt", &m_i_NormalMap_rotation_opt, 0, 5);

		ImGui::SliderFloat3("Quad orientation", &m_quad_orientation[0], -45, 45);
	}

	void T04_NormalMap_02_TangentSpace::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T04_NormalMap_02_TangentSpace::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
	}

	void T04_NormalMap_02_TangentSpace::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	void T04_NormalMap_02_TangentSpace::ManualCalculationTangentAndBitanget(float * ptr_quad)
	{
		// positions 
		glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
		glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
		glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
		glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
		// texture coordinates
		glm::vec2 uv1(0.0f, 1.0f);
		glm::vec2 uv2(0.0f, 0.0f);
		glm::vec2 uv3(1.0f, 0.0f);
		glm::vec2 uv4(1.0f, 1.0f);
		// normlal vector
		glm::vec3 nm(0.0f, 0.0f, 1.0f);

		// calculate tangent/bitangent vectors of both triangles
		glm::vec3 tangent1, bitangent1;
		glm::vec3 tangent2, bitangent2;

		// triangle 1
		// --------------------------------------------------------------------
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// triangle 2
		// --------------------------------------------------------------------
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;
		deltaUV1 = uv3 - uv1;
		deltaUV2 = uv4 - uv1;

		f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		// (3) position		   // (3) normal
		float quadVertices[] = {
			// (3) position			// (3) normal	  // texcoords  // (3) tangent						// (3) bitangent
			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		};

		for (int i = 0; i < sizeof(quadVertices) / sizeof(float) ; i++)
			ptr_quad[i] = quadVertices[i];
	}
}


