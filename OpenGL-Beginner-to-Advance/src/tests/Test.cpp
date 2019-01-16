#include "Test.h"

namespace test {

	TestMenu::TestMenu(Test*& currentTestPointer, const std::string& name)
		: m_CurrentTest(currentTestPointer),
		m_MenuName(name)
	{
	}

	void TestMenu::OnImGuiRender()
	{
		ImGui::Text(m_MenuName.c_str());
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_CurrentTest = test.second();
		}
	}
}