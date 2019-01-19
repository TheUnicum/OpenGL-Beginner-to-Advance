#include "T04_HelloTriangle.h"


namespace test {

	T04_HelloTriangle::T04_HelloTriangle()
	{
		float positions[] = {
			-0.5f, -0.5f,
			+0.5f, -0.5f,
			+0.0f, +0.5f
		};

		unsigned int m_indices[] = {
			0, 1, 2
		};

		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, 3 * (2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);

		m_VAO = std::make_unique<VertexArray>();
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(m_indices, 3);
		
		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/04_HelloTriangle/S04_HelloTriangle.Shader");
	}

	T04_HelloTriangle::~T04_HelloTriangle()
	{
	}

	void T04_HelloTriangle::OnUpdate(float deltaTime)
	{
	}

	void T04_HelloTriangle::OnRender()
	{
		m_Shader->Bind();
		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void T04_HelloTriangle::OnImGuiRender()
	{
	}
}