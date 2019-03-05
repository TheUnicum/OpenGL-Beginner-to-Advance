#include "T06_Textures_04.h"

namespace test {

	T06_Textures_04::T06_Textures_04()
		: m_scaleTex(1.0f),
		m_b_clamp_to_edge(true), m_b_clamp_to_edge_i_1(false),
		m_b_clamp_to_border(false), m_b_clamp_to_border_i_1(false),
		m_b_repeat(false), m_b_repeat_i_1(false),
		m_b_mirrored_repeat(false), m_b_mirrored_repeat_i_1(false),

		m_b_filter_linear(true), m_b_filter_linear_i_1(false),
		m_b_filter_nearest(false), m_b_filter_nearest_i_1(false),

		m_b_mipmap(false), m_b_mipmap_i_i(false)
	{
		float positions[] = {
			-0.9f, -0.9f, 0.0f, 0.0f,
			+0.9f, -0.9f, 1.0f, 0.0f,
			+0.9f, +0.9f, 1.0f, 1.0f,
			-0.9f, +0.9f, 0.0f, 1.0f
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

		m_Shader = std::make_unique<Shader>("src/tests/01_Getting_started/06_Textures/S06_Textures_04.Shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/LittleAwesomeImage.png");
		m_Texture->Bind();
		m_Shader->SetUniform1i("u_Texture", 0);
	}

	T06_Textures_04::~T06_Textures_04()
	{
	}

	void T06_Textures_04::OnUpdate(float deltaTime)
	{
	}

	void T06_Textures_04::OnRender()
	{
		bool b_reload_texture = false;

		// Texture Wrapping------------------------------------------
		if (m_b_clamp_to_edge && !m_b_clamp_to_edge_i_1)
		{
			m_wrap_s = GL_CLAMP_TO_EDGE;
			ResetAllBoolWrap();
			m_b_clamp_to_edge = true;
			b_reload_texture = true;
		}

		if (m_b_clamp_to_border && !m_b_clamp_to_border_i_1)
		{
			m_wrap_s = GL_CLAMP_TO_BORDER;
			ResetAllBoolWrap();
			m_b_clamp_to_border = true;
			b_reload_texture = true;
		}

		if (m_b_repeat && !m_b_repeat_i_1)
		{
			m_wrap_s = GL_REPEAT;
			ResetAllBoolWrap();
			m_b_repeat = true;
			b_reload_texture = true;
		}

		if (m_b_mirrored_repeat && !m_b_mirrored_repeat_i_1)
		{
			m_wrap_s = GL_MIRRORED_REPEAT;
			ResetAllBoolWrap();
			m_b_mirrored_repeat = true;
			b_reload_texture = true;
		}
		m_b_clamp_to_border_i_1 = m_b_clamp_to_border;
		m_b_clamp_to_edge_i_1 = m_b_clamp_to_edge;
		m_b_repeat_i_1 = m_b_repeat;
		m_b_mirrored_repeat_i_1 = m_b_mirrored_repeat;

		// Texture filtering ----------------------------------------
		if (m_b_filter_linear && !m_b_filter_linear_i_1)
		{
			m_min_filter_Map_DisActive = GL_LINEAR;
			m_min_filter_Map_Active = GL_LINEAR_MIPMAP_LINEAR;
			m_mag_filter = GL_LINEAR;
			ResetAllBoolFilter();
			m_b_filter_linear = true;
			b_reload_texture = true;
		}

		if (m_b_filter_nearest && !m_b_filter_nearest_i_1)
		{
			m_min_filter_Map_DisActive = GL_NEAREST;
			m_min_filter_Map_Active = GL_NEAREST_MIPMAP_NEAREST;
			m_mag_filter = GL_NEAREST;
			ResetAllBoolFilter();
			m_b_filter_nearest = true;
			b_reload_texture = true;
		}
		m_b_filter_linear_i_1 = m_b_filter_linear;
		m_b_filter_nearest_i_1 = m_b_filter_nearest;

		// Texture MipMapping----------------------------------------
		if ((m_b_mipmap != m_b_mipmap_i_i) || b_reload_texture)
		{
			if (m_b_mipmap)
				m_min_filter = m_min_filter_Map_Active;
			else
				m_min_filter = m_min_filter_Map_DisActive;
			b_reload_texture = true;
			m_b_mipmap_i_i = m_b_mipmap;
		}

		// Reload texture if needed...
		if (b_reload_texture)
		{
			m_Texture.reset();
			m_Texture = std::make_unique<Texture>("res/textures/LittleAwesomeImage.png", m_min_filter, m_mag_filter, m_wrap_s, m_wrap_s);
			m_Texture->Bind();
			m_Shader->SetUniform1i("u_Texture", 0);
		}

		m_Shader->Bind();
		m_Shader->SetUniform1f("u_scale", m_scaleTex);
		Renderer renderer;
		renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
	}

	void T06_Textures_04::OnImGuiRender()
	{
		ImGui::SliderFloat("Scale Texture", &m_scaleTex, 0.02f, 1.0f);

		ImGui::Text("Texture Wrapping");
		ImGui::Checkbox("Clamp to edge", &m_b_clamp_to_edge);
		ImGui::Checkbox("Clamp to border", &m_b_clamp_to_border);
		ImGui::Checkbox("Repeat", &m_b_repeat);
		ImGui::Checkbox("Mirrored Repeat", &m_b_mirrored_repeat);

		ImGui::Text("Filter type");
		ImGui::Checkbox("Linear", &m_b_filter_linear);
		ImGui::Checkbox("Nearest", &m_b_filter_nearest);

		ImGui::Text("MIPMAP");
		ImGui::Checkbox("MipMap", &m_b_mipmap);
	}


	void T06_Textures_04::ResetAllBoolWrap()
	{
		m_b_clamp_to_edge = false;
		m_b_clamp_to_border = false;
		m_b_repeat = false;
		m_b_mirrored_repeat = false;

		m_b_clamp_to_edge_i_1 = false;
		m_b_clamp_to_border_i_1 = false;
		m_b_repeat_i_1 = false;
		m_b_mirrored_repeat_i_1 = false;
	}

	void T06_Textures_04::ResetAllBoolFilter()
	{
		m_b_filter_linear = false;
		m_b_filter_nearest = false;

		m_b_filter_linear_i_1 = false;
		m_b_filter_nearest_i_1 = false;
	}

}
