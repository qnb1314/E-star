/*************************************************

Author:乔南

Date:2022-4.01

Description:纹理基类

**************************************************/
#pragma once
#include "../RenderResource.h"

class Texture:public RenderResource
{
public:
	bgfx::TextureHandle m_texh;
	virtual void ReleaseRHI() override { bgfx::destroy(m_texh); }
};