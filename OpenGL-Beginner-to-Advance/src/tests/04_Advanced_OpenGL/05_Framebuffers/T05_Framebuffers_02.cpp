#include "T05_Framebuffers_02.h"
#include "GLFW/glfw3.h"

namespace test {

	T05_Framebuffers_02::T05_Framebuffers_02()
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
		m_ShaderMesh = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/05_Framebuffers/S05_Framebuffers_02.Shader");



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

		// FrameBuffer
		glGenFramebuffers(1, &m_fb);
		glGenTextures(1, &m_textureColorBuffer);
		glGenRenderbuffers(1, &m_rbo);
		FramebufferSetup(m_framebufferWidth, m_framebufferHeight);


		// Quad already x[-1, 1], y[-1, 1]
		m_ScreenVBO = std::make_unique<VertexBuffer>(quadScreenVertices_2v_2t, 6 * (2 + 2) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		m_ScreenVAO = std::make_unique<VertexArray>();
		m_ScreenVAO->AddBuffer(*m_ScreenVBO, layout);
		m_ShaderScreen = std::make_unique<Shader>("src/tests/04_Advanced_OpenGL/05_Framebuffers/S05_Framebuffers_02_Screeen.Shader");

	}

	T05_Framebuffers_02::~T05_Framebuffers_02()
	{
		glfwSwapInterval(0);

		GLCall(glDisable(GL_CULL_FACE));
		GLCall(glCullFace(GL_BACK));
		GLCall(glFrontFace(GL_CCW));
	}

	void T05_Framebuffers_02::OnUpdate(float deltaTime)
	{
	}

	void T05_Framebuffers_02::OnRender(GLFWwindow* window)
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
		glEnable(GL_DEPTH_TEST); // Enable depth testing (is disabled for rendering screen-space quad)

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Global Variables
		glm::mat4 model(1.0f);
		glm::mat4 view(1.0f);
		glm::mat4 proj(1.0f);
		glm::mat4 mvp;

		view = m_camera->GetViewMatrix();
		proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);


		{	// Mesh 1st Cube
			m_ShaderMesh->Bind();

			// Model Matrix
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, 0.01f, -1.0f));	// 0.01 to avoid Z-fighting

			// MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);
	
			// Draw MESH
			m_mesh->Draw(m_ShaderMesh);
		}

		{	// Mesh 2nd Cube
			m_ShaderMesh->Bind();
			model = glm::mat4(1.0f);

			// Model Matrix
			model = glm::translate(model, glm::vec3(2.0f, 0.01f, 0.0f));	// 0.01 to avoid Z-fighting

			// MVP
			mvp = proj * view * model;
			m_ShaderMesh->SetUniformMat4f("u_mvp", mvp);

			// Draw MESH
			m_mesh->Draw(m_ShaderMesh);
		}

		// Disable face culling for bidimensional object
		GLCall(glDisable(GL_CULL_FACE));
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

			m_mesh_quad->Draw(m_ShaderMesh);
		}


		//--------------------------------------------------------------------------------------------
		// Framebuffer B (Draw Texture in a quad!)
		//--------------------------------------------------------------------------------------------

		// now bind back to the default framebuffer and draw a quad plane with the attached framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);	//  disable depth test so screen-space quad isn't discard due depth test.
		
		// clear all relevant buffers
		// set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		m_ShaderScreen->Bind();
		m_ShaderScreen->SetUniform1i("u_i_postproc_effect", m_i_postproc_effect);
		m_ScreenVAO->Bind();
		// use the color attachment texture as the texture of the quad plane
		glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);


	}

	void T05_Framebuffers_02::OnImGuiRender()
	{
		ImGui::Text("Framebuffer - Base");
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

		ImGui::Text("Options:");
		ImGui::Checkbox("Disable VSync", &m_b_VSync_disabled);
	}

	void T05_Framebuffers_02::OnProcessInput(GLFWwindow * window, float deltaTime)
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

	void T05_Framebuffers_02::framebuffer_size_callback(GLFWwindow * window, int width, int height)
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

	void T05_Framebuffers_02::mouse_callback(GLFWwindow * window, double xpos, double ypos)
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
	
	
	void T05_Framebuffers_02::FramebufferSetup(int width, int height)
	{
		// - We have to attach at least one buffer(color, depth or stencil buffer).
		// - There should be at least one color attachment.
		// -  All attachments should be complete as well(reserved memory).
		// - Each buffer should have the same number of samples.

		// 1- framebuffer configuration 
		// ----------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, m_fb);

		// 2- create a color attachment texture
		glBindTexture(GL_TEXTURE_2D, m_textureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_framebufferWidth, m_framebufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		// 3- attach it to currently bound framebuffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer, 0);

		// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		// use a single renderbuffer object for both a depth & stencil buffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_framebufferWidth, m_framebufferHeight);

		// 5- attach it to currently bound framebuffer object
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFERS:: Framebuffer is not complete!" << std::endl;

		// 7- Unbind FrameBuffer and reset to default
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


