#include "T05_Shader_01.h"


namespace test {

	T05_Shader_01::T05_Shader_01()
	{
		float positions[] = {
			-0.5f, -0.5f,
			+0.5f, -0.5f,
			+0.0f, +0.5f
		};

		unsigned int m_indices[] = {
			0, 1, 2
		};

		int nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 3 * (2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(m_indices, 3);
		
		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/05_Shader/S05_Shader_01.Shader");
	}

	T05_Shader_01::~T05_Shader_01()
	{
	}

	void T05_Shader_01::OnUpdate(float deltaTime)
	{
	}

	void T05_Shader_01::OnRender()
	{
		m_Shader->Bind();

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		m_Shader->SetUniform4f("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void T05_Shader_01::OnImGuiRender()
	{
	}
}