/*************************************************

Author:««ƒœ

Date:2022-4.01

Description:æ≤Ã¨Œ∆¿Ì¿‡

**************************************************/
#pragma once
#include "Texture.h"

class StaticTexture :public Texture
{
public:
	std::string texturePath;
	StaticTexture(std::string path) :texturePath(path) {}
	~StaticTexture() {}
};


class StaticTexture2D :public StaticTexture
{
public:
	StaticTexture2D(std::string path) :StaticTexture(path) {}

	virtual void InitRHI() override
	{
		m_texh = loadTexture(texturePath.c_str());
	}
};
typedef CountRef<StaticTexture2D> StaticTexture2DRef;


class StaticTextureCubeMap :public StaticTexture
{
public:
	StaticTextureCubeMap(std::string path) :StaticTexture(path) {}

	virtual void InitRHI() override
	{
		m_texh = loadTexture(texturePath.c_str(), BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_W_CLAMP);
	}
};
typedef CountRef<StaticTextureCubeMap> StaticTextureCubeMapRef;