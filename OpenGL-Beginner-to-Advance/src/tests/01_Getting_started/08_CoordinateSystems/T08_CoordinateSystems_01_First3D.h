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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	class T08_CoordinateSystems_01_First3D : public Test
	{
	public:
		T08_CoordinateSystems_01_First3D();
		~T08_CoordinateSystems_01_First3D();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Texture> m_Texture0, m_Texture1;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;
		float m_f_mixFactor, m_f_fov;
		bool m_b_depth_test_active, m_b_depth_test_active_i_1;
		bool m_b_more_cubes, m_b_more_cubes_i_1;
	};
}
