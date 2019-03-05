#pragma once

#include "tests/Test.h"

//#include <iostream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"

#include "Renderer.h"

#include <memory>

//#include "GLFW/glfw3.h"

namespace test {

	class T06_Textures_04 : public Test
	{
	public:
		T06_Textures_04();
		~T06_Textures_04();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;
		float m_scaleTex;
		bool m_b_clamp_to_edge, m_b_clamp_to_edge_i_1;
		bool m_b_clamp_to_border, m_b_clamp_to_border_i_1;
		bool m_b_repeat, m_b_repeat_i_1;
		bool m_b_mirrored_repeat, m_b_mirrored_repeat_i_1;

		bool m_b_filter_nearest, m_b_filter_nearest_i_1;
		bool m_b_filter_linear, m_b_filter_linear_i_1;

		bool m_b_mipmap, m_b_mipmap_i_i;
	private:
		int m_wrap_s = GL_CLAMP_TO_EDGE;
		int m_min_filter_Map_DisActive = GL_NEAREST;
		int m_min_filter_Map_Active = GL_NEAREST;
		int m_mag_filter = GL_NEAREST;
		int m_min_filter = GL_NEAREST;
	private:
		void ResetAllBoolWrap();
		void ResetAllBoolFilter();
	};
}
