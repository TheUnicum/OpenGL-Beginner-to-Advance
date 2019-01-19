#pragma once

#include "tests/Test.h"

#include <iostream>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

#include "Renderer.h"

#include <memory>

#include "GLFW/glfw3.h"

namespace test {

	class T05_Shader_02_More_Attributes: public Test
	{
	public:
		T05_Shader_02_More_Attributes();
		~T05_Shader_02_More_Attributes();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;
	};
}
