/*************************************************

Author:乔南

Date:2022-4.01

Description:debug渲染器类,用于将Gbuffer渲染在屏幕上

**************************************************/
#pragma once
#include "RenderBase.h"

class DebugRender :public RenderBase
{
public:
	ShaderRef DebugShaderref;

	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
	bgfx::UniformHandle s_texColor;

	DebugRender(bgfx::ViewId id) :RenderBase(id), DebugShaderref(new DebugShader) {}
	~DebugRender() {}

	virtual void InitRHI() override
	{
		//初始化着色器
		DebugShaderref->InitRHI();

		//初始化顶点流
		struct UVVertex
		{
			float u;
			float v;
		};
		static const UVVertex buffer[] =
		{
			{ 0.0f,0.0f },
			{ 1.0f,0.0f },
			{ 1.0f,1.0f },
			{ 0.0f,0.0f },
			{ 1.0f,1.0f },
			{ 0.0f,1.0f }
		};
		bgfx::VertexLayout layout;
		layout
			.begin()
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
		m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(buffer, 6 * sizeof(UVVertex)), layout);

		//初始化索引流
		static const uint16_t Indices[] = { 0,1,2,3,4,5 };
		m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(Indices, 6 * sizeof(uint16_t)));

		//初始化统一值
		s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
	}

	virtual void ReleaseRHI() override
	{
		DebugShaderref->ReleaseRHI();
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);
		bgfx::destroy(s_texColor);
	}

public:
	void BeforeRender(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, DynamicTexture2DRef ColorTextureref)
	{
		bgfx::setViewClear(m_viewid
			, BGFX_CLEAR_COLOR
			, 0x000000ff
		);
		bgfx::setViewRect(m_viewid, _x, _y, _width, _height);
		bgfx::touch(m_viewid);

		bgfx::setTexture(0, s_texColor, ColorTextureref->m_texh);

		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
		);
	}

	void Render()
	{
		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);
		bgfx::submit(m_viewid, DebugShaderref->m_program);
	}
};
typedef CountRef<DebugRender> DebugRenderRef;