#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include "IndexBuffer.h"

#include <memory>

class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout,
				   const std::shared_ptr<IndexBuffer> ib = nullptr, unsigned int AttrP_offset = 0);
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, 
				   unsigned int AttrP_offset, const std::shared_ptr<IndexBuffer> ib = nullptr);

	void Bind() const;
	void Unbind() const;
};