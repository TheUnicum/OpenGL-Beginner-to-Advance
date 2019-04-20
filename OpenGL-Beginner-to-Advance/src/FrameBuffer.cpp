#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
	: m_RendererID(0)
{
	GLCall(glGenFramebuffers(1, &m_RendererID));
}

FrameBuffer::~FrameBuffer()
{
	GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void FrameBuffer::Bind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void FrameBuffer::Unbind() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::TextureBind(unsigned int slot /*= 0*/) const
{
	m_texture.Bind(slot);
}

// Initialize FBO standard:
// 1- Attachment texture 
// 2- depth/stencil RenderBuffer
bool FrameBuffer::Initialize(int width, int height, int internalFormat)
{
	// - We have to attach at least one buffer(color, depth or stencil buffer).
	// - There should be at least one color attachment.
	// - All attachments should be complete as well(reserved memory).
	// - Each buffer should have the same number of samples.

	// 1- framebuffer configuration 
	Bind();
	// 2- create a color attachment texture
	m_texture.Initialize(width, height, internalFormat);
	// 3- attach it to currently bound framebuffer object
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.GetID(), 0));
	// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
	m_rbo.Initialize(width, height);
	// 5- attach it to currently bound framebuffer object
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo.GetID()));
	// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	bool complete = IsComplete();
	// 7- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::Initialize(int width, int height, int internalFormat, int nr_of_colorBuffers)
{
	// 1- framebuffer configuration 
	Bind();
	for (unsigned int i = 0; i < nr_of_colorBuffers; i++)
	{
		// 2- create a color attachment texture
		//std::shared_ptr<Texture> _texture_temp = std::make_shared<Texture>();
		//_texture_temp->Initialize(width, height, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		//m_textures.push_back(_texture_temp);
		while (m_textures.size() < nr_of_colorBuffers)
		{
			std::shared_ptr<Texture> _texture_temp = std::make_shared<Texture>();
			m_textures.push_back(_texture_temp);
		}
		m_textures[i]->Initialize(width, height, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		// 3- attach it to currently bound framebuffer object
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_textures[i]->GetID(), 0));
	}
	// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
	m_rbo.Initialize(width, height);
	// 5- attach it to currently bound framebuffer object
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo.GetID()));
	// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	
	//unsigned int attachments[nr] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, ..... };
	unsigned int* attachments = (unsigned int*)alloca(nr_of_colorBuffers * sizeof(unsigned int));
	for (unsigned int i = 0; i < nr_of_colorBuffers; i++)
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(nr_of_colorBuffers, attachments);
	
	bool complete = IsComplete();
	// 7- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::InitializeDepthMap(int width, int height, bool low_quality)
{
	// 1- framebuffer configuration 
	Bind();
	// 2- create a Depth map attachment texture
	if (low_quality)
		m_texture.Initialize(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);
	else
		m_texture.Initialize(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_LINEAR, GL_LINEAR,
			GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, glm::vec4(1.0f));
	// 3- attach it to currently bound framebuffer object
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture.GetID(), 0));
	// 4- A framebuffer object however is not complete without a color buffer 
	// so we need to explicitly tell OpenGL we're not going to render any color data
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	// 5- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	bool complete = IsComplete();
	// 6- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::InitializeDepthCubeMap(int width, int height, bool low_quality)
{
	// 1- framebuffer configuration
	Bind();
	// 2- create a Depth CUBO map attachment texture
	if (low_quality)
		m_texture.InitializeCube(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
			GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	else
		m_texture.InitializeCube(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT,
			GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	// 3- attach it to currently bound framebuffer object
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture.GetID(), 0));
	// 4- A framebuffer object however is not complete without a color buffer
	// so we need to explicitly tell OpenGL we're not going to render any color data
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));
	// 5- Now that we actually created the framebuffer and added all attachmentes we want to check if it is actually complete now
	bool complete = IsComplete();
	// 6- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::InitializeGBuffer(int width, int height)
{
	int nr_of_colorBuffers = 3;

	// 1- framebuffer configuration 
	Bind();

	// The first Initialized fill the Textures' vector
	while (m_textures.size() < nr_of_colorBuffers)
	{
		std::shared_ptr<Texture> _texture_temp = std::make_shared<Texture>();
		m_textures.push_back(_texture_temp);
	}

	// 2- Create a color attachment textures 
	// &
	// 3- attach it to currently bound framebuffer object

	// - position color buffer [gPosition]
	m_textures[0]->Initialize(width, height, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D, m_textures[0]->GetID(), 0));

	// - normal color buffer [gNormal]
	m_textures[1]->Initialize(width, height, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, m_textures[1]->GetID(), 0));

	// - color + specular color buffer [gAlbedoSpec]
	m_textures[2]->Initialize(width, height, GL_RGBA, GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, m_textures[2]->GetID(), 0));


	// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
	m_rbo.Initialize(width, height);
	// 5- attach it to currently bound framebuffer object
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo.GetID()));
	// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

	//unsigned int attachments[nr] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, ..... };
	unsigned int* attachments = (unsigned int*)alloca(nr_of_colorBuffers * sizeof(unsigned int));
	for (unsigned int i = 0; i < nr_of_colorBuffers; i++)
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(nr_of_colorBuffers, attachments);

	bool complete = IsComplete();
	// 7- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::InitializeGBufferSSAO(int width, int height)
{

	int nr_of_colorBuffers = 3;

	// 1- framebuffer configuration 
	Bind();

	// The first Initialized fill the Textures' vector
	while (m_textures.size() < nr_of_colorBuffers)
	{
		std::shared_ptr<Texture> _texture_temp = std::make_shared<Texture>();
		m_textures.push_back(_texture_temp);
	}

	// 2- Create a color attachment textures 
	// &
	// 3- attach it to currently bound framebuffer object

	// - position color buffer [gPosition]
	m_textures[0]->Initialize(width, height, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 0, GL_TEXTURE_2D, m_textures[0]->GetID(), 0));

	// - normal color buffer [gNormal]
	m_textures[1]->Initialize(width, height, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, m_textures[1]->GetID(), 0));

	// - color + specular color buffer [gAlbedoSpec]
	m_textures[2]->Initialize(width, height, GL_RGBA, GL_RGBA, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, m_textures[2]->GetID(), 0));


	// 4- create a render buffer object for depth and stencil attachment ( we won't be sampling these)
	m_rbo.Initialize(width, height);
	// 5- attach it to currently bound framebuffer object
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo.GetID()));
	// 6- Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

	//unsigned int attachments[nr] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, ..... };
	unsigned int* attachments = (unsigned int*)alloca(nr_of_colorBuffers * sizeof(unsigned int));
	for (unsigned int i = 0; i < nr_of_colorBuffers; i++)
		attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(nr_of_colorBuffers, attachments);

	bool complete = IsComplete();
	// 7- Unbind FrameBuffer and reset to default
	Unbind();

	return complete;
}

bool FrameBuffer::IsComplete() const
{
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
