/*************************************************

Author:乔南

Date:2022-4.01

Description:子渲染器基类

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