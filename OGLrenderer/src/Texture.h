#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	unsigned char* m_LocalBuffer;
	int m_Height, m_Width, m_BPP;

public:
	Texture(const std::string& filePath);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void Bind(unsigned int slot = 0) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeigth() const { return m_Height; }
};
