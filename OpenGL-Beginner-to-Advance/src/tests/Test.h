#pragma once

#include <GL/glew.h>
#include "Utility.h"
#include "imgui\imgui.h"

namespace test {

	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};
}