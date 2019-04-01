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

// 01_Getting_started----------------------------------------------------------
#include "tests/01_Getting_started/04_HelloTriangle/T04_HelloTriangle.h"

#include "tests/01_Getting_started/05_Shader/T05_Shader_01.h"
#include "tests/01_Getting_started/05_Shader/T05_Shader_02_More_Attributes.h"

#include "tests/01_Getting_started/06_Textures/T06_Textures_01.h"
#include "tests/01_Getting_started/06_Textures/T06_Textures_02.h"
#include "tests/01_Getting_started/06_Textures/T06_Textures_03.h"
#include "tests/01_Getting_started/06_Textures/T06_Textures_04.h"

#include "tests/01_Getting_started/07_Transformations/T07_Transformations_01.h"
#include "tests/01_Getting_started/07_Transformations/T07_Transformations_02.h"

#include "tests/01_Getting_started/08_CoordinateSystems/T08_CoordinateSystems_01_First3D.h"

#include "tests/01_Getting_started/09_Camera/T09_Camera_01_Basic.h"
#include "tests/01_Getting_started/09_Camera/T09_Camera_02_Mouse.h"
#include "tests/01_Getting_started/09_Camera/T09_Camera_03_CamClass.h"
#include "tests/01_Getting_started/09_Camera/T09_Camera_04_CamClassOpt.h"

#include "tests/02_Lighting/01_Colors/T01_Color_01.h"

#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_01.h"
#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_02.h"
#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_03_ViewSpace.h"
#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_04_Gouraud.h"
#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_05_Compare.h"
#include "tests/02_Lighting/02_BasicLighting/T02_Basic_Lighting_06_Scale.h"

#include "tests/02_Lighting/03_Materials/T03_Materials_01.h"
#include "tests/02_Lighting/03_Materials/T03_Materials_02_DifferentColors.h"
#include "tests/02_Lighting/03_Materials/T03_Materials_03_DataFile.h"

#include "tests/02_Lighting/04_LightingMaps/T04_LightingMaps_01.h"
#include "tests/02_Lighting/04_LightingMaps/T04_LightingMaps_02_Exercises.h"
#include "tests/02_Lighting/04_LightingMaps/T04_LightingMaps_03_Emission.h"

#include "tests/02_Lighting/05_LightCasters/T05_LightCaster_01_DirectL.h"
#include "tests/02_Lighting/05_LightCasters/T05_LightCaster_02_PointL.h"
#include "tests/02_Lighting/05_LightCasters/T05_LightCaster_03_SpotL.h"

#include "tests/03_Model_Loading/02_Mesh/T02_Mesh_01.h"
#include "tests/03_Model_Loading/02_Mesh/T03_Mesh_02_ibo.h"

#include "tests/03_Model_Loading/03_Model/T03_Model_01.h"

#include "tests/04_Advanced_OpenGL/01_DepthTesting/T01_DepthTesting.h"

#include "tests/04_Advanced_OpenGL/02_StencilTesting/T02_StencilTesting.h"

#include "tests/04_Advanced_OpenGL/03_Blending/T03_Blending_01.h"
#include "tests/04_Advanced_OpenGL/03_Blending/T03_Blending_02_Order.h"

#include "tests/04_Advanced_OpenGL/04_FaceCulling/T04_FaceCulling_01.h"

#include "tests/04_Advanced_OpenGL/05_Framebuffers/T05_Framebuffers_01.h"
#include "tests/04_Advanced_OpenGL/05_Framebuffers/T05_Framebuffers_02.h"
#include "tests/04_Advanced_OpenGL/05_Framebuffers/T05_Framebuffers_03_Abstract_A.h"
#include "tests/04_Advanced_OpenGL/05_Framebuffers/T05_Framebuffers_04_Abstract_B.h"
#include "tests/04_Advanced_OpenGL/05_Framebuffers/T05_Framebuffers_05_Multi.h"

#include "tests/04_Advanced_OpenGL/06_Cubemaps/T06_Cubemaps_01.h"
#include "tests/04_Advanced_OpenGL/06_Cubemaps/T06_Cubemaps_02_Reflection.h"
#include "tests/04_Advanced_OpenGL/06_Cubemaps/T06_Cubemaps_03_Model2.h"

#include "tests/04_Advanced_OpenGL/08_Advanced_GLSL/T08_Advanced_GLSL_01.h"
#include "tests/04_Advanced_OpenGL/08_Advanced_GLSL/T08_Advanced_GLSL_02_FragCoord.h"
#include "tests/04_Advanced_OpenGL/08_Advanced_GLSL/T08_Advanced_GLSL_03_UniformBuff.h"

#include "tests/04_Advanced_OpenGL/09_GeometryShader/T09_GeometryShader_01_Line.h"
#include "tests/04_Advanced_OpenGL/09_GeometryShader/T09_GeometryShader_02_Line.h"
#include "tests/04_Advanced_OpenGL/09_GeometryShader/T09_GeometryShader_03_House.h"
#include "tests/04_Advanced_OpenGL/09_GeometryShader/T09_GeometryShader_04_Exploding.h"
#include "tests/04_Advanced_OpenGL/09_GeometryShader/T09_GeometryShader_05_Fur_Normal.h"

#include "tests/04_Advanced_OpenGL/10_Instancing/T10_Instancing_01_Multi_Quad.h"
#include "tests/04_Advanced_OpenGL/10_Instancing/T10_Instancing_02_InstancedA.h"
#include "tests/04_Advanced_OpenGL/10_Instancing/T10_Instancing_03_Asteroid_Field_Base.h"
#include "tests/04_Advanced_OpenGL/10_Instancing/T10_Instancing_04_Asteroid_Field_Instancing.h"
#include "tests/04_Advanced_OpenGL/10_Instancing/T10_Instancing_05_Asteroid_Field_Tests.h"

#include "tests/04_Advanced_OpenGL/11_AntiAliasing/T11_AntiAliasing_01.h"

// Functions Declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int framebufferWidth = 0, framebufferHeight = 0;

float deltaTime = 0.0f;
float lastframe = 0.0f;

test::Test* currentTest = nullptr;

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

	// Anti Aliasing Hint
	// To Enable MultiSample Anti Aliasing uncomment this line before create a Window!
	//glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Added this lines to get framebuffer width/height and set Viewport
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	framebuffer_size_callback(window, framebufferWidth, framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Mouse callback register fuctions
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);	// active or deactive syncronize frame rate to 60Hz

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	
	// GL info :
	std::cout << glGetString(GL_VERSION) << std::endl;
	int data;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &data);
	std::cout << "MAX Vertex Uniform components: " << data << std::endl;

	{	// make a scope to force delete a VertexBuffer before context windows get distroyed
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		//test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		//-----------------------------
		// 01_Getting_started
		//-----------------------------
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
		TM_06_Textures->RegisterTest<test::T06_Textures_04>("Textures 04 Wrap 2.0");
		TM_01_Getting_started->RegisterMenu(*TM_06_Textures);

		// 07 Transformations
		test::TestMenu* TM_07_Transformations = new test::TestMenu(currentTest, "Transformations");
		TM_07_Transformations->RegisterTest<test::T07_Transformations_01>("Transformations 01 - Basic");
		TM_07_Transformations->RegisterTest<test::T07_Transformations_02>("Transformations 02 - Projector");
		TM_01_Getting_started->RegisterMenu(*TM_07_Transformations);

		// 08 Cordinate System
		test::TestMenu* TM_08_CoordinateSystems = new test::TestMenu(currentTest, "Coordinate Systems");
		TM_08_CoordinateSystems->RegisterTest<test::T08_CoordinateSystems_01_First3D>("Coordinate Systems 01 - First 3D Model");
		TM_01_Getting_started->RegisterMenu(*TM_08_CoordinateSystems);

		// 09 Camera
		test::TestMenu* TM_09_Camera = new test::TestMenu(currentTest, "Camera");
		TM_09_Camera->RegisterTest<test::T09_Camera_01_Basic>("Camera 01 - Basic(Keys) + callbacks");
		TM_09_Camera->RegisterTest<test::T09_Camera_02_Mouse>("Camera 02 - Mouse Movement");
		TM_09_Camera->RegisterTest<test::T09_Camera_03_CamClass>("Camera 03 - CAM Class");
		TM_09_Camera->RegisterTest<test::T09_Camera_04_CamClassOpt>("Camera 04 - CAM Class Opt");
		TM_01_Getting_started->RegisterMenu(*TM_09_Camera);

		//-----------------------------
		// 02 Lighting
		//-----------------------------
		test::TestMenu* TM_02_Lighting = new test::TestMenu(currentTest, "Lighting");
		
		// 01 Color
		test::TestMenu* TM_01_Color = new test::TestMenu(currentTest, "Colors");
		TM_01_Color->RegisterTest<test::T01_Color_01>("Colors 01 - Basic");
		TM_02_Lighting->RegisterMenu(*TM_01_Color);

		// 02 Basic Lighting
		test::TestMenu* TM_02_Basic_Lighting = new test::TestMenu(currentTest, "Basic Lighting");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_01>("B-Lighting 01");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_02>("B-Lighting 02 - Ex01: Moving");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_03_ViewSpace>("B-Lighting 03 - Ex02: ViewSpace");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_04_Gouraud>("B-Lighting 04 - Ex03: Gouraud Shading");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_05_Compare>("B-Lighting 05 - COMPARE SHADERS");
		TM_02_Basic_Lighting->RegisterTest<test::T02_Basic_Lighting_06_Scale>("B-Lighting 06 - Scale Test");
		TM_02_Lighting->RegisterMenu(*TM_02_Basic_Lighting);
		
		// 03 Materials
		test::TestMenu* TM_03_Materials = new test::TestMenu(currentTest, "Materials");
		TM_03_Materials->RegisterTest<test::T03_Materials_01>("Materials 01 - Basic");
		TM_03_Materials->RegisterTest<test::T03_Materials_02_DifferentColors>("Materials 02 - Different Colors");
		TM_03_Materials->RegisterTest<test::T03_Materials_03_DataFile>("Materials 03 - Data Files VRML");
		TM_02_Lighting->RegisterMenu(*TM_03_Materials);

		// 04 Lighting maps
		test::TestMenu* TM_04_Lighting_maps = new test::TestMenu(currentTest, "Lighting maps");
		TM_04_Lighting_maps->RegisterTest<test::T04_LightingMaps_01>("Lighting maps 01");
		TM_04_Lighting_maps->RegisterTest<test::T04_LightingMaps_02_Exercises>("Lighting maps 02 - Exercises");
		TM_04_Lighting_maps->RegisterTest<test::T04_LightingMaps_03_Emission>("Lighting maps 03 - Emission!!");
		TM_02_Lighting->RegisterMenu(*TM_04_Lighting_maps);

		// 05 Light Casters
		test::TestMenu* TM_05_Light_casters = new test::TestMenu(currentTest, "Light casters");
		TM_05_Light_casters->RegisterTest<test::T05_LightCaster_01_DirectL>("Light casters 01 - Direct.L.");
		TM_05_Light_casters->RegisterTest<test::T05_LightCaster_02_PointL>("Light casters 02 - Point.L.");
		TM_05_Light_casters->RegisterTest<test::T05_LightCaster_03_SpotL>("Light casters 02 - Spot.L.");
		TM_02_Lighting->RegisterMenu(*TM_05_Light_casters);

		//-----------------------------
		// 03 Model Loading
		//-----------------------------
		test::TestMenu* TM_03_Model_Loading = new test::TestMenu(currentTest, "Model Loading");

		// 02 Mesh
		test::TestMenu* TM_02_Mesh = new test::TestMenu(currentTest, "Mesh");
		TM_02_Mesh->RegisterTest<test::T02_Mesh_01>("Mesh 01 - Class");
		TM_02_Mesh->RegisterTest<test::T03_Mesh_02_ibo>("Mesh 02 - glDrawElements(ibo)");
		TM_03_Model_Loading->RegisterMenu(*TM_02_Mesh);

		// 03 Model
		test::TestMenu* TM_03_Model = new test::TestMenu(currentTest, "Model");
		TM_03_Model->RegisterTest<test::T03_Model_01>("Model 01 - Class");
		TM_03_Model_Loading->RegisterMenu(*TM_03_Model);

		//-----------------------------
		// 04 Advanced OpenGL
		//-----------------------------

		test::TestMenu* TM_04_Advanced_OpenGL = new test::TestMenu(currentTest, "Advanced OpenGL");

		// 01 Depth testing
		test::TestMenu* TM_01_DepthTesting = new test::TestMenu(currentTest, "Depth testing");
		TM_01_DepthTesting->RegisterTest<test::T01_DepthTesting>("Depth testing 01 - Basic");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_01_DepthTesting);

		// 02 Stencil Testing
		test::TestMenu* TM_02_StencilTesting = new test::TestMenu(currentTest, "Stencil testing");
		TM_02_StencilTesting->RegisterTest<test::T02_StencilTesting>("Stencil testing 01 - Basic");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_02_StencilTesting);

		// 03 Blending
		test::TestMenu* TM_03_Blending = new test::TestMenu(currentTest, "Blending");
		TM_03_Blending->RegisterTest<test::T03_Blending_01>("Blending 01 - Manual Discard");
		TM_03_Blending->RegisterTest<test::T03_Blending_02_Order>("Blending 02 - GL_BLEND Function");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_03_Blending);

		// 04 Face culling
		test::TestMenu* TM_04_FaceCulling = new test::TestMenu(currentTest, "Face Culling");
		TM_04_FaceCulling->RegisterTest<test::T04_FaceCulling_01>("Face Culling 01 - Base");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_04_FaceCulling);

		// 05 Framebuffers
		test::TestMenu* TM_05_Framebuffers = new test::TestMenu(currentTest, "FrameBuffers");
		TM_05_Framebuffers->RegisterTest<test::T05_Framebuffers_01>("Framebuffers 01 - Base");
		TM_05_Framebuffers->RegisterTest<test::T05_Framebuffers_02>("Framebuffers 02 - Improved");
		TM_05_Framebuffers->RegisterTest<test::T05_Framebuffers_03_Abstract_A>("Framebuffers 03 - Abstracting A");
		TM_05_Framebuffers->RegisterTest<test::T05_Framebuffers_04_Abstract_B>("Framebuffers 04 - Abstracting B");
		TM_05_Framebuffers->RegisterTest<test::T05_Framebuffers_05_Multi>("Framebuffers 05 - Multi");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_05_Framebuffers);

		// 06 Cubemaps
		test::TestMenu* TM_06_Cubemaps = new test::TestMenu(currentTest, "Cubemaps");
		TM_06_Cubemaps->RegisterTest<test::T06_Cubemaps_01>("Cubemaps 01 - Base");
		TM_06_Cubemaps->RegisterTest<test::T06_Cubemaps_02_Reflection>("Cubemaps 02 - Reflection/Refraction");
		TM_06_Cubemaps->RegisterTest<test::T06_Cubemaps_03_Model2>("Cubemaps 03 - Model 2.0");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_06_Cubemaps);

		// 08 Advanced GLSL
		test::TestMenu* TM_08_Advanced_GLSL = new test::TestMenu(currentTest, "Advanced GLSL");
		TM_08_Advanced_GLSL->RegisterTest<test::T08_Advanced_GLSL_01>("Advanced GLSL 01 - PointSize");
		TM_08_Advanced_GLSL->RegisterTest<test::T08_Advanced_GLSL_02_FragCoord>("GLSL02 - FragCoord/FrontFacing");
		TM_08_Advanced_GLSL->RegisterTest<test::T08_Advanced_GLSL_03_UniformBuff>("GLSL03 - Uniform buffer objects");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_08_Advanced_GLSL);

		// 09 Geometry Shader
		test::TestMenu* TM_09_Geometry_Shader = new test::TestMenu(currentTest, "Geometry Shader");
		TM_09_Geometry_Shader->RegisterTest<test::T09_GeometryShader_01_Line>("GeoS 01 - 1 Simgle File");
		TM_09_Geometry_Shader->RegisterTest<test::T09_GeometryShader_02_Line>("GeoS 02 - 3 Files, V-G-F");
		TM_09_Geometry_Shader->RegisterTest<test::T09_GeometryShader_03_House>("GeoS 03 - Build a House");
		TM_09_Geometry_Shader->RegisterTest<test::T09_GeometryShader_04_Exploding>("GeoS 04 - Exploding object");
		TM_09_Geometry_Shader->RegisterTest<test::T09_GeometryShader_05_Fur_Normal>("GeoS 05 - Normal Vectors");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_09_Geometry_Shader);

		// 10 Instancing
		test::TestMenu* TM_10_Instancing = new test::TestMenu(currentTest, "Instancing");
		TM_10_Instancing->RegisterTest<test::T10_Instancing_01_Multi_Quad>("Inst. 01 - Hundred 2D quads");
		TM_10_Instancing->RegisterTest<test::T10_Instancing_02_InstancedA>("Inst. 02 - Instanced Array");
		TM_10_Instancing->RegisterTest<test::T10_Instancing_03_Asteroid_Field_Base>("Inst. 03 - Asteroid_Field_Base");
		TM_10_Instancing->RegisterTest<test::T10_Instancing_04_Asteroid_Field_Instancing>("Inst. 04 - Asteroid_Field Inst.");
		TM_10_Instancing->RegisterTest<test::T10_Instancing_05_Asteroid_Field_Tests>("Inst. 05 - Asteroid_Field Tests");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_10_Instancing);

		// Anti Aliasing
		test::TestMenu* TM_11_Anti_Aliasing = new test::TestMenu(currentTest, "Anti Aliasing");
		TM_11_Anti_Aliasing->RegisterTest < test::T11_AntiAliasing_01>("AntiAliasing 01 - Base");
		TM_04_Advanced_OpenGL->RegisterMenu(*TM_11_Anti_Aliasing);

		//-----------------------------
		//-----------------------------
		// Main Menu
		//-----------------------------
		//-----------------------------
		testMenu->RegisterMenu(*TM_01_Getting_started);
		testMenu->RegisterMenu(*TM_02_Lighting);
		testMenu->RegisterMenu(*TM_03_Model_Loading);
		testMenu->RegisterMenu(*TM_04_Advanced_OpenGL);

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastframe;
			lastframe = currentFrame;

			// Main Input -----
			processInput(window);

			GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
			renderer.Clear();

			ImGui_ImplGlfwGL3_NewFrame();

			if (currentTest)
			{
				currentTest->OnProcessInput(window, deltaTime);
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				currentTest->OnRender(window);
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


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// Callback funcitons definitions
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimension; notes that width and
	// height will be significantly larger than specifies on retina display
	glViewport(0, 0, width, height);
	framebufferWidth = width;
	framebufferHeight = height;

	if (currentTest)
		currentTest->framebuffer_size_callback(window, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//std::cout << "MAIN -> mouse_callback : ( " << xpos << " : " << ypos << " )" << std::endl;
	if (currentTest)
		currentTest->mouse_callback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	std::cout << "scroll_callback : (" << xoffset << " : " << yoffset<< ")" << std::endl;
	if (currentTest)
		currentTest->scroll_callback(window, xoffset, yoffset);
}
