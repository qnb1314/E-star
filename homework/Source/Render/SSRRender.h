/*************************************************

Author:乔南

Date:2022-4.18

Description:SSR渲染器

**************************************************/
#pragma once
#include "RenderBase.h"

class SSRRender :public RenderBase
{
public:
	ShaderRef SSRShaderref;
	DynamicTexture2DRef ColorTextureref;

	bgfx::FrameBufferHandle FBO;
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;

	unsigned int size;
	unsigned int size_level;

	bgfx::UniformHandle s_texGBuffer[GBUFFERSIZE];
	bgfx::UniformHandle s_texDirLight;
	bgfx::UniformHandle s_texDepth;

	bgfx::UniformHandle u_viewpoint;
	bgfx::UniformHandle u_indirlightinfo;

	SSRRender(bgfx::ViewId id, unsigned int sl)
		:RenderBase(id), size(pow(2, sl)), size_level(sl)
		, SSRShaderref(new SSRShader)
		, ColorTextureref(new DynamicTexture2D(pow(2, sl), pow(2, sl), TextureFormat::COLORHDR)) {}
	~SSRRender() {}

	virtual void InitRHI() override
	{
		//创建shader
		SSRShaderref->InitRHI();

		//创建颜色buffer
		ColorTextureref->InitRHI();

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
		gbufferAt[0].init(ColorTextureref->m_texh);
		FBO = bgfx::createFrameBuffer(1, gbufferAt, false);

		//创建统一值
		s_texGBuffer[0] = bgfx::createUniform("s_texGBuffer0", bgfx::UniformType::Sampler);
		s_texGBuffer[1] = bgfx::createUniform("s_texGBuffer1", bgfx::UniformType::Sampler);
		s_texGBuffer[2] = bgfx::createUniform("s_texGBuffer2", bgfx::UniformType::Sampler);
		s_texGBuffer[3] = bgfx::createUniform("s_texGBuffer3", bgfx::UniformType::Sampler);
		s_texGBuffer[4] = bgfx::createUniform("s_texGBuffer4", bgfx::UniformType::Sampler);
		s_texDirLight = bgfx::createUniform("s_texDirLight", bgfx::UniformType::Sampler);
		s_texDepth = bgfx::createUniform("s_texDepth", bgfx::UniformType::Sampler);

		u_viewpoint = bgfx::createUniform("viewpoint", bgfx::UniformType::Vec4);
		u_indirlightinfo = bgfx::createUniform("indirlightinfo", bgfx::UniformType::Vec4);
	}

	virtual void ReleaseRHI() override
	{
		SSRShaderref->ReleaseRHI();

		ColorTextureref->ReleaseRHI();
		bgfx::destroy(FBO);
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);

		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			bgfx::destroy(s_texGBuffer[i]);
		}
		bgfx::destroy(s_texDirLight);
		bgfx::destroy(s_texDepth);
		bgfx::destroy(u_viewpoint);
		bgfx::destroy(u_indirlightinfo);
	}


public:
	void BeforeRender(const View& view, DynamicTexture2DArrayRef GBufferref, DynamicTexture2DRef DirLightref, DynamicTexture2DRef Depthbufferref, int HiZLevel)
	{
		bgfx::setViewFrameBuffer(m_viewid, FBO);

		bgfx::setViewClear(m_viewid
			, BGFX_CLEAR_COLOR
			, 0x000000ff
		);
		bgfx::setViewRect(m_viewid, 0, 0, uint16_t(size), uint16_t(size));
		bgfx::touch(m_viewid);

		//视点变换矩阵
		float viewmat[16];
		float projmat[16];
		GetMat4WithMatrix4f(view.GetViewMatrix(), viewmat);
		GetMat4WithMatrix4f(view.GetProjectionMatrix(), projmat);
		bgfx::setViewTransform(m_viewid, viewmat, projmat);

		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			bgfx::setTexture(i, s_texGBuffer[i], GBufferref->textures[i]->m_texh);
		}
		bgfx::setTexture(GBUFFERSIZE, s_texDirLight, DirLightref->m_texh);
		bgfx::setTexture(GBUFFERSIZE + 1, s_texDepth, Depthbufferref->m_texh);


		float viewpoint[4] = { view.ViewPoint.x,view.ViewPoint.y,view.ViewPoint.z,0.0f };
		bgfx::setUniform(u_viewpoint, viewpoint);

		float indirlightinfo[4] = { 1.0f / size,HiZLevel,1.0f,50.0f };
		bgfx::setUniform(u_indirlightinfo, indirlightinfo);

	}

	void Render()
	{
		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);

		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
		);

		bgfx::submit(m_viewid, SSRShaderref->m_program);
	}
};
typedef CountRef<SSRRender> SSRRenderRef;
