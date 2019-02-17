#include "T06_Textures_02.h"

namespace test {

	T06_Textures_02::T06_Textures_02()
		: m_scaleTex(1.0f)
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

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/06_Textures/S06_Textures_02.Shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/LittleAwesomeImage.png");
		m_Texture->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	T06_Textures_02::~T06_Textures_02()
	{
	}

	void T06_Textures_02::OnUpdate(float deltaTime)
	{
	}

	void T06_Textures_02::OnRender()
	{
		m_Shader->Bind();
		m_Shader->SetUniform1f("u_scale", m_scaleTex);
		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void T06_Textures_02::OnImGuiRender()
	{
		ImGui::SliderFloat("Scale Texture", &m_scaleTex, 0.05f, 1.0f);
	}

}
