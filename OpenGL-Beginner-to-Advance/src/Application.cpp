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

#include "tests/TestClearColor.h"

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

		test::TestMenu* subtestMenu_A = new test::TestMenu(currentTest, "SUBMENU_Prova_A");
		subtestMenu_A->RegisterTest<test::TestClearColor>("Clear Color SUB");

		test::TestMenu* subtestMenu_B = new test::TestMenu(currentTest, "SUBMENU_Prova_B");
		subtestMenu_B->RegisterTest<test::TestClearColor>("Clear Color SUB");


		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestClearColor>("Clear Color 02");
		testMenu->RegisterTest<test::TestClearColor>("Clear Color 03");
		testMenu->RegisterMenu(*subtestMenu_A);
		testMenu->RegisterTest<test::TestClearColor>("Clear Color 04");
		testMenu->RegisterMenu(*subtestMenu_B);

		test::TestMenu* subLevel1 = new test::TestMenu(currentTest, "Level1");
		test::TestMenu* subLevel2 = new test::TestMenu(currentTest, "Level2");
		test::TestMenu* subLevel3 = new test::TestMenu(currentTest, "Level3");
		testMenu->RegisterMenu(*subLevel1);
		subLevel1->RegisterMenu(*subLevel2);
		subLevel2->RegisterTest<test::TestClearColor>("Clear Color In LEVEL2");
		subLevel2->RegisterMenu(*subLevel3);

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
