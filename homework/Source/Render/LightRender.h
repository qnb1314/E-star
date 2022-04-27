/*************************************************

Author:乔南

Date:2022-4.02

Description:LightPass渲染器,在屏幕四边形上通过Gbuffer完成光照渲染,分为IBL环境光源和直接光源

**************************************************/
#pragma once
#include "RenderBase.h"

class LightRender :public RenderBase
{
public:
	ShaderRef PointLightShaderref;
	ShaderRef IBLShaderref;
	DynamicTexture2DRef ColorTextureref;
	StaticTexture2DRef brdfLUTref;

	bgfx::FrameBufferHandle FBO;
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;

	unsigned int size;

	bgfx::UniformHandle s_texGBuffer[GBUFFERSIZE];
	bgfx::UniformHandle s_texShadowMap;
	bgfx::UniformHandle u_viewpoint;
	bgfx::UniformHandle u_lightpoint;
	bgfx::UniformHandle u_lightpower;
	bgfx::UniformHandle u_lightinfo;

	bgfx::UniformHandle s_texCube;
	bgfx::UniformHandle s_texCubeIrr;
	bgfx::UniformHandle s_texbrdfLUT;

	LightRender(bgfx::ViewId id, unsigned int s)
		:RenderBase(id),size(s)
		, PointLightShaderref(new PointLightShader)
		,IBLShaderref(new IBLShader)
		, ColorTextureref(new DynamicTexture2D(s, s, TextureFormat::COLORHDR))
		, brdfLUTref(new StaticTexture2D("./textures/ibl_brdf_lut.dds")){}
	~LightRender() {}

	virtual void InitRHI() override
	{
		//创建shader
		PointLightShaderref->InitRHI();
		IBLShaderref->InitRHI();

		//创建brdf贴图
		brdfLUTref->InitRHI();

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
		s_texShadowMap = bgfx::createUniform("s_texShadowMap", bgfx::UniformType::Sampler);

		u_viewpoint = bgfx::createUniform("viewpoint", bgfx::UniformType::Vec4);
		u_lightpoint = bgfx::createUniform("lightpoint", bgfx::UniformType::Vec4);
		u_lightpower = bgfx::createUniform("lightpower", bgfx::UniformType::Vec4);
		u_lightinfo = bgfx::createUniform("lightinfo", bgfx::UniformType::Vec4);

		s_texCube = bgfx::createUniform("s_texCube", bgfx::UniformType::Sampler);
		s_texCubeIrr = bgfx::createUniform("s_texCubeIrr", bgfx::UniformType::Sampler);
		s_texbrdfLUT = bgfx::createUniform("s_texbrdfLUT", bgfx::UniformType::Sampler);
	}

	virtual void ReleaseRHI() override
	{
		PointLightShaderref->ReleaseRHI();
		IBLShaderref->ReleaseRHI();

		brdfLUTref->ReleaseRHI();

		ColorTextureref->ReleaseRHI();
		bgfx::destroy(FBO);
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);

		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			bgfx::destroy(s_texGBuffer[i]);
		}
		bgfx::destroy(s_texShadowMap);
		bgfx::destroy(u_viewpoint);
		bgfx::destroy(u_lightpoint);
		bgfx::destroy(u_lightpower);
		bgfx::destroy(u_lightinfo);

		bgfx::destroy(s_texCube);
		bgfx::destroy(s_texCubeIrr);
		bgfx::destroy(s_texbrdfLUT);
	}


public:
	void BeforeRender(const View& view)
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
	}

	void Render(const std::vector<LightRef>& render_lights, SkyBoxRef render_sky, const View& view, DynamicTexture2DArrayRef GBufferref)
	{
		IBLRender(render_sky, view, GBufferref);
		for (int i = 0; i < render_lights.size(); i++)
		{
			LightRef currlight = render_lights[i];
			switch (currlight->lighttype)
			{
			case LightType::POINT:
				PointLightRender(currlight, view, GBufferref);
				break;
			default:
				break;
			}
		}
	}

private:

	void IBLRender(SkyBoxRef render_sky, const View& view, DynamicTexture2DArrayRef GBufferref)
	{
		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			bgfx::setTexture(i, s_texGBuffer[i], GBufferref->textures[i]->m_texh);
		}
		bgfx::setTexture(GBUFFERSIZE, s_texCube, render_sky->CubeMapLOD->m_texh);
		bgfx::setTexture(GBUFFERSIZE + 1, s_texCubeIrr, render_sky->CubeMapIRR->m_texh);
		bgfx::setTexture(GBUFFERSIZE + 2, s_texbrdfLUT, brdfLUTref->m_texh);

		float viewpoint[4] = { view.ViewPoint.x,view.ViewPoint.y,view.ViewPoint.z,0.0f };
		bgfx::setUniform(u_viewpoint, viewpoint);

		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);
		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_BLEND_ADD
		);
		bgfx::submit(m_viewid, IBLShaderref->m_program);
	}

	void PointLightRender(LightRef render_light, const View& view, DynamicTexture2DArrayRef GBufferref)
	{
		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			bgfx::setTexture(i, s_texGBuffer[i], GBufferref->textures[i]->m_texh);
		}
		bgfx::setTexture(GBUFFERSIZE, s_texShadowMap, render_light->shadowmap->m_texh);

		float viewpoint[4] = { view.ViewPoint.x,view.ViewPoint.y,view.ViewPoint.z,0.0f };
		bgfx::setUniform(u_viewpoint, viewpoint);
		Eigen::Matrix4f light_information = render_light->GetInformation();
		float lightpoint[4] = { light_information(0, 0), light_information(0, 1), light_information(0, 2),0.0f };
		bgfx::setUniform(u_lightpoint, lightpoint);
		float lightpower[4] = { light_information(1, 0), light_information(1, 1), light_information(1, 2),0.0f };
		bgfx::setUniform(u_lightpower, lightpower);
		float lightinfo[4] = { light_information(2, 0),render_light->far,1.0f / render_light->shadowmap->width,0.0f };
		bgfx::setUniform(u_lightinfo, lightinfo);

		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);
		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_BLEND_ADD
		);
		bgfx::submit(m_viewid, PointLightShaderref->m_program);
	}
};
typedef CountRef<LightRender> LightRenderRef;