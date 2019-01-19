#include "T05_Shader_02_More_Attributes.h"


namespace test {

	T05_Shader_02_More_Attributes::T05_Shader_02_More_Attributes()
	{
		float positions[] = {
			// positions // colors
			-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			+0.0f, +0.5f, 0.0f, 0.0f, 1.0f
		};

		unsigned int m_indices[] = {
			0, 1, 2
		};

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 3 * (2 + 3) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(3);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(m_indices, 3);
		
		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/05_Shader/S05_Shader_02_More_Attributes.Shader");
	}

	T05_Shader_02_More_Attributes::~T05_Shader_02_More_Attributes()
	{
	}

	void T05_Shader_02_More_Attributes::OnUpdate(float deltaTime)
	{
	}

	void T05_Shader_02_More_Attributes::OnRender()
	{
		m_Shader->Bind();
		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void T05_Shader_02_More_Attributes::OnImGuiRender()
	{
	}
}