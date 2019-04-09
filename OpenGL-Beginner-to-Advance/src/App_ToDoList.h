#pragma once

/*

posponed:
- Lighting/Multiple-lights
- Advanced-OpenGL/Anti-Aliasing (MSAA Anti Aliasin and Framebuffers)

Improvements
- Cubomaps: make a test with multiple skyboxes to chose.
- Abstract <uniform buffers>

- Improve VertexBuffer layout to add Division (check T10_Instancing_02_InstancedA)
- Add glDrawArraysInstanced in MESH and MODEL

- VertexArray add get_max_element in layout for adding new layout glVertexAttribPointer


// Shadow T01_[SM_02_RenderingShadows]
Fare un test per tutte le options
	Texture DepthMap dimension Widht & Height
	Bias (None, Fixed, dinamic)
	Over sampling Texture(GL_CLAMP_TO_BORDER) see (!m_fbo.InitializeDepthMap(width, height, false or true))
-Orthographic vs projection


// 07 Bloom
Check the :
	T07_Bloom_01_Base::FramebufferSetup
	bool FrameBuffer::Initialize(int width, int height, int internalFormat, int nr_of_colorBuffers)
	m_textures.push_back(_texture_temp);

	Important not to create & push new (ptr)Texture every change of window resolution

*/
