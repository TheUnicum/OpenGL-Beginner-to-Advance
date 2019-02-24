#include "T07_Transformations_01.h"
#include "GLFW/glfw3.h"

namespace test {

	T07_Transformations_01::T07_Transformations_01()
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

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/07_Transformations/S07_Transformations_01.Shader");
		m_Shader->Bind();

		m_Texture0 = std::make_unique<Texture>("res/textures/awesomeface.png");
		//m_Texture0->Bind(0);
		m_Shader->SetUniform1i("u_Texture0", 0);

		m_Texture1 = std::make_unique<Texture>("res/textures/container.jpg");
		//m_Texture1->Bind(1);  // <---------------------------------- Important Index 1
		m_Shader->SetUniform1i("u_Texture1", 1);


		m_Shader->Unbind();
	}

	T07_Transformations_01::~T07_Transformations_01()
	{
	}

	void T07_Transformations_01::OnUpdate(float deltaTime)
	{
	}

	void T07_Transformations_01::OnRender()
	{
		//  MODEL MATRIX Correct order
		// - translation
		// - rotation
		// - scale
		{
			glm::mat4 trans(1.0f);
			trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
			trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));

			m_Shader->Bind();
			m_Texture0->Bind(0);
			m_Texture1->Bind(1);
			m_Shader->SetUniform1f("u_mixFactor", m_mixFactor);
			m_Shader->SetUniformMat4f("u_transform", trans);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		{
			glm::mat4 trans(1.0f);
			float scaleAmount = sin(glfwGetTime()) * 0.5f + 0.5f;
			trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
			trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
			trans = glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));

			m_Shader->Bind();
			m_Texture0->Bind(0);
			m_Texture1->Bind(1);
			m_Shader->SetUniform1f("u_mixFactor", m_mixFactor);
			m_Shader->SetUniformMat4f("u_transform", trans);
			Renderer renderer;
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

	}

	void T07_Transformations_01::OnImGuiRender()
	{
		ImGui::SliderFloat("Texture Mixing Factor", &m_mixFactor, 0.0f, 1.0f);
	}

}
