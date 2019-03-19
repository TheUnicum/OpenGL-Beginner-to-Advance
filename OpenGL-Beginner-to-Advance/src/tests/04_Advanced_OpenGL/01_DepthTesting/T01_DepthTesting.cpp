#include "T01_DepthTesting.h"
#include "GLFW/glfw3.h"

namespace test {

	T01_DepthTesting::T01_DepthTesting()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_Depht_GL_Always_active(false), m_b_Depht_GL_Always_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),

		m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(false), m_b_emission(false),
		m_b_light_move_active(false), m_b_cube_rotating_active(false), m_b_cube_scale_active(false)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(-2.0f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.f, -20.f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW mesh----------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : cube_vertices_3v_3n_2t)
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
		msp_mTexture0 = std::make_shared<Texture>("res/textures/marble.jpg", TextureType::DIFFUSE); // ontainer2.png
		msp_mTexture1 = std::make_shared<Texture>("res/textures/container2_specular.png", TextureType::SPECULAR);
		msp_mTexture2 = std::make_shared<Texture>("res/textures/matrix_2.jpg", TextureType::EMISSION);
		msp_Textures.push_back(msp_mTexture0);
		msp_Textures.push_back(msp_mTexture1);
		msp_Textures.push_back(msp_mTexture2);

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		m_ShaderMesh = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/01_DepthTesting/S01_DepthTesting.Shader");







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
		std::vector<unsigned int> vec_quad_indices;
		i = 0;
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
		
		msp_mTextureFloor = std::make_shared<Texture>("res/textures/metal.png", TextureType::DIFFUSE); // ontainer2.png
		msp_mTextures2.push_back(msp_mTextureFloor);

		m_mesh_quad = std::make_unique<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_mTextures2);






	}

	T01_DepthTesting::~T01_DepthTesting()
	{
	}

	void T01_DepthTesting::OnUpdate(float deltaTime)
	{
	}

	void T01_DepthTesting::OnRender(GLFWwindow* window)
	{
		if (m_b_depth_test_active != m_b_depth_test_active_i_1)
		{
			if (m_b_depth_test_active)
			{
				// Remember to glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
				GLCall(glEnable(GL_DEPTH_TEST)); 
				//GLCall(glDepthMask(GL_FALSE)); //OpenGL allows us to disable writing to the depth buffer by setting its depth mask to GL_FALSE
			}
			else
			{
				GLCall(glDisable(GL_DEPTH_TEST));
			}
			m_b_depth_test_active_i_1 = m_b_depth_test_active;
		}

		// Depth Testing
		if (m_b_Depht_GL_Always_active != m_b_Depht_GL_Always_active_i_1)
		{
			if (m_b_Depht_GL_Always_active)
			{
				GLCall(glDepthFunc(GL_ALWAYS));
			}
			else
			{
				GLCall(glDepthFunc(GL_LESS));
			}
			m_b_Depht_GL_Always_active_i_1 = m_b_Depht_GL_Always_active;
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

		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

		// Light
		// change the light's position values over time (can be done anywhere in the render loop actually, 
		// but try to do it at least before using the light source positions)
		if (m_b_light_move_active)
		{
			m_lightPos.x = (1.0f + (float)sin(glfwGetTime()) * 2.0f);
			m_lightPos.y = (float)sin(glfwGetTime() / 2.0f) * 1.0f;
		}


		if (true)
		{	// Mesh 1st Cube
			m_ShaderMesh->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
			if (m_b_cube_rotating_active)
			{
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (m_b_cube_scale_active)
				model = glm::scale(model, glm::vec3(1.0f, 0.2f, 2.3f));

			// MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);
			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// Material
			m_ShaderMesh->SetUniform1f("material.shininess", 2.0f);

			// Light Position
			m_ShaderMesh->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_ShaderMesh->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			m_ShaderMesh->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);
			m_ShaderMesh->SetUniform1i("u_b_emission", m_b_emission);

			// Draw MESH
			m_mesh->Draw(m_ShaderMesh);
		}

		{	// Mesh 2nd Cube
			m_ShaderMesh->Bind();
			model = glm::mat4(1.0f);

			// Model Matrix
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
			if (m_b_cube_rotating_active)
			{
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (m_b_cube_scale_active)
				model = glm::scale(model, glm::vec3(1.0f, 0.2f, 2.3f));

			// MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);
			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// Material
			m_ShaderMesh->SetUniform1f("material.shininess", 2.0f);

			// Light Position
			m_ShaderMesh->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_ShaderMesh->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			m_ShaderMesh->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);
			m_ShaderMesh->SetUniform1i("u_b_emission", m_b_emission);

			// Draw MESH
			m_mesh->Draw(m_ShaderMesh);
		}

		{	// Floor
			m_ShaderMesh->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
			model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(10.0f));

			//MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);
			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// Material
			m_ShaderMesh->SetUniform1f("material.shininess", 2.0f);

			// Light Position
			m_ShaderMesh->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			m_ShaderMesh->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			m_ShaderMesh->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			m_ShaderMesh->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			m_ShaderMesh->SetUniform1i("u_b_ambient", m_b_ambient);
			m_ShaderMesh->SetUniform1i("u_b_diffuse", m_b_diffuse);
			m_ShaderMesh->SetUniform1i("u_b_specular", m_b_specular);
			m_ShaderMesh->SetUniform1i("u_b_emission", m_b_emission);

			m_mesh_quad->Draw(m_ShaderMesh);
		}

	}

	void T01_DepthTesting::OnImGuiRender()
	{
		ImGui::Text("Depth testing ");
		
		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);
		ImGui::Checkbox("Emission Light", &m_b_emission);

		ImGui::Text("Exercises A - motion options");
		ImGui::Checkbox("Light move active", &m_b_light_move_active);
		ImGui::Checkbox("Cube rotation active", &m_b_cube_rotating_active);
		ImGui::Checkbox("Cube scale active", &m_b_cube_scale_active);

		ImGui::Text("Depth Testing Options:");
		ImGui::Checkbox("Depth Testing func(GL_ALWAYS)", &m_b_Depht_GL_Always_active);
	}

	void T01_DepthTesting::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T01_DepthTesting::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T01_DepthTesting::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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


