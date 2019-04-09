#include "T07_Bloom_01_Base.h"
#include "GLFW/glfw3.h"

namespace test {

	T07_Bloom_01_Base::T07_Bloom_01_Base()
		: m_f_fov(45.0f),
		m_b_depth_test_active(true), m_b_depth_test_active_i_1(false),
		m_b_firstMouse(true),
		m_mouse_lock(false),
		m_b_VSync_disabled(false), m_b_VSync_disabled_i_1(false),
		m_framebufferWidth(WINDOW_WIDTH), m_framebufferHeight(WINDOW_HEIGHT),

		//HDR High Dynamic Range
		m_i_Tone_mapping_Type(0), m_f_hdr_expose(1.0f),
		m_b_disable_Floating_point_fb(false), m_b_disable_Floating_point_fb_i_1(false),
		m_i_fbo_colorbuffer_selected(0), m_b_bloom(false), m_b_bloom_debug(false), m_i_bloom_intensity(10)
	{
		// Initialize camera
		m_camera = std::make_unique<Camera>(glm::vec3(-8.0f, 2.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -20.f, 0.f);
		//m_camera->ResetYawPitch();
		m_camera->ProcessMouseMovement(0, 0);

		#include "E00_cube_vertices.h"

		// NEW mesh----------------------------------------------------------------------------------------
		std::vector<unsigned int> indices0;
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
		m_mesh_floor = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_Textures);
		
		msp_mTextureCube = std::make_shared<Texture>("res/textures/container2.png", TextureType::DIFFUSE); // ontainer2.png
		msp_TextureCube.push_back(msp_mTextureCube);
		m_mesh_cube = std::make_unique<Mesh>(vertices_3v_3n_2t, indices0, msp_TextureCube);

		m_ShaderMesh = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/07_Bloom/S07_Bloom_01_Base.Shader");

		m_ShaderLight = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/07_Bloom/S00_Color_01a_Light.Shader");

		

		//  VSync / Enabel & Disable
		glfwSwapInterval(1);

		// FrameBuffer
		FramebufferSetup(m_framebufferWidth, m_framebufferHeight);
		
;
		//m_pingPongFBOs.push_back(std::make_shared<FrameBuffer>());
		//m_pingPongFBOs.push_back(std::make_shared<FrameBuffer>());
		//if (!m_pingPongFBOs[0]->Initialize(m_framebufferWidth, m_framebufferHeight, GL_RGB16F, 1))
		//	std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;
		//if (!m_pingPongFBOs[1]->Initialize(m_framebufferWidth, m_framebufferHeight, GL_RGB16F, 1))
		//	std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

		//m_fbo_double.Initialize(m_framebufferWidth, m_framebufferHeight, GL_RGB16F, 2);


		// Quad already x[-1, 1], y[-1, 1]
		m_ScreenVBO = std::make_unique<VertexBuffer>(quadScreenVertices_2v_2t, 6 * (2 + 2) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_ScreenVAO = std::make_unique<VertexArray>();
		m_ScreenVAO->AddBuffer(*m_ScreenVBO, layout);
		m_ShaderScreen = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/07_Bloom/S07_Bloom_01_Base_Screeen.Shader");
		m_ShaderBlur = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/07_Bloom/S07_Bloom_01_Blur.Shader");
		m_ShaderFinal = std::make_unique<Shader>("src/tests/05_Advanced_Lighting/07_Bloom/S07_Bloom_01_Final_Blending.Shader");

		//unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		
		////char* message = (char*)alloca(length * sizeof(char));
		//int length = 2;
		//unsigned int* attachments = (unsigned int*)alloca(length * sizeof(unsigned int));
		//for (unsigned int i = 0; i < length; i++)
		//	attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		////unsigned int attachments1[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		//glDrawBuffers(2, attachments);


		//Lenum drawBuffers[] = { GL_NONE, GL_NONE, GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT4 };

	}

	T07_Bloom_01_Base::~T07_Bloom_01_Base()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T07_Bloom_01_Base::OnUpdate(float deltaTime)
	{
	}

	void T07_Bloom_01_Base::OnRender(GLFWwindow* window)
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


		if (m_b_disable_Floating_point_fb != m_b_disable_Floating_point_fb_i_1)
		{
			FramebufferSetup(m_framebufferWidth, m_framebufferHeight);
			m_b_disable_Floating_point_fb_i_1 = m_b_disable_Floating_point_fb;
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
		m_fbo_double.Bind();
		glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)

		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);


