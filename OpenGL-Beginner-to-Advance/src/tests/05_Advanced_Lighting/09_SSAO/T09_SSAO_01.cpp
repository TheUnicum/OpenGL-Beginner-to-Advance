#include "T09_SSAO_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T09_SSAO_01::T09_SSAO_01()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_i_postproc_effect(0),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		m_lightPos(glm::vec3(2.0f, 4.0f, -2.0f)), m_lightColor(glm::vec3(0.2f, 0.2f, 0.7f)),
		m_i_kernelSize(64), m_f_radius(0.5f), m_f_bias(0.025f), m_i_power(1)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(-1.5f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -75.f, -20.f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW CUBE mesh----------------------------------------------------------------------------------------
		std::vector<Vertex> vertices_3v_3n_2t;
		glm::vec3 temp_pos, temp_norm;
		glm::vec2 temp_texC;
		int i = 0;
		for (float val : cube_Unitario_vertices_3v_3n_2t)
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
		msp_mTexture0 = std::make_shared<Texture>("res/textures/wood.png", TextureType::DIFFUSE); // ontainer2.png
		msp_Textures.push_back(msp_mTexture0);

		std::vector<unsigned int> indices0;
		m_mesh = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		//m_ShaderMesh = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_DeferredShader_01.Shader");



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
		
		msp_mTextureFloor = std::make_shared<Texture>("res/textures/metal.png", TextureType::DIFFUSE);
		msp_mTextures2.push_back(msp_mTextureFloor);
		m_mesh_quad = std::make_unique<Mesh>(vertices_quad_3v_3n_2t, vec_quad_indices, msp_mTextures2);

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);


		// NEW Model----------------------------------------------------------------------------------------
		//m_model = std::make_unique<Model>("res/objects/nanosuit_reflection/nanosuit.obj");
		m_model = std::make_unique<Model>("res/objects/nanosuit/nanosuit.obj");



		// FrameBuffer
		FramebufferSetup(m_framebufferWidth, m_framebufferHeight);


		// Quad already x[-1, 1], y[-1, 1]
		m_ScreenVBO = std::make_unique<VertexBuffer>(quadScreenVertices_2v_2t, 6 * (2 + 2) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_ScreenVAO = std::make_unique<VertexArray>();
		m_ScreenVAO->AddBuffer(*m_ScreenVBO, layout);
		m_ShaderScreen = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_DeferredShader_01_Screeen.Shader");

		m_ShaderGeometryPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_SSAO_01_pt1_gBuffer.Shader");
		m_ShaderSSAOPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_SSAO_01_pt2_ssao.Shader");
		m_ShaderBlurPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_SSAO_01_pt3_blur.Shader");
		m_ShaderLightPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_SSAO_01_pt4_LightingPass.Shader");

		m_ShaderLightBox = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/09_SSAO/S08_SSAO_01_pt5_LightBox.Shader");

		// generate sample kernel
		// ----------------------
		generateSamleKernel(m_i_kernelSize);


		// generate noise texture
		// ----------------------
		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f); // generate random floats between 0.0f and 1.0f
		std::default_random_engine generator;
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				0.0f); // rotate around z-axis (in tangent space)
			m_ssaoNoise.push_back(noise);
		}
		//m_noiseTexture = std::make_unique<Texture>();
		//m_noiseTexture->Initialize(4, 4, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
		//m_i_noiseTexture
		glGenTextures(1, &m_i_noiseTexture);
		glBindTexture(GL_TEXTURE_2D, m_i_noiseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, &m_ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	T09_SSAO_01::~T09_SSAO_01()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T09_SSAO_01::OnUpdate(float deltaTime)
	{
	}

	void T09_SSAO_01::OnRender(GLFWwindow* window)
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

		// Change sample kernel number
		if (m_i_kernelSize != m_i_kernelSize_i_1)
		{
			generateSamleKernel(m_i_kernelSize);
			m_i_kernelSize_i_1 = m_i_kernelSize;
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
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 50.0f);

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// ----------------------------------------------------------------
		{	// Mesh 1st Cube
			m_fbo_gBuffer.Bind();
			glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)
			glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Active Shader
			m_ShaderGeometryPass->Bind();

			// room cube
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 7.0f, 0.0f));
			model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
			mvp = proj * view * model;
			m_ShaderGeometryPass->SetUniformMat4f("u_mvp", mvp);
			m_ShaderGeometryPass->SetUniformMat4f("u_model", model);
			m_ShaderGeometryPass->SetUniformMat4f("u_view", view);
			m_ShaderGeometryPass->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(view * model))));
			m_ShaderGeometryPass->SetUniform1i("u_b_inverse_normals", true);
			m_mesh->Draw(m_ShaderGeometryPass);

			// nanosuit model on the floor
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 5.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f));
			mvp = proj * view * model;
			m_ShaderGeometryPass->SetUniformMat4f("u_mvp", mvp);
			m_ShaderGeometryPass->SetUniformMat4f("u_model", model);
			m_ShaderGeometryPass->SetUniformMat4f("u_view", view);
			m_ShaderGeometryPass->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(view * model))));
			m_ShaderGeometryPass->SetUniform1i("u_b_inverse_normals", false);
			m_model->Draw(m_ShaderGeometryPass);

			m_fbo_gBuffer.Unbind();
		}

		// 0.0 debug gbuffer texture rendering to a quad!
		//--------------------------------------------------------------------------------------------
		// Framebuffer B (Draw Texture in a quad!)
		//--------------------------------------------------------------------------------------------
		if (m_i_deferred_Shader_debug == 0)
		{
			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			//m_fbo.Unbind();
			//m_fbo_gBuffer.Unbind();
			glDisable(GL_DEPTH_TEST);	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(-0.5f, +0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 0);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(+0.5f, +0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 1);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(-0.5f, -0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 2);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(+0.5f, -0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 3);
		}

		if (m_i_deferred_Shader_debug > 0)
		{
			// 2. generate SSAO texture
			// ------------------------
			if (true)
			{
				bool b_only_blur = false;
				if (m_i_deferred_Shader_debug == 1)
					b_only_blur = true; // is not necessary
				else
					m_fbo_ssaoFBO.Bind();

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Active Shader
				m_ShaderSSAOPass->Bind();

				// send kernel + rotation
				for (unsigned int i = 0; i < m_i_kernelSize; i++)
					m_ShaderSSAOPass->SetUniform3fv("samples[" + std::to_string(i) + "]", m_ssaoKernel[i]);
				m_ShaderSSAOPass->SetUniformMat4f("u_proj", proj);

				// Active Texture
				m_fbo_gBuffer.m_textures[0]->Bind(0);
				m_fbo_gBuffer.m_textures[1]->Bind(1);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, m_i_noiseTexture);
				m_ShaderSSAOPass->SetUniform1i("gPosition", 0);
				m_ShaderSSAOPass->SetUniform1i("gNormal", 1);
				m_ShaderSSAOPass->SetUniform1i("texNoise", 2);

				glm::vec2 noiseScale = glm::vec2((float)m_framebufferWidth / 4.0f, (float)m_framebufferHeight / 4.0f);
				m_ShaderSSAOPass->SetUniform2fv("u_noiseScale", noiseScale);

				m_ShaderSSAOPass->SetUniform1i("u_i_kernelSize", m_i_kernelSize);
				m_ShaderSSAOPass->SetUniform1f("u_f_radius", m_f_radius);
				m_ShaderSSAOPass->SetUniform1f("u_f_bias", m_f_bias);
				m_ShaderSSAOPass->SetUniform1i("u_i_power", m_i_power);

				// Render Quad!
				m_ScreenVAO->Bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);

				m_fbo_ssaoFBO.Unbind();
			}

			// 3. blur SSAO texture to remove noise
			// ------------------------------------
			if (m_i_deferred_Shader_debug > 1)
			{
				if (m_i_deferred_Shader_debug > 2)
					m_fbo_ssaoBlurFBO.Bind();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Active Shader
				m_ShaderBlurPass->Bind();

				// Active Texture
				m_fbo_ssaoFBO.TextureBind(0);
				m_ShaderBlurPass->SetUniform1i("ssaoInput", 0);

				// Render Quad!
				m_ScreenVAO->Bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);

				m_fbo_ssaoBlurFBO.Unbind();
			}

			// 4. ligthing pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
			// -----------------------------------------------------------------------------------------------------
			if (m_i_deferred_Shader_debug > 2)
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Active Shader
				m_ShaderLightPass->Bind();

				// send light relevant uniforms
				glm::vec3 lightPosView = glm::vec3(view * glm::vec4(m_lightPos, 1.0));
				m_ShaderLightPass->SetUniform3fv("light.Position", lightPosView);
				m_ShaderLightPass->SetUniform3fv("light.Color", m_lightColor);

				// send light relevant parameters
				const float constant = 1.0f; // not that we don't send this to the shader, we assume it is always 1.0 (in our case)
				const float linear = 0.09f;
				const float quadratic = 0.032;
				m_ShaderLightPass->SetUniform1f("light.Linear", linear);
				m_ShaderLightPass->SetUniform1f("light.Quadratic", quadratic);

				// Active Texture
				m_fbo_gBuffer.m_textures[0]->Bind(0);
				m_fbo_gBuffer.m_textures[1]->Bind(1);
				m_fbo_gBuffer.m_textures[2]->Bind(2);
				m_ShaderLightPass->SetUniform1i("gPosition", 0);
				m_ShaderLightPass->SetUniform1i("gNormal", 1);
				m_ShaderLightPass->SetUniform1i("gAlbedo", 2);
				m_fbo_ssaoBlurFBO.TextureBind(3);
				m_ShaderLightPass->SetUniform1i("ssao", 3);

				// Render Quad!
				m_ScreenVAO->Bind();
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}

			//// 3. render lights on top of scene
			// --------------------------------
			if (true)
			{
				glClear(GL_DEPTH_BUFFER_BIT);

				// Active Shader
				m_ShaderLightBox->Bind();

				// Model Matrix
				model = glm::mat4(1.0f);
				model = glm::translate(model, m_lightPos);
				model = glm::scale(model, glm::vec3(0.125f));
				mvp = proj * view * model;
				m_ShaderLightBox->SetUniformMat4f("u_mvp", mvp);
				m_ShaderLightBox->SetUniform3fv("u_lightColor", m_lightColor);

				// Render Cube light!
				m_mesh->Draw(m_ShaderLightBox);
			}

		}
	}

	void T09_SSAO_01::OnImGuiRender()
	{
		ImGui::Text("Framebuffer - Multi");
		IMGUI_FPS;

		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text("PostProcessor Options:");
		//ImGui::Text("0 - None");
		//ImGui::Text("1 - Inversion");
		//ImGui::Text("2 - Grayscale");
		//ImGui::Text("3 - Grayscale(weighted average)");
		//ImGui::Text("4 - Kernel A (16)");
		//ImGui::Text("5 - Kernel B (16)");
		//ImGui::Text("6 - Blur");
		//ImGui::Text("7 - Edge detection");
		ImGui::SliderInt("Num:", &m_i_postproc_effect, 0, 7);

		ImGui::Text("");
		ImGui::Text("SSAO");
		ImGui::SliderInt("opt:", &m_i_deferred_Shader_debug, 0, 3);

		// 
		ImGui::Text("");
		ImGui::Text("Options_");
		ImGui::SliderInt("kernelSize", &m_i_kernelSize, 2, 256);
		ImGui::SliderFloat("Radius", &m_f_radius, 0.1f, 2.0f);
		ImGui::SliderFloat("Bias", &m_f_bias, 0.0f, 0.2f);
		ImGui::SliderInt("Power", &m_i_power, 0.5, 8);

		ImGui::Text("Options:");
		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T09_SSAO_01::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T09_SSAO_01::framebuffer_size_callback(GLFWwindow * window, int width, int height)
	{
		if (!window)
		{
			// 
		}
		m_framebufferWidth = width;
		m_framebufferHeight = height;
		glViewport(0, 0, width, height);
		FramebufferSetup(m_framebufferWidth, m_framebufferHeight);
	}

	void T09_SSAO_01::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	
	
	void T09_SSAO_01::FramebufferSetup(int width, int height)
	{
		// gBuffer
		if (!m_fbo_gBuffer.InitializeGBuffer(width, height))
			std::cout << "ERROR::FRAMEBUFFERS:: GBUFFER is not complete!" << std::endl;
		// SSAO
		if (!m_fbo_ssaoFBO.Initialize(width, height, GL_RED))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;
		// and blur stage
		if (!m_fbo_ssaoBlurFBO.Initialize(width, height, GL_RED))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

	}

	void T09_SSAO_01::RenderFramebufferTexture(glm::vec3 scale, glm::vec3 position, int index, int color_attch_Nr)
	{
		m_ShaderScreen->Bind();
		m_ShaderScreen->SetUniform1i("u_i_postproc_effect", index);

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, scale);
		m_ShaderScreen->SetUniformMat4f("u_model", model);
		m_ShaderScreen->SetUniform1i("u_i_gBuffer_idx", color_attch_Nr);

		m_ScreenVAO->Bind();
		// use the color attachment texture as the texture of the quad plane
		if (color_attch_Nr < 2)
			m_fbo_gBuffer.m_textures[color_attch_Nr]->Bind(0);
		else
			m_fbo_gBuffer.m_textures[2]->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	void T09_SSAO_01::generateSamleKernel(int sampleNr)
	{
		m_ssaoKernel.clear();

		std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f); // generate random floats between 0.0f and 1.0f
		std::default_random_engine generator;
		for (unsigned int i = 0; i < sampleNr; ++i)
		{
			glm::vec3 sample(
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator) * 2.0f - 1.0f,
				randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / sampleNr;

			// scale samples s.t. they're more aligned to center of kernel
			scale = lerp(1.0f, 1.0f, scale * scale);
			sample *= scale;
			m_ssaoKernel.push_back(sample);
		}
		std::cout << "generate Sample Kernel: " << m_ssaoKernel.size() << std::endl;
	}
}


