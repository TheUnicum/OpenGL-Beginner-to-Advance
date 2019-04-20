#include "T02_PM_01_Base.h"
#include "GLFW/glfw3.h"

namespace test {

	T02_PM_01_Base::T02_PM_01_Base()
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
		m_SHADOW_WIDTH(256), m_SHADOW_HEIGHT(256),
		m_b_gamma(false), m_f_gamma(2.2), m_i_viewShadow_index(0), m_i_PCF_type(0)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.f, +0.0f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// Light--------------------------------------------------------------------------
		m_VertexBuffer = std::make_unique<VertexBuffer>(cube_vertices_3v_3n_2t, 36 * (3 + 3 + 2) * sizeof(float));

		VertexBufferLayout layoutLight;
		layoutLight.Push<float>(3);
		layoutLight.Push<float>(3);
		layoutLight.Push<float>(2);
		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layoutLight);

		m_lightVAO = std::make_unique<VertexArray>();
		m_lightVAO->AddBuffer(*m_VertexBuffer, layoutLight);



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
		
		m_ShaderMesh = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_A_BlinnPhong.Shader");

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

		m_ShaderB_DepthMap = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_B_DepthMap.Shader");
		m_ShaderB2_DepthCubeMap = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_B2_DepthCubeMap.Shader");
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
		m_ShaderScreen = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_C_Texture.Shader");
		
		m_ShaderFinal = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_D_RenderingShadows.Shader");

		m_ShaderCubeFinal = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S02_PM_01_Base_D2_CUBE_RenderingShadows.Shader");
		m_ShaderLight = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/03_Shadows/02_Point_Shadows/S00_Color_01a_Light.Shader");
		//  VSync / Enabel & Disable
		glfwSwapInterval(1);
	}

	T02_PM_01_Base::~T02_PM_01_Base()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T02_PM_01_Base::OnUpdate(float deltaTime)
	{
	}

	void T02_PM_01_Base::OnRender(GLFWwindow* window)
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
		m_lightPos = glm::vec3(0.0f, 0.0f, 0.0f);

		//---glm::mat4 lightView = glm::lookAt(m_lightPos, glm::vec3(.0f), glm::vec3(.0f, 1.0f, .0f));
		//---glm::mat4 lightSpaceMatrix = lightProjection * lightView;


		// Light space transform
		near_plane = 1.0f, m_far_plane = 25.0f;
		float inv_ratio_aspect_shadow = (float)m_SHADOW_WIDTH / (float)m_SHADOW_HEIGHT;
		m_shadowProj = glm::perspective(glm::radians(90.0f), inv_ratio_aspect_shadow, near_plane, m_far_plane);
		if (m_shadowView.size() == 0)
		{
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
			m_shadowView.push_back(glm::lookAt(m_lightPos, m_lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
		}
		
		GLCall(glEnable(GL_DEPTH_TEST));

		//-------------------------------------------------------------------------------------------------------------
		// Render scene as normal (Blinn-Phong Lighting)
		if (m_i_DP_Visualizing_opt == 0)
		{	// Floor
			m_ShaderMesh->Bind();

			SetLighting(m_ShaderMesh);
			// cubes
			RenderScene(m_ShaderMesh, view, proj);
		}

		//-------------------------------------------------------------------------------------------------------------
		// Render scene as seeing from light position (direction)
		if (m_i_DP_Visualizing_opt == 1)
		{	// Floor
			m_ShaderB_DepthMap->Bind();

			// cubes
			RenderScene(m_ShaderB_DepthMap, m_shadowView[m_i_viewShadow_index], m_shadowProj);
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

			m_ShaderB_DepthMap->Bind();
			// cubes
			RenderScene(m_ShaderB_DepthMap, m_shadowView[m_i_viewShadow_index], m_shadowProj);


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
		// Store DepthMap in a framebuffer_Texture CUBE !!!! and trying to visualize.
		if (m_i_DP_Visualizing_opt == 3)
		{
			glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
			//--------------------------------------------------------------------------------------------
			// Framebuffer A (Draw Scene into texture/renderbuffer)
			//--------------------------------------------------------------------------------------------
			// bind to framebuffer and draw scene as we normally would to color texture
			m_fboCube.Bind();
			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClear(GL_DEPTH_BUFFER_BIT);

			m_ShaderB2_DepthCubeMap->Bind();


			for (unsigned int i = 0; i < 6; ++i)
				m_ShaderB2_DepthCubeMap->SetUniformMat4f("u_shadowMatrices[" + std::to_string(i) + "]", proj * m_shadowView[i]);
			m_ShaderB2_DepthCubeMap->SetUniform1f("u_far_plane", m_far_plane);
			m_ShaderB2_DepthCubeMap->SetUniform3fv("u_lightPos", m_lightPos);

			// cubes
			RenderScene(m_ShaderB2_DepthCubeMap);


			//--------------------------------------------------------------------------------------------
			// Framebuffer B (Draw Texture in a quad!)
			//--------------------------------------------------------------------------------------------

			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			m_fboCube.Unbind();
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
			m_fboCube.TextureBind(0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			GLCall(glEnable(GL_DEPTH_TEST));
		}


		//-------------------------------------------------------------------------------------------------------------
		// Render final scene
		if (m_i_DP_Visualizing_opt == 4)
		{
			glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
			//--------------------------------------------------------------------------------------------
			// Framebuffer A (Draw Scene into texture/renderbuffer)
			//--------------------------------------------------------------------------------------------
			// bind to framebuffer and draw scene as we normally would to color texture
			m_fboCube.Bind();
			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClear(GL_DEPTH_BUFFER_BIT);

			m_ShaderB2_DepthCubeMap->Bind();


			for (unsigned int i = 0; i < 6; ++i)
				m_ShaderB2_DepthCubeMap->SetUniformMat4f("u_shadowMatrices[" + std::to_string(i) + "]", m_shadowProj * m_shadowView[i]);
			m_ShaderB2_DepthCubeMap->SetUniform1f("u_far_plane", m_far_plane);
			m_ShaderB2_DepthCubeMap->SetUniform3fv("u_lightPos", m_lightPos);

			// cubes
			RenderScene(m_ShaderB2_DepthCubeMap);


			//--------------------------------------------------------------------------------------------
			// Framebuffer B (Draw Texture in a quad!)
			//--------------------------------------------------------------------------------------------
			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			m_fboCube.Unbind();
			GLCall(glEnable(GL_DEPTH_TEST));	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
			glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//proj and view ok


			m_ShaderCubeFinal->Bind();

			SetLighting(m_ShaderCubeFinal);
			m_ShaderCubeFinal->SetUniform1f("u_far_plane", m_far_plane);
			m_ShaderCubeFinal->SetUniform1i("u_shadows", true);

			m_ShaderCubeFinal->SetUniform1i("u_i_PCF_type", m_i_PCF_type);

			m_fboCube.TextureBind(1);
			m_ShaderCubeFinal->SetUniform1i("u_depthMap", 1);

			m_ShaderCubeFinal->SetUniform1i("u_b_gamma", m_b_gamma);
			m_ShaderCubeFinal->SetUniform1f("u_f_gamma", m_f_gamma);

			// cubes
			RenderScene(m_ShaderCubeFinal, view, proj);


			GLCall(glEnable(GL_DEPTH_TEST));
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

	void T02_PM_01_Base::OnImGuiRender()
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
		ImGui::Text("3 - Rendering cube depth map");
		ImGui::Text("4 - Rendering shadows");
		ImGui::SliderInt("Choise", &m_i_DP_Visualizing_opt, 0, 4);
		ImGui::SliderInt("PCF: ", &m_i_PCF_type, 0, 2);
		
		ImGui::Text("Choise 1 or 2");
		ImGui::SliderInt("View Idx:", &m_i_viewShadow_index, 0, 5);

		ImGui::Text("");
		ImGui::Checkbox("Gamma correction", &m_b_gamma);
		ImGui::SliderFloat("val:", &m_f_gamma, 1.5f, 3.0f);

		ImGui::Text("");
		ImGui::Checkbox("Cubes Rotating", &m_b_rotating);

		//ImGui::Text("");
		//ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T02_PM_01_Base::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T02_PM_01_Base::framebuffer_size_callback(GLFWwindow * window, int width, int height)
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

	void T02_PM_01_Base::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T02_PM_01_Base::SetLighting(std::shared_ptr<Shader> shader)
	{
		//shader->SetUniform3fv("light.direction", m_lightDirection);
		shader->SetUniform3fv("light.position", m_lightPos);

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

	void T02_PM_01_Base::DrawCube(std::shared_ptr<Shader> shader, glm::mat4 & model, glm::mat4 & view, glm::mat4 & proj, bool rotating, bool depthmap)
	{
		if (rotating)
		{
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		}
	
		if (!depthmap) // if render the depth map dont need to send to the shader the matrices
		{
			glm::mat4 mvp = proj * view * model;
			shader->SetUniformMat4f("u_mvp", mvp);
			shader->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
		}
		shader->SetUniformMat4f("u_model", model);

		m_mesh_cube->Draw(shader);
	}

	void T02_PM_01_Base::FramebufferSetup(int width, int height)
	{
		if (!m_fbo.InitializeDepthMap(width, height, false))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

		if (!m_fboCube.InitializeDepthCubeMap(width, height, false))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer CUBE is not complete!" << std::endl;
	}

	void T02_PM_01_Base::RenderScene(std::shared_ptr<Shader> shader, glm::mat4 & view, glm::mat4 & proj, bool only_model)
	{
		// room cube
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(5.0f));
		glDisable(GL_CULL_FACE);; // note that we disable culling here since we render 'inside' the cube instead of the 
		// usual 'outside' which throws off the normal culling methods.
		if (!only_model)
			shader->SetUniform1i("u_b_inverse_normals", true); // A small little hack to invert normals when drawing cube
		// from the inside so lighting still works
		DrawCube(shader, model, view, proj, false, only_model);
		if (!only_model)
			shader->SetUniform1i("u_b_inverse_normals", false);
		glEnable(GL_CULL_FACE);

		// cubes
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		DrawCube(shader, model, view, proj, m_b_rotating, only_model);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.75f));
		DrawCube(shader, model, view, proj, m_b_rotating, only_model);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		DrawCube(shader, model, view, proj, m_b_rotating, only_model);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5f));
		model = glm::scale(model, glm::vec3(0.5f));
		DrawCube(shader, model, view, proj, m_b_rotating, only_model);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0f));
		model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
		model = glm::scale(model, glm::vec3(0.75f));
		DrawCube(shader, model, view, proj, m_b_rotating, only_model);
	}

	// Render scene without set again the view and proj matrices
	void T02_PM_01_Base::RenderScene(std::shared_ptr<Shader> shader)
	{
		glm::mat4 matrix_temp = glm::mat4(1.0f);
		RenderScene(shader, matrix_temp, matrix_temp, true);
	}

}
