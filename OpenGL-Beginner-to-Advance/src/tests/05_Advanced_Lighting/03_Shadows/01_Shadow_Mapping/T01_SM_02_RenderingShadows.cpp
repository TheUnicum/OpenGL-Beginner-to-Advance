#include "T01_SM_02_RenderingShadows.h"
#include "GLFW/glfw3.h"

namespace test {

	T01_SM_02_RenderingShadows::T01_SM_02_RenderingShadows()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_lightPos(glm::vec3(0.0f, 0.0f, 0.0f)),
		m_b_ambient(true), m_b_diffuse(true), m_b_specular(true), m_b_emission(true),
		m_b_blinn(false), m_b_disable_adapt_shininess(false), m_b_specular_texture(false),
		m_b_rotating(false),
		//m_SHADOW_WIDTH(1024), m_SHADOW_HEIGHT(1024),
		m_SHADOW_WIDTH(4024), m_SHADOW_HEIGHT(4024),
		m_b_gamma(false), m_b_quadratic(false), m_f_gamma(2.2)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.f, 0.f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, +0.0f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW mesh-  index quad  for FLOOR--------------------------------------------------------------------------
		std::vector<Vertex> vertices_quad_3v_3n_2t;
		std::vector<unsigned int> vec_quad_indices;

		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : planeVerticesSM_3v_3n_2t)
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
		
		msp_mTextureFloor = std::make_shared<Texture>("res/textures/wood.png", TextureType::DIFFUSE);
		msp_mTextures1.push_back(msp_mTextureFloor);
		m_mesh_quad = std::make_unique<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_mTextures1);
		
		m_ShaderMesh = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/01_Shadow_Mapping/S01_SM_02_RenderingShadows_A_BlinnPhong.Shader");

		// NEW mesh--CUBE--------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_CUBE_3v_3n_2t;
		std::vector<unsigned int> indices0;
		i = 0;
		for (float val : cube1_SM_vertices_3v_3n_2t)
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
				vertices_CUBE_3v_3n_2t.emplace_back(temp_pos, temp_norm, temp_texC);
				i = -1; // Restart counter...
				break;
			default:
				break;
			}
			i++;
		}

		// Textures - Shared pointer
		msp_mTextureCube = std::make_shared<Texture>("res/textures/wood.png", TextureType::DIFFUSE);
		msp_mTextures2.push_back(msp_mTextureCube);
		m_mesh_cube = std::make_unique<Mesh>(vertices_CUBE_3v_3n_2t, indices0, msp_mTextures2);

		m_ShaderA_DepthMap = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/01_Shadow_Mapping/S01_SM_02_RenderingShadows_B_DepthMap.Shader");
		// 
		// FrameBuffer
		FramebufferSetup(m_SHADOW_WIDTH, m_SHADOW_HEIGHT);

		// Quad already x[-1, 1], y[-1, 1]
		m_ScreenVBO = std::make_unique<VertexBuffer>(quadScreenVertices_2v_2t, 6 * (2 + 2) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_ScreenVAO = std::make_unique<VertexArray>();
		m_ScreenVAO->AddBuffer(*m_ScreenVBO, layout);
		m_ShaderScreen = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/01_Shadow_Mapping/S01_SM_02_RenderingShadows_C_Texture.Shader");
		
		m_ShaderFinal = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/01_Shadow_Mapping/S01_SM_02_RenderingShadows_D_RenderingShadows.Shader");

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T01_SM_02_RenderingShadows::~T01_SM_02_RenderingShadows()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T01_SM_02_RenderingShadows::OnUpdate(float deltaTime)
	{
	}

	void T01_SM_02_RenderingShadows::OnRender(GLFWwindow* window)
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

		// Disable blending for manual discard-----------------------------
		GLCall(glDisable(GL_BLEND));

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

		// Light space transform
		float near_plane = 1.0f, far_plane = 15;// 7.5;
		glm::mat4 lightProjection = glm::ortho(-10.0f, +10.0f, -10.0f, +10.0f, near_plane, far_plane);

		// Calculate position/direction of light 
		m_lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
		if (m_b_light_move_active)
		{
			m_lightPos.x = (float)cos(glfwGetTime()/10) * 5.0f;
			m_lightPos.y = abs((float)sin(glfwGetTime()/10) * 5.0f);
		}
		// Light Direction/Position
		m_lightDirection = glm::vec3(0.f) - m_lightPos;

		glm::mat4 lightView = glm::lookAt(m_lightPos, glm::vec3(.0f), glm::vec3(.0f, 1.0f, .0f));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		GLCall(glEnable(GL_DEPTH_TEST));

		//-------------------------------------------------------------------------------------------------------------
		// Render scene as normal (Blinn-Phong Lighting)
		if (m_i_DP_Visualizing_opt == 0)
		{	// Floor
			m_ShaderMesh->Bind();

			SetLighting(m_ShaderMesh);

			//MVP
			model = glm::mat4(1.0f);
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);
			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
			m_mesh_quad->Draw(m_ShaderMesh);
		
			// 1st Cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderMesh, model, view, proj, m_b_rotating);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderMesh, model, view, proj, false);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.25));
			DrawCube(m_ShaderMesh, model, view, proj, m_b_rotating);
		}

		//-------------------------------------------------------------------------------------------------------------
		// Render scene as seeing from light position (direction)
		if (m_i_DP_Visualizing_opt == 1)
		{	// Floor
			m_ShaderA_DepthMap->Bind();

			//MVP
			model = glm::mat4(1.0f);
			mvp = lightProjection * lightView * model;
			m_ShaderA_DepthMap->SetUniformMat4f("u_mvp", mvp);
			m_mesh_quad->Draw(m_ShaderA_DepthMap);

			// 1st Cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, false, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.25));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);
		}

		//-------------------------------------------------------------------------------------------------------------
		// Store DepthMap in a framebuffer_Texture GL_DEPTH_COMPONENT and show texture in a QUAD!
		if (m_i_DP_Visualizing_opt == 2)
		{
			glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
			//--------------------------------------------------------------------------------------------
			// Framebuffer A (Draw Scene into texture/renderbuffer)
			//--------------------------------------------------------------------------------------------
			// bind to framebuffer and draw scene as we normally would to color texture
			m_fbo.Bind();
			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClear(GL_DEPTH_BUFFER_BIT);

			m_ShaderA_DepthMap->Bind();


			//MVP
			model = glm::mat4(1.0f);
			mvp = lightProjection * lightView * model;
			m_ShaderA_DepthMap->SetUniformMat4f("u_mvp", mvp);
			m_mesh_quad->Draw(m_ShaderA_DepthMap);

			// 1st Cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, false, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.25));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);

			//--------------------------------------------------------------------------------------------
			// Framebuffer B (Draw Texture in a quad!)
			//--------------------------------------------------------------------------------------------

			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			m_fbo.Unbind();
			GLCall(glDisable(GL_DEPTH_TEST));	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_ShaderScreen->Bind();
			//m_ShaderScreen->SetUniform1i("u_i_postproc_effect", false);// m_i_postproc_effect);
			m_ScreenVAO->Bind();
			// use the color attachment texture as the texture of the quad plane
			m_fbo.TextureBind(0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			
			GLCall(glEnable(GL_DEPTH_TEST));
		}

		//-------------------------------------------------------------------------------------------------------------
		// Render scene as seeing from light position (direction)
		if (m_i_DP_Visualizing_opt == 3)
		{	// Floor

			glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
			//--------------------------------------------------------------------------------------------
			// Framebuffer A (Draw Scene into texture/renderbuffer)
			//--------------------------------------------------------------------------------------------
			// bind to framebuffer and draw scene as we normally would to color texture
			m_fbo.Bind();
			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClear(GL_DEPTH_BUFFER_BIT);

			m_ShaderA_DepthMap->Bind();

			//MVP
			model = glm::mat4(1.0f);
			mvp = lightProjection * lightView * model;
			m_ShaderA_DepthMap->SetUniformMat4f("u_mvp", mvp);
			m_mesh_quad->Draw(m_ShaderA_DepthMap);

			// 1st Cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, false, true);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.25));
			DrawCube(m_ShaderA_DepthMap, model, lightView, lightProjection, m_b_rotating, true);

			//--------------------------------------------------------------------------------------------
			// Framebuffer B (Draw Texture in a quad!)
			//--------------------------------------------------------------------------------------------
			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			m_fbo.Unbind();
			GLCall(glEnable(GL_DEPTH_TEST));	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
			glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			
			
			m_ShaderFinal->Bind();

			m_fbo.TextureBind(5);
			m_ShaderFinal->SetUniform1i("u_shadowMap", 5);

			m_ShaderFinal->SetUniform1i("u_b_gamma", m_b_gamma);
			m_ShaderFinal->SetUniform1f("u_f_gamma", m_f_gamma);
			m_ShaderFinal->SetUniform1i("u_b_quadratic", m_b_quadratic);

			SetLighting(m_ShaderFinal);

			m_ShaderFinal->SetUniformMat4f("u_lightSpaceMatrix", lightSpaceMatrix);

			//MVP
			model = glm::mat4(1.0f);
			mvp = proj * view * model;
			m_ShaderFinal->SetUniformMat4f("u_mvp", mvp);
			m_ShaderFinal->SetUniformMat4f("u_model", model);
			m_ShaderFinal->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
			m_mesh_quad->Draw(m_ShaderFinal);

			// 1st Cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderFinal, model, view, proj, m_b_rotating);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderFinal, model, view, proj, false);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.25));
			DrawCube(m_ShaderFinal, model, view, proj, m_b_rotating);
		}
	}

	void T01_SM_02_RenderingShadows::OnImGuiRender()
	{
		ImGui::Text("Shadow Map!");
		IMGUI_FPS;

		//ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		//ImGui::Text("Press M to active/disable mouse!");
		//ImGui::Text("Press N to active/disable mouse!");
		//ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		//ImGui::Text("");
		ImGui::Checkbox("Blinn-Phong Light", &m_b_blinn);
		ImGui::Checkbox("Disable adapt shininess", &m_b_disable_adapt_shininess);
		ImGui::Checkbox("Active Specular texture", &m_b_specular_texture);


		ImGui::Text("");
		ImGui::Checkbox("Ambient Light", &m_b_ambient);
		ImGui::Checkbox("Diffuse Light", &m_b_diffuse);
		ImGui::Checkbox("Specular Light", &m_b_specular);
		//ImGui::Checkbox("Emission Light", &m_b_emission);

		ImGui::Text("0 - Normal Blinn-Phong Lighting");
		ImGui::Text("1 - Scene Depth Map From Light");
		ImGui::Text("2 - Quad: DepthMap Texture");
		ImGui::Text("3 - Rendering shadows");
		ImGui::SliderInt("Choise", &m_i_DP_Visualizing_opt, 0, 3);

		ImGui::Checkbox("Gamma correction", &m_b_gamma);
		ImGui::SliderFloat("val:", &m_f_gamma, 1.5f, 3.0f);
		ImGui::Checkbox("Attenuatic Quadratic", &m_b_quadratic);

		ImGui::Text("");
		ImGui::Checkbox("Cubes Rotating", &m_b_rotating);
		ImGui::Checkbox("Sunrise animation", &m_b_light_move_active);

		//ImGui::Text("");
		//ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T01_SM_02_RenderingShadows::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T01_SM_02_RenderingShadows::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
		FramebufferSetup(m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
	}

	void T01_SM_02_RenderingShadows::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T01_SM_02_RenderingShadows::SetLighting(std::shared_ptr<Shader> shader)
	{
		shader->SetUniform3fv("light.direction", m_lightDirection);

		// IMPORTANT! material.ambient & material.diffuse are the same so ratio on ambient must be on light.ambient!!!!!
		shader->SetUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
		shader->SetUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
		shader->SetUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
		shader->SetUniform1f("material.shininess", 64.0f);

		// View position
		shader->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

		// Active Phong lighting componetst
		shader->SetUniform1i("u_b_ambient", m_b_ambient);
		shader->SetUniform1i("u_b_diffuse", m_b_diffuse);
		shader->SetUniform1i("u_b_specular", m_b_specular);
		//m_ShaderMesh->SetUniform1i("u_b_emission", m_b_emission);

		shader->SetUniform1i("u_b_specular_texture", m_b_specular_texture);

		shader->SetUniform1i("u_b_blinn", m_b_blinn);
	}

	void T01_SM_02_RenderingShadows::DrawCube(std::shared_ptr<Shader> shader, glm::mat4 & model, glm::mat4 & view, glm::mat4 & proj, bool rotating, bool depthmap)
	{
		if (rotating)
		{
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
	
		glm::mat4 mvp = proj * view * model;
		shader->SetUniformMat4f("u_mvp", mvp);
		if (!depthmap)
		{
			shader->SetUniformMat4f("u_model", model);
			shader->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
		}
		m_mesh_cube->Draw(shader);
	}

	void T01_SM_02_RenderingShadows::FramebufferSetup(int width, int height)
	{
		if (!m_fbo.InitializeDepthMap(width, height, false))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;
	}

}
