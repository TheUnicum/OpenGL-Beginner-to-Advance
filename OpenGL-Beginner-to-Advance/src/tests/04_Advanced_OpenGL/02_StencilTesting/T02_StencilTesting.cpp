#include "T02_StencilTesting.h"
#include "GLFW/glfw3.h"

namespace test {

	T02_StencilTesting::T02_StencilTesting()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_Depht_GL_Always_active(false), m_b_Depht_GL_Always_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),

		m_lightPos(glm::vec3(1.2f, 1.0f, 2.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(false), m_b_emission(false),
		m_b_light_move_active(false), m_b_cube_rotating_active(false), m_b_cube_scale_active(false),
		m_i_DP_Visualizing_opt(0),
		m_b_Object_outlining(true)
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
		m_mesh = std::make_shared<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		m_ShaderMesh = std::make_shared<Shader>("src/tests/04_Advanced_OpenGL/02_StencilTesting/S02_StencilTesting.Shader");
		m_ShaderSingleColor = std::make_shared<Shader>("src/tests/04_Advanced_OpenGL/02_StencilTesting/S02_StencilTesting_SingleColor.Shader");







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

		m_mesh_quad = std::make_shared<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_mTextures2);






	}

	T02_StencilTesting::~T02_StencilTesting()
	{
	}

	void T02_StencilTesting::OnUpdate(float deltaTime)
	{
	}

	void T02_StencilTesting::OnRender(GLFWwindow* window)
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

		// Depth Test & Stencil Test
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glDepthFunc(GL_LESS));
		GLCall(glEnable(GL_STENCIL_TEST));
		GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));


		GLCall(glStencilMask(0x00)); // Make sure we don't update the stencil buffer while drawing the floor
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

			// Setup Visualizing depth buffer opton
			m_ShaderMesh->SetUniform1i("u_i_DP_Visualizing_opt", m_i_DP_Visualizing_opt);

			m_mesh_quad->Draw(m_ShaderMesh);
		}

		// 1st. render pass, draw objects as normal, writing to the stencil buffer
		// --------------------------------------------------------------------
		GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
		GLCall(glStencilMask(0xFF));
		RenderMesh(m_mesh, m_ShaderMesh, glm::vec3(-1.0f, 0.0f, -1.0f), 1);
		RenderMesh(m_mesh, m_ShaderMesh, glm::vec3(2.0f, 0.0f, 0.0f), 1);

		// 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		// Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing 
		// the objects' size differences, making it look like borders.
		// -----------------------------------------------------------------------------------------------------------------------------
		if (m_b_Object_outlining)
		{
			GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0XFF));
			GLCall(glStencilMask(0x00));
			GLCall(glDisable(GL_DEPTH_TEST));
			RenderMesh(m_mesh, m_ShaderSingleColor, glm::vec3(-1.0f, 0.0f, -1.0f), 1.1);
			RenderMesh(m_mesh, m_ShaderSingleColor, glm::vec3(2.0f, 0.0f, 0.0f), 1.1);
		}
		// Reset opt
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST); // To enable ImGui!!

	}

	void T02_StencilTesting::OnImGuiRender()
	{
		ImGui::Text("Depth testing ");
		IMGUI_FPS;

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
		ImGui::Text("Visualizing the depth buffer opt:");
		ImGui::Text("0 - Normal");
		ImGui::Text("1 - Non linear");
		ImGui::Text("2 - linear");
		ImGui::SliderInt("Choise", &m_i_DP_Visualizing_opt, 0, 2);

		ImGui::Text("Stencil Testing Options:");
		ImGui::Checkbox("Active Object Outlining", &m_b_Object_outlining);

	}

	void T02_StencilTesting::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T02_StencilTesting::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
	}

	void T02_StencilTesting::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T02_StencilTesting::RenderMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, glm::vec3 position, float scale)
	{	// Mesh 1st Cube
			// Global Variables
			float inv_ratio_aspect = 960.0f / 540.0f;
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);
			glm::mat4 mvp;

			view = m_camera->GetViewMatrix();
			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

			shader->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			model = glm::translate(model, position);
			if (m_b_cube_rotating_active)
			{
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (m_b_cube_scale_active)
				model = glm::scale(model, glm::vec3(1.0f, 0.2f, 2.3f));

			model = glm::scale(model, glm::vec3(scale));

			// MVP
			mvp = proj * view * model;
			shader->SetUniformMat4f("u_mvp", mvp);
			shader->SetUniformMat4f("u_model", model);
			shader->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// Material
			shader->SetUniform1f("material.shininess", 2.0f);

			// Light Position
			shader->SetUniform3fv("light.position", m_lightPos);

			// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
			shader->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
			shader->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
			shader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

			// View position
			shader->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Active Phong lighting componetst
			shader->SetUniform1i("u_b_ambient", m_b_ambient);
			shader->SetUniform1i("u_b_diffuse", m_b_diffuse);
			shader->SetUniform1i("u_b_specular", m_b_specular);
			shader->SetUniform1i("u_b_emission", m_b_emission);

			// Setup Visualizing depth buffer opton
			shader->SetUniform1i("u_i_DP_Visualizing_opt", m_i_DP_Visualizing_opt);

			// Draw MESH
			mesh->Draw(shader);
		}

}
