#pragma once

#include "tests/Test.h"

#include <iostream>

#include "Shader.h"
#include "Texture.h"

#include <memory>

//#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.h"	

#include "Mesh.h"
#include "Model.h"

namespace test {

	class T10_Instancing_04_Asteroid_Field_Instancing : public Test
	{
	public:
		T10_Instancing_04_Asteroid_Field_Instancing();
		~T10_Instancing_04_Asteroid_Field_Instancing();

		void OnUpdate(float deltaTime) override;
		void OnRender(GLFWwindow* window) override;
		void OnImGuiRender() override;
		void OnProcessInput(GLFWwindow* window, float deltaTime) override;
		// callbacks functions
		void framebuffer_size_callback(GLFWwindow* window, int width, int height) override;
		void mouse_callback(GLFWwindow* window, double xpos, double ypos) override;
	private:
		float m_f_fov;

		bool m_b_firstMouse;
		float m_lastX, m_lastY;
		std::unique_ptr<Camera> m_camera;
		// keys pressed options
		bool m_mouse_disable, m_mouse_disable_i_1;
		bool m_mouse_lock;
		bool m_key_pressed = false;
		int m_framebufferWidth, m_framebufferHeight;
		// Disable VSync 
		bool m_b_VSync_disabled, m_b_VSync_disabled_i_1;

		std::shared_ptr<Shader> m_Shader, m_ShaderInstance;

		// MODEL
		std::unique_ptr<Model> m_model_Planet, m_model_Rock;
		
		std::unique_ptr<VertexBuffer> m_VertexBuffer;

		unsigned int m_amount;
		glm::mat4* m_modelMatrices; //glm::mat4 m_modelMatrices[1000];
	};

}