		// lighting info
		// -------------
		// positions
		std::vector<glm::vec3> lightPositions;
		lightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
		lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
		lightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
		lightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));
		// colors
		std::vector<glm::vec3> lightColors;
		lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
		lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
		lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
		lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));



		if (true)
		{	// Mesh 1st Cube
			m_ShaderMesh->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
			model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));

			// MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
			m_ShaderMesh->SetUniformMat4f("u_model", model);
			m_ShaderMesh->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));

			// set lighting uniforms
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				m_ShaderMesh->SetUniform3fv("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				m_ShaderMesh->SetUniform3fv("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			}
			// View position
			//m_ShaderMesh->SetUniform3fv("u_viewPos", m_camera->GetCamPosition());

			// Draw MESH
			m_mesh_floor->Draw(m_ShaderMesh);


			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderMesh, proj, view, model);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderMesh, proj, view, model);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			DrawCube(m_ShaderMesh, proj, view, model);
	
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
			model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(1.25));
			DrawCube(m_ShaderMesh, proj, view, model);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
			model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			DrawCube(m_ShaderMesh, proj, view, model);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
			model = glm::scale(model, glm::vec3(0.5f));
			DrawCube(m_ShaderMesh, proj, view, model);


			// Lights
			m_ShaderLight->Bind();
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(lightPositions[i]));
				model = glm::scale(model, glm::vec3(0.25f));
				m_ShaderLight->SetUniform3fv("u_lightColor", lightColors[i]);
				DrawCube(m_ShaderLight, proj, view, model);
			}
		}

		m_fbo_double.Unbind();

		//--------------------------------------------------------------------------------------------
		// 2. blur bright fragment with two-pass Gaussian Blur
		//--------------------------------------------------------------------------------------------

		glDisable(GL_DEPTH_TEST);
		bool horizontal = true, first_iteration = true;
		unsigned int amount = 50;
		amount = m_i_bloom_intensity * 2;
		m_ShaderBlur->Bind();
		for (unsigned int i = 0; i < amount; i++)
		{
			m_pingPongFBOs[horizontal]->Bind();
			m_ShaderBlur->SetUniform1i("u_horizontal", horizontal);
			if (first_iteration)
			{
				m_fbo_double.m_textures[1]->Bind(0);
				first_iteration = false;
			}
			else
				m_pingPongFBOs[!horizontal]->m_textures[0]->Bind(0);
			
			m_ScreenVAO->Bind();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		
			horizontal = !horizontal;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		//--------------------------------------------------------------------------------------------
		// Framebuffer B (Draw Texture in a quad!)
		//--------------------------------------------------------------------------------------------

		// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
		//m_fbo.Unbind();
		//m_fbo_double.Unbind();
		glDisable(GL_DEPTH_TEST);	//  disable depth test so screen-space quad isn't discard due depth test.
		
		// clear all relevant buffers
		// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		if (m_b_bloom_debug)
		{
			m_ShaderScreen->Bind();
			
			m_ShaderScreen->SetUniform1i("u_i_Tone_mapping_Type", m_i_Tone_mapping_Type);
			m_ShaderScreen->SetUniform1f("u_f_hdr_expose", m_f_hdr_expose);
			
			m_ScreenVAO->Bind();
			// use the color attachment texture as the texture of the quad plane
			//m_fbo.TextureBind(0);
			//m_fbo_double.TextureBind(0);
			if (m_i_fbo_colorbuffer_selected == 2)
				m_pingPongFBOs[0]->m_textures[0]->Bind(0);
			else
				m_fbo_double.m_textures[m_i_fbo_colorbuffer_selected]->Bind(0);
				
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else
		{
			m_ShaderFinal->Bind();
			m_ShaderFinal->SetUniform1f("u_f_hdr_expose", m_f_hdr_expose);
			m_ShaderFinal->SetUniform1f("u_b_bloom", m_b_bloom);
			m_ScreenVAO->Bind();
			m_fbo_double.m_textures[0]->Bind(0);
			m_ShaderFinal->SetUniform1i("scene", 0);
			m_pingPongFBOs[0]->m_textures[0]->Bind(1);
			m_ShaderFinal->SetUniform1i("bloomBlur", 1);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}


	}

	void T07_Bloom_01_Base::OnImGuiRender()
	{
		ImGui::Text("Framebuffer - Abstracting B");
		IMGUI_FPS;

		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Text("Press M to active/disable mouse!");
		ImGui::Text("Press N to active/disable mouse!");
		ImGui::Text("Press F to fix player to ground!");
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);

		ImGui::Text("PostProcessor Options:");
		ImGui::Text("0 : None");
		ImGui::Text("1 : Reinhard tone mapping");
		ImGui::Text("2 : Enable Expose");
		ImGui::SliderInt("Type:", &m_i_Tone_mapping_Type, 0, 2);
		ImGui::SliderFloat("Expose", &m_f_hdr_expose, 0.1f, 10.0f);
		ImGui::Checkbox("Disable float point FB", &m_b_disable_Floating_point_fb);
		
		ImGui::Text("COLOR_ATTACHMENT");
		ImGui::SliderInt("nr:", &m_i_fbo_colorbuffer_selected, 0, 2);

		ImGui::Checkbox("Bloom Active", &m_b_bloom);
		ImGui::Checkbox("Bloom Debug", &m_b_bloom_debug);
		ImGui::Text("Bloom Intensity");
		ImGui::SliderInt("BI:", &m_i_bloom_intensity, 1, 30);

		ImGui::Text("Options:");
		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);


	}

	void T07_Bloom_01_Base::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T07_Bloom_01_Base::framebuffer_size_callback(GLFWwindow * window, int width, int height)
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

	void T07_Bloom_01_Base::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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

	void T07_Bloom_01_Base::FramebufferSetup(int width, int height)
	{
		int internal_format = m_b_disable_Floating_point_fb ? GL_RGB : GL_RGBA32F;// GL_RGBA16F;
		if (!m_fbo_double.Initialize(width, height, internal_format, 2))// , GL_RGBA16F))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

		if (m_pingPongFBOs.size() == 0)
		{
			m_pingPongFBOs.push_back(std::make_shared<FrameBuffer>());
			m_pingPongFBOs.push_back(std::make_shared<FrameBuffer>());
		}

		if (!m_pingPongFBOs[0]->Initialize(width, height, internal_format, 1))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;
		if (!m_pingPongFBOs[1]->Initialize(width, height, internal_format, 1))
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

	}

	void T07_Bloom_01_Base::DrawCube(std::shared_ptr<Shader> shader, glm::mat4 proj, glm::mat4 view, glm::mat4 model)
	{
		glm::mat4 mvp = proj * view * model;
		shader->SetUniformMat4f("u_mvp", mvp);
		shader->SetUniformMat4f("u_model", model);
		shader->SetUniformMat3f("u_transInvers_model", glm::mat3(glm::transpose(glm::inverse(model))));
		m_mesh_cube->Draw(shader);
	}
}


