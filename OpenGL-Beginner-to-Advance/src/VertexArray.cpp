#include "VertexArray.h"

#include "Utility.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout,
							const std::shared_ptr<IndexBuffer> ib, unsigned int AttrP_offset)
{
	Bind();
	vb.Bind();
	if (ib)
		ib->Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(i + AttrP_offset));
		GLCall(glVertexAttribPointer(i + AttrP_offset, element.count, element.type, element.normalize, layout.GetStride(), (const void*)(offset)));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout,
							unsigned int AttrP_offset, const std::shared_ptr<IndexBuffer> ib)
{
	this->AddBuffer(vb, layout, ib, AttrP_offset);
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
