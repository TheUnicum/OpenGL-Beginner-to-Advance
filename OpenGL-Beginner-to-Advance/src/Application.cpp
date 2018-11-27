#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Utility.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //GLFW_OPENGL_COMPAT_PROFILE
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);	// active or deactive syncronize frame rate to 60Hz
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		+0.5f, +0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, +0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	{	// make a scope to force delete a VertexBuffer before context windows get distroyed
		VertexArray va;

		VertexBuffer vb(positions, 4 * (2 + 3 + 2) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(3);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		glm::mat4 proj = glm::ortho(+0.0f, +960.0f, 0.0f, +540.0f, -1.0f, +1.0f); // Ratio4:3
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f));

		Shader shader("res/shaders/Basic.shader");

		Texture texture_A("res/textures/grass.png");
		Texture texture_B("res/textures/blending_transparent_window.png");
		texture_A.Bind(0);
		texture_B.Bind(1);

		shader.Unbind();
		va.Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;


		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		glm::vec3 translation_A(+200.0f, 200, 0);
		glm::vec3 translation_B(+400.0, 200, 0);
		float rotation_A_Z = 0;
		float rotation_B_Z = 0;
		glm::vec3 scaleRatio_A(1.0f);
		glm::vec3 scaleRatio_B(1.0f);

		bool flashing_color = false;

		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();
			
			{
				// ---Model Matrix = T * R * S
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_A);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(+1.0f, +0.0f, +0.0f));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(+0.0f, +1.0f, +0.0f));
				model = glm::rotate(model, glm::radians(rotation_A_Z), glm::vec3(+0.0f, +0.0f, +1.0f));
				model = glm::scale(model, glm::vec3(1.f * 200));
				model = glm::scale(model, scaleRatio_A);
				// ------------MVP-------------- // Model/ View/ Projection
				glm::mat4 mvp = proj * view * model;

				shader.Bind();
				shader.SetUniform1i("u_flashing_color", flashing_color);
				shader.SetUniform4f("u_Color", r, r, r, 1.0f);
				shader.SetUniform1i("u_Texture", 0);
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			{
				// ---Model Matrix = T * R * S
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_B);
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(+1.0f, +0.0f, +0.0f));
				model = glm::rotate(model, glm::radians(0.0f), glm::vec3(+0.0f, +1.0f, +0.0f));
				model = glm::rotate(model, glm::radians(rotation_B_Z), glm::vec3(+0.0f, +0.0f, +1.0f));
				model = glm::scale(model, glm::vec3(1.f * 200));
				model = glm::scale(model, scaleRatio_B);
				// ------------MVP-------------- // Model/ View/ Projection
				glm::mat4 mvp = proj * view * model;

				shader.Bind();
				shader.SetUniform1i("u_flashing_color", flashing_color);
				shader.SetUniform4f("u_Color", r, r, r, 1.0f);
				shader.SetUniform1i("u_Texture", 1);
				shader.SetUniformMat4f("u_MVP", mvp);

				renderer.Draw(va, ib, shader);
			}

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			{
				ImGui::SliderFloat3("Translation_A", &translation_A.x, 0.0f, 960.0f); 
				ImGui::SliderFloat("Rotation_A AxisZ", &rotation_A_Z, 0.0f, 360.0f);
				ImGui::SliderFloat3("Scale_A", &scaleRatio_A.x, 0.1f, 3.0f);

				ImGui::SliderFloat3("Translation_B", &translation_B.x, 0.0f, 960.0f);
				ImGui::SliderFloat("RotationB AxisZ", &rotation_B_Z, 0.0f, 360.0f);
				ImGui::SliderFloat3("Scale_B", &scaleRatio_B.x, 0.1f, 3.0f);

				ImGui::Checkbox("Flashing Color", &flashing_color);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

