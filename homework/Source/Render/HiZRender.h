/*************************************************

Author:乔南

Date:2022-4.17

Description:HiZ渲染器 

**************************************************/
#pragma once
#include "RenderBase.h"

class HiZRender :public RenderBase
{
public:
	ShaderRef HiZShaderref;
	DynamicTexture2DRef HiZBuffer;

	bgfx::FrameBufferHandle FBO[15];
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;
	bgfx::UniformHandle s_texDepth;
	bgfx::UniformHandle u_mapinfo;

	unsigned int size;
	unsigned int size_level;

	HiZRender(bgfx::ViewId id, unsigned int sl)
		:RenderBase(id)
		, size(pow(2, sl)), size_level(sl)
		, HiZShaderref(new HiZShader)
		, HiZBuffer(new DynamicTexture2D(pow(2, sl), pow(2, sl), TextureFormat::DEPTHMIP))
	{}
	~HiZRender() {}

	virtual void InitRHI() override
	{
		//初始化着色器
		HiZShaderref->InitRHI();
		HiZBuffer->InitRHI();

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

		//创建FBO
		bgfx::Attachment gbufferAt[1];
		gbufferAt[0].init(HiZBuffer->m_texh, bgfx::Access::Write, uint16_t(0U), uint16_t(1U), uint16_t(0U), BGFX_RESOLVE_NONE);
		FBO[0] = bgfx::createFrameBuffer(1, gbufferAt, false);
		for (int i = 1; i <= size_level; i++)
		{
			bgfx::Attachment gbufferAt_curr[1];
			gbufferAt_curr[0].init(HiZBuffer->m_texh, bgfx::Access::Write, uint16_t(0U), uint16_t(1U), uint16_t(i), BGFX_RESOLVE_NONE);
			FBO[i] = bgfx::createFrameBuffer(1, gbufferAt_curr, false);
		}

		//初始化统一值
		s_texDepth = bgfx::createUniform("s_texDepth", bgfx::UniformType::Sampler);
		u_mapinfo = bgfx::createUniform("mapinfo", bgfx::UniformType::Vec4);
	}

	virtual void ReleaseRHI() override
	{
		HiZShaderref->ReleaseRHI();
		HiZBuffer->ReleaseRHI();
		for (int i = 0; i <= size_level; i++)
		{
			bgfx::destroy(FBO[i]);
		}
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);
		bgfx::destroy(s_texDepth);
		bgfx::destroy(u_mapinfo);
	}

public:
	void BeforeRender()
	{
		for (int i = 0; i <= size_level; i++)
		{
			bgfx::setViewClear(m_viewid + i
				, BGFX_CLEAR_DEPTH
				, 0x000000ff
				, 1.0f
			);
			bgfx::setViewRect(m_viewid + i, 0, 0, size / pow(2, i), size / pow(2, i));
			bgfx::touch(m_viewid + i);

			bgfx::setViewFrameBuffer(m_viewid + i, FBO[i]);
		}
	}

	void Render(DynamicTexture2DRef DepthBuffer, int HiZ_Level)
	{
		bgfx::setTexture(0, s_texDepth, DepthBuffer->m_texh);
		float mapinfo[4] = { 0.0,0.0,0.0f,0.0f };
		bgfx::setUniform(u_mapinfo, mapinfo);
		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);
		bgfx::setState(0
			| BGFX_STATE_WRITE_Z
		);
		bgfx::submit(m_viewid, HiZShaderref->m_program);
		
		
		for (int i = 1; i <= HiZ_Level; i++)
		{
			bgfx::setTexture(0, s_texDepth, HiZBuffer->m_texh);
			float mapinfo[4] = { i,1.0f / (size / pow(2,i - 1)),0.0f,0.0f };
			bgfx::setUniform(u_mapinfo, mapinfo);
			bgfx::setVertexBuffer(0, m_vbh);
			bgfx::setIndexBuffer(m_ibh);
			bgfx::setState(0
				| BGFX_STATE_WRITE_Z
			);
			bgfx::submit(m_viewid + i, HiZShaderref->m_program);
		}
	}
};
typedef CountRef<HiZRender> HiZRenderRef;
