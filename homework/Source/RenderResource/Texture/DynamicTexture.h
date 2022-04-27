/*************************************************

Author:乔南

Date:2022-4.01

Description:动态纹理类,负责开辟GPU显存

**************************************************/
#pragma once
#include "Texture.h"

//纹理数据格式
enum class TextureFormat
{
	COLOR,
	COLORHDR,
	DEPTH,
	DEPTHMIP
};

//动态纹理基类
class DynamicTexture :public Texture
{
public:
	unsigned int width;
	unsigned int height;
	TextureFormat format;
	DynamicTexture(unsigned int w, unsigned int h, TextureFormat f) :width(w), height(h), format(f) {}
};
typedef CountRef<DynamicTexture> DynamicTextureRef;

//二维动态纹理
class DynamicTexture2D :public DynamicTexture
{
public:
	DynamicTexture2D(unsigned int w, unsigned int h, TextureFormat f) :DynamicTexture(w, h, f) {}
	virtual void InitRHI() override
	{
		const uint64_t tsFlags = 0
			| BGFX_SAMPLER_MIN_POINT
			| BGFX_SAMPLER_MAG_POINT
			| BGFX_SAMPLER_MIP_POINT
			| BGFX_SAMPLER_U_CLAMP
			| BGFX_SAMPLER_V_CLAMP
			;
		switch (format)
		{
		case TextureFormat::COLOR:
			m_texh = bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_RT | tsFlags);
			break;
		case TextureFormat::COLORHDR:
			m_texh = bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_RT | tsFlags);
			break;
		case TextureFormat::DEPTH:
			m_texh = bgfx::createTexture2D(uint16_t(width), uint16_t(height), false, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT | tsFlags);
			break;
		case TextureFormat::DEPTHMIP:
			m_texh = bgfx::createTexture2D(uint16_t(width), uint16_t(height), true, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT | tsFlags | BGFX_SAMPLER_W_CLAMP);
			break;
		default:
			break;
		}
	}
};
typedef CountRef<DynamicTexture2D> DynamicTexture2DRef;




//二维动态纹理数组
class DynamicTexture2DArray
{
public:
	std::vector<DynamicTexture2DRef> textures;
	DynamicTexture2DArray(unsigned int w, unsigned int h, unsigned int n, TextureFormat f = TextureFormat::COLOR)
	{
		for (int i = 0; i < n; i++)textures.push_back(DynamicTexture2DRef(new DynamicTexture2D(w, h, f)));
	}
	void InitRHI()
	{
		for (int i = 0; i < textures.size(); i++)textures[i]->InitRHI();
	}
	void ReleaseRHI()
	{
		for (int i = 0; i < textures.size(); i++)textures[i]->ReleaseRHI();
	}
};
typedef CountRef<DynamicTexture2DArray> DynamicTexture2DArrayRef;

//立方体动态纹理
class DynamicTextureCubeMap : public DynamicTexture
{
public:
	DynamicTextureCubeMap(unsigned int size, TextureFormat f) :DynamicTexture(size, size, f) {}

	virtual void InitRHI() override
	{
		const uint64_t tsFlags = 0
			| BGFX_SAMPLER_MIN_POINT
			| BGFX_SAMPLER_MAG_POINT
			| BGFX_SAMPLER_MIP_POINT
			| BGFX_SAMPLER_U_CLAMP
			| BGFX_SAMPLER_V_CLAMP
			| BGFX_SAMPLER_W_CLAMP
			;
		switch (format)
		{
		case TextureFormat::COLOR:
			m_texh = bgfx::createTextureCube(uint16_t(width), false, 1, bgfx::TextureFormat::RGBA16, BGFX_TEXTURE_RT | tsFlags);
			break;
		case TextureFormat::DEPTH:
			m_texh = bgfx::createTextureCube(uint16_t(width), false, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT | tsFlags);
			break;
		default:
			break;
		}
	}
};