#pragma once

class VertexBuffer
{
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void FillBuffer(const void* data, unsigned int size);

	void Bind() const;
	void Unbind() const;
};