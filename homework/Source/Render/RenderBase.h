/*************************************************

Author:����

Date:2022-4.01

Description:����Ⱦ������

**************************************************/
#pragma once
#include "../Scene/Scene.h"
#include "../Scene/View.h"
#include "../RenderResource/Shader/Shaders_VF.h"
#include "../RenderResource/Texture/DynamicTexture.h"

#undef GBUFFERSIZE
#define GBUFFERSIZE 5

class RenderBase
{
public:
	bgfx::ViewId m_viewid;
	RenderBase(bgfx::ViewId id) :m_viewid(id) {}
	virtual void InitRHI() = 0;
	virtual void ReleaseRHI() = 0;
};