#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Utility.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);	// active or deactive syncronize frame rate to 60Hz

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[] = {
		-0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		+0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, +0.5f, 1.0f, 1.0f, 0.0f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	{	// make a scope to force delete a VertexBuffer before context windows get distroyed
		VertexArray va;

		VertexBuffer vb(positions, 4 * (2 + 3) * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(3);
		va.AddBuffer(vb, layout);

		IndexBuffer ibo(indices, 6);

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.3f, 0.3f, 0.8f, 1.0f);

		shader.Unbind();
		va.Unbind();
		vb.Unbind();
		ibo.Unbind();

		float r = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			GLCall(glClear(GL_COLOR_BUFFER_BIT));

			shader.Bind();
			shader.SetUniform4f("u_Color", r, r, r, 1.0f);

			va.Bind();
			ibo.Bind();

			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (r > 1.0f)
				increment = -0.05f;
			else if (r < 0.0f)
				increment = 0.05f;
			r += increment;

			/* Swap front and back buffers */
			GLCall(glfwSwapBuffers(window));

			/* Poll for and process events */
			GLCall(glfwPollEvents());
		}
	}

	glfwTerminate();
	return 0;
}