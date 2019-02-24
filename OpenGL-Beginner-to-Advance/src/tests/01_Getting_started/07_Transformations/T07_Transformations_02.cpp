#include "T07_Transformations_02.h"
#include "GLFW/glfw3.h"

namespace test {

	T07_Transformations_02::T07_Transformations_02()
		: m_mixFactor(.5f)
	{
		float positions[] = {
			-0.5f, -0.5f, 0.0f, 0.0f,
			+0.5f, -0.5f, 1.0f, 0.0f,
			+0.5f, +0.5f, 1.0f, 1.0f,
			-0.5f, +0.5f, 0.0f, 1.0f
		};

		unsigned int m_indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 4 * (2 + 2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(m_indices, 6);

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/07_Transformations/S07_Transformations_02.Shader");
		m_Shader->Bind();

		m_Texture0 = std::make_unique<Texture>("res/textures/awesomeface.png");
		//m_Texture0->Bind(0);
		m_Shader->SetUniform1i("u_Texture0", 0);

		m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
		//m_Texture1->Bind(1);  // <---------------------------------- Important Index 1
		m_Shader->SetUniform1i("u_Texture1", 1);


		m_Shader->Unbind();
	}

	T07_Transformations_02::~T07_Transformations_02()
	{
	}

	void T07_Transformations_02::OnUpdate(float deltaTime)
	{
	}

	void T07_Transformations_02::OnRender()
	{

		float inv_ratio_aspect = 960.0f / 540.0f;

		m_Shader->Bind();
		m_Texture0->Bind(0);
		m_Texture1->Bind(1);
		m_Shader->SetUniform1f("u_mixFactor", m_mixFactor);

		// Top Left Box
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(-0.5f, +0.5f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			view = view;
			proj = proj; 

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		// Top Right Box
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(+0.5f, +0.5f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			view = view;

			if (m_B_ortho_enabled)
				proj = glm::ortho(-inv_ratio_aspect, inv_ratio_aspect, -1.0f, 1.0f, -1.0f, 1.0f);
			else
				proj = glm::mat4(1.0f);

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		// Botton Left Box
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			view = view;

			if (m_B_ortho_enabled)
				proj = glm::ortho(-inv_ratio_aspect, inv_ratio_aspect, -1.0f, 1.0f, -1.0f, 1.0f);
			else
				proj = glm::mat4(1.0f);

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		// Botton Right Box
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(+0.5f, -0.5f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			
			if (m_B_perspective_enabled)
			{
				proj = glm::perspective(glm::radians(45.0f), inv_ratio_aspect, 0.1f, 100.0f);
				view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			}
			else
				proj = glm::ortho(-inv_ratio_aspect, inv_ratio_aspect, -1.0f, 1.0f, -1.0f, 1.0f);

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		// Center Box
		{
			glm::mat4 model(1.0f);
			glm::mat4 view(1.0f);
			glm::mat4 proj(1.0f);

			model = glm::translate(model, glm::vec3(+0.0f, +0.0f, 0.0f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

			float scaledistance = sin(glfwGetTime() / 4);
			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f - (scaledistance*3) ));


			if (m_B_perspective_enabled)
			{
				proj = glm::perspective(glm::radians(45.0f), inv_ratio_aspect, 0.1f, 100.0f);
			}
			else
				proj = glm::ortho(-inv_ratio_aspect, inv_ratio_aspect, -1.0f, 1.0f, -10.0f, 10.0f);

			glm::mat4 mvp = proj * view * model;

			m_Shader->SetUniformMat4f("u_mvp", mvp);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

	}

	void T07_Transformations_02::OnImGuiRender()
	{
		ImGui::SliderFloat("Texture Mixing Factor", &m_mixFactor, 0.0f, 1.0f);
		ImGui::Checkbox("enabling project/ortho (TR,BL)", &m_B_ortho_enabled);
		ImGui::Checkbox("Enabling perspective   (BR,CT)", &m_B_perspective_enabled);
	}

}
