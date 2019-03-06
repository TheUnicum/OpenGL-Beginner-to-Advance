#include "T08_CoordinateSystems_01_First3D.h"
#include "GLFW/glfw3.h"

namespace test {

	T08_CoordinateSystems_01_First3D::T08_CoordinateSystems_01_First3D()
		: m_f_mixFactor(.5f), m_f_fov(45.0f),
		m_b_depth_test_active(false), m_b_depth_test_active_i_1(false)
	{
		#include "E00_cube_vertices.h"

		m_VertexBuffer = std::make_unique<VertexBuffer>(cube_vertices, 36 * (3 + 2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/08_CoordinateSystems/S08_CoordinateSystems_01_First3D.Shader");
		m_Shader->Bind();

		m_Texture0 = std::make_unique<Texture>("res/textures/awesomeface.png");
		m_Shader->SetUniform1i("u_Texture0", 0);
		m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
		m_Shader->SetUniform1i("u_Texture1", 1);

		m_Shader->Unbind();
	}

	T08_CoordinateSystems_01_First3D::~T08_CoordinateSystems_01_First3D()
	{
	}

	void T08_CoordinateSystems_01_First3D::OnUpdate(float deltaTime)
	{
	}

	void T08_CoordinateSystems_01_First3D::OnRender()
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

		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		float inv_ratio_aspect = 960.0f / 540.0f;
		m_Shader->Bind();
		m_Texture0->Bind(0);
		m_Texture1->Bind(1);
		m_Shader->SetUniform1f("u_mixFactor", m_f_mixFactor);

		if (!m_b_more_cubes)
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(+0.0f, +0.0f, +0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			float scaledistance = (float)sin(glfwGetTime() / 4);
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f - (scaledistance * 3) ));
			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f); 

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, 36, *m_Shader);
		}
		else
		{
			glm::vec3 cubePositions[] = {
					glm::vec3(0.0f,  0.0f,  0.0f),
					glm::vec3(2.0f,  5.0f, -15.0f),
					glm::vec3(-1.5f, -2.2f, -2.5f),
					glm::vec3(-3.8f, -2.0f, -12.3f),
					glm::vec3(2.4f, -0.4f, -3.5f),
					glm::vec3(-1.7f,  3.0f, -7.5f),
					glm::vec3(1.3f, -2.0f, -2.5f),
					glm::vec3(1.5f,  2.0f, -2.5f),
					glm::vec3(1.5f,  0.2f, -1.5f),
					glm::vec3(-1.3f,  1.0f, -1.5f)
			};

			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			proj = glm::perspective(glm::radians(m_f_fov), inv_ratio_aspect, 0.1f, 100.0f);

			for (unsigned int i = 0; i < sizeof(cubePositions); i++)
			{
				model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::rotate(model, (float)glfwGetTime() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));

				glm::mat4 mvp = proj * view * model;

				m_Shader->SetUniformMat4f("u_mvp", mvp);
				Renderer renderer;
				renderer.Draw(*m_VAO, 36, *m_Shader);
			}
		}
	}

	void T08_CoordinateSystems_01_First3D::OnImGuiRender()
	{
		ImGui::SliderFloat("Texture Mixing Factor", &m_f_mixFactor, 0.0f, 1.0f);
		ImGui::Checkbox("Depth Test", &m_b_depth_test_active);
		ImGui::Checkbox("More Cube", &m_b_more_cubes);
		ImGui::SliderFloat("FOV", &m_f_fov, 20.0f, 80.0f);
	}

}
