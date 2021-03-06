#include "T08_DeferredShader_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T08_DeferredShader_01::T08_DeferredShader_01()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_i_postproc_effect(0),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(-1.5f, 2.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -75.f, -20.f);
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
		m_ShaderMesh = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/08_Deferred_Shading/S08_DeferredShader_01.Shader");



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

		m_objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
		m_objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
		m_objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
		m_objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
		m_objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
		m_objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
		m_objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
		m_objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
		m_objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

		// lighing info
		// ------------
		std::srand(13);
		for (unsigned int i = 0; i < NR_LIGHTS; i++)
		{
			// calculate slightly random offsets
			float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
			float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
			m_lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
			// also calculate random color
			float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
			m_lightColors.push_back(glm::vec3(rColor, gColor, bColor));
		}


		// FrameBuffer
		FramebufferSetup(m_framebufferWidth, m_framebufferHeight);


		// Quad already x[-1, 1], y[-1, 1]
		m_ScreenVBO = std::make_unique<VertexBuffer>(quadScreenVertices_2v_2t, 6 * (2 + 2) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_ScreenVAO = std::make_unique<VertexArray>();
		m_ScreenVAO->AddBuffer(*m_ScreenVBO, layout);
		m_ShaderScreen = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/08_Deferred_Shading/S08_DeferredShader_01_Screeen.Shader");

		m_ShaderGeometryPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/08_Deferred_Shading/S08_DeferredShader_01_gBuffer.Shader");
		m_ShaderLightingPass = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/08_Deferred_Shading/S08_DeferredShader_01_deferred.Shader");
		m_ShaderLightBox = std::make_shared<Shader>("src/tests/05_Advanced_Lighting/08_Deferred_Shading/S08_DeferredShader_01_deffered_lighBox.Shader");
	}

	T08_DeferredShader_01::~T08_DeferredShader_01()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T08_DeferredShader_01::OnUpdate(float deltaTime)
	{
	}

	void T08_DeferredShader_01::OnRender(GLFWwindow* window)
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

		//--------------------------------------------------------------------------------------------
		// Framebuffer A (Draw Scene into texture/renderbuffer)
		//--------------------------------------------------------------------------------------------
		// bind to framebuffer and draw scene as we normally would to color texture
		//m_fbo.Bind();
		m_fbo_gBuffer.Bind();
		glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)

		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

		// 1. geometry pass: render scene's geometry/color data into buffer
		{	// Mesh 1st Cube
			//m_fbo.Bind();
			//m_ShaderMesh->Bind();
			m_ShaderGeometryPass->Bind();
			for (unsigned int i = 0; i < m_objectPositions.size(); i++)
			{
				// Model Matrix
				model = glm::mat4(1.0f);
				model = glm::translate(model, m_objectPositions[i]);
				model = glm::scale(model, glm::vec3(0.25f));
				mvp = proj * view * model;

				m_ShaderGeometryPass->SetUniformMat4f("u_mvp", mvp);
				m_ShaderGeometryPass->SetUniformMat4f("u_model", model);
				m_ShaderGeometryPass->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

				m_model->Draw(m_ShaderGeometryPass);
			}
		}



		if (m_i_deferred_Shader_debug == 0)
		{
			//--------------------------------------------------------------------------------------------
			// Framebuffer B (Draw Texture in a quad!)
			//--------------------------------------------------------------------------------------------

			// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
			//m_fbo.Unbind();
			m_fbo_gBuffer.Unbind();
			glDisable(GL_DEPTH_TEST);	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT);

			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(-0.5f, +0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 0);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(+0.5f, +0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 1);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(-0.5f, -0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 2);
			RenderFramebufferTexture(glm::vec3(0.5), glm::vec3(+0.5f, -0.5f, 0.0f), ((m_i_postproc_effect + 0) % 7), 3);
		}

		if (m_i_deferred_Shader_debug > 0)
		{
			m_fbo_gBuffer.Unbind();
			//glDisable(GL_DEPTH_TEST);	//  disable depth test so screen-space quad isn't discard due depth test.

			// clear all relevant buffers
			// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			m_ShaderLightingPass->Bind();
			m_fbo_gBuffer.m_textures[0]->Bind(0);
			m_fbo_gBuffer.m_textures[1]->Bind(1);
			m_fbo_gBuffer.m_textures[2]->Bind(2);
			m_ShaderLightingPass->SetUniform1i("gPosition", 0);
			m_ShaderLightingPass->SetUniform1i("gNormal", 1);
			m_ShaderLightingPass->SetUniform1i("gAlbedoSpec", 2);

			// send light relevant uniforms
			for (unsigned int i = 0; i < m_lightPositions.size(); i++)
			{
				m_ShaderLightingPass->SetUniform3fv("lights[" + std::to_string(i) + "].Position", m_lightPositions[i]);
				m_ShaderLightingPass->SetUniform3fv("lights[" + std::to_string(i) + "].Color", m_lightColors[i]);
				// Update attenuation parameters and calculate radius
				const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
				const float linear = 0.7f;
				const float quadratic = 1.8f;
				m_ShaderLightingPass->SetUniform1f("lights[" + std::to_string(i) + "].Linear", linear);
				m_ShaderLightingPass->SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", quadratic);
			}

			// View position
			m_ShaderLightingPass->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			m_ScreenVAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


		if (m_i_deferred_Shader_debug > 1)
		{
			// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
			// ----------------------------------------------------------------------------------
			glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_gBuffer.GetID());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
			// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
			// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
			// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
			glBlitFramebuffer(0, 0, m_framebufferWidth, m_framebufferHeight, 0, 0, m_framebufferWidth, m_framebufferHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			// 3. render lights on top of scene
			// --------------------------------
			m_ShaderLightBox->Bind();
			for (unsigned int i = 0; i < m_lightPositions.size(); i++)
			{
				// Model Matrix
				model = glm::mat4(1.0f);
				model = glm::translate(model, m_lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.125f));
				mvp = proj * view * model;

				m_ShaderLightBox->SetUniformMat4f("u_mvp", mvp);
				m_ShaderLightBox->SetUniform3fv("u_lightColor", m_lightColors[i]);

				m_mesh->Draw(m_ShaderLightBox);
			}

		}

	}

	void T08_DeferredShader_01::OnImGuiRender()
	{
		ImGui::Text("Framebuffer - Multi");
		IMGUI_FPS;

		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text("PostProcessor Options:");
		ImGui::Text("0 - None");
		ImGui::Text("1 - Inversion");
		ImGui::Text("2 - Grayscale");
		ImGui::Text("3 - Grayscale(weighted average)");
		ImGui::Text("4 - Kernel A (16)");
		ImGui::Text("5 - Kernel B (16)");
		ImGui::Text("6 - Blur");
		ImGui::Text("7 - Edge detection");
		ImGui::SliderInt("Num:", &m_i_postproc_effect, 0, 7);

		ImGui::Text("");
		ImGui::Text("Deferred Shader");
		ImGui::SliderInt("opt:", &m_i_deferred_Shader_debug, 0, 2);

		ImGui::Text("Options:");
		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T08_DeferredShader_01::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T08_DeferredShader_01::framebuffer_size_callback(GLFWwindow * window, int width, int height)
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

	void T08_DeferredShader_01::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	
	
	void T08_DeferredShader_01::FramebufferSetup(int width, int height)
	{
		if (!m_fbo.Initialize(width, height))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

		if (!m_fbo_gBuffer.InitializeGBuffer(width, height))
			std::cout << "ERROR::FRAMEBUFFERS:: GBUFFER is not complete!" << std::endl;
	}

	void T08_DeferredShader_01::RenderFramebufferTexture(glm::vec3 scale, glm::vec3 position, int index, int color_attch_Nr)
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
		//m_fbo.TextureBind(0);
		if (color_attch_Nr < 2)
			m_fbo_gBuffer.m_textures[color_attch_Nr]->Bind(0);
		else
			m_fbo_gBuffer.m_textures[2]->Bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}


