#pragma once

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "Utility.h"
#include "imgui\imgui.h"

#include <vector>
#include <functional>
#include <string>

namespace test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void OnProcessInput(GLFWwindow* window, float deltaTime) {}
		// callbacks functions
		virtual void framebuffer_size_callback(GLFWwindow* window, int width, int height) {}
		virtual void mouse_callback(GLFWwindow* window, double xpos, double ypos) {}
		virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer, const std::string& name = "MAIN TESTS:");

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string name)
		{
			std::cout << "Registering test: " << name << std::endl;
			m_Tests.push_back(std::make_pair("-> " + name, []() { return new T(); }));
		}

		void RegisterMenu(TestMenu& menu)
		{
			m_Tests.push_back(std::make_pair("[" + menu.m_MenuName + "]", [&]() {
				menu.setParentTest(this);
				return &menu; 
			}));
		}

		inline void setParentTest(Test* parent) { m_parentTest = parent; }
		inline Test* getParentTest() { return m_parentTest; }

	private:
		Test*& m_CurrentTest;
		Test* m_parentTest = nullptr;
		std::string m_MenuName;
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
	};
}