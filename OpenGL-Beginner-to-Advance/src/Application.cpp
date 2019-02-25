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

// include tests
#include "tests/TestClearColor.h"

#include "tests/01_Getting_started/04_HelloTriangle/T04_HelloTriangle.h"

#include "tests/01_Getting_started/05_Shader/T05_Shader_01.h"
#include "tests/01_Getting_started/05_Shader/T05_Shader_02_More_Attributes.h"

#include "tests/01_Getting_started/06_Textures/T06_Textures_01.h"
#include "tests/01_Getting_started/06_Textures/T06_Textures_02.h"
#include "tests/01_Getting_started/06_Textures/T06_Textures_03.h"

#include "tests/01_Getting_started/07_Transformations/T07_Transformations_01.h"
#include "tests/01_Getting_started/07_Transformations/T07_Transformations_02.h"
#include "tests/01_Getting_started/07_Transformations/T07_Transformations_03_First3D.h"


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

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	std::cout << glGetString(GL_VERSION) << std::endl;

	{	// make a scope to force delete a VertexBuffer before context windows get distroyed
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		// 01_Getting_started
		test::TestMenu* TM_01_Getting_started = new test::TestMenu(currentTest, "Getting started");
		TM_01_Getting_started->RegisterTest<test::TestClearColor>("Clear Color test");

		// 04 Hello Triangle
		test::TestMenu* TM_04_Hello_Triangle = new test::TestMenu(currentTest, "Hello Triangle");
		TM_04_Hello_Triangle->RegisterTest<test::T04_HelloTriangle>("Hello Triangle");
		TM_01_Getting_started->RegisterMenu(*TM_04_Hello_Triangle);

		// 05 Shader
		test::TestMenu* TM_05_Shader = new test::TestMenu(currentTest, "Shader");
		TM_05_Shader->RegisterTest<test::T05_Shader_01>("Shader 01 Base");
		TM_05_Shader->RegisterTest<test::T05_Shader_02_More_Attributes>("Shader 02 More attributes");
		TM_01_Getting_started->RegisterMenu(*TM_05_Shader);

		// 06 Texture
		test::TestMenu* TM_06_Textures = new test::TestMenu(currentTest, "Textures");
		TM_06_Textures->RegisterTest<test::T06_Textures_01>("Textures 01 Base");
		TM_06_Textures->RegisterTest<test::T06_Textures_02>("Textures 02 Wrap");
		TM_06_Textures->RegisterTest<test::T06_Textures_03>("Textures 03 Multi Texture Mixing");
		TM_01_Getting_started->RegisterMenu(*TM_06_Textures);

		// 07 Transformations
		test::TestMenu* TM_07_Transformations = new test::TestMenu(currentTest, "Transformations");
		TM_07_Transformations->RegisterTest<test::T07_Transformations_01>("Transformations 01 - Basic");
		TM_07_Transformations->RegisterTest<test::T07_Transformations_02>("Transformations 02 - Projector");
		TM_07_Transformations->RegisterTest<test::T07_Transformations_03_First3D>("Transformation 03 - First 3D Model");
		TM_01_Getting_started->RegisterMenu(*TM_07_Transformations);

		// Main Menu
		testMenu->RegisterMenu(*TM_01_Getting_started);

		while (!glfwWindowShouldClose(window))
		{
			GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();

			if (currentTest)
			{
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-"))
				{
					if (dynamic_cast<test::TestMenu*>(currentTest) == nullptr)
					{
						delete currentTest;
						currentTest = testMenu;
					}
					else
						currentTest = dynamic_cast<test::TestMenu*>(currentTest)->getParentTest();
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);

			glfwPollEvents();
		}
		delete currentTest;
		if (currentTest != testMenu)
			delete testMenu;
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
