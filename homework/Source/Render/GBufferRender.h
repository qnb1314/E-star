/*************************************************

Author:乔南

Date:2022-4.01

Description:GbufferPass渲染器，将shadingpoint的所有需要的属性存储在一系列纹理附件中

**************************************************/
#pragma once
#include "RenderBase.h"

class GBufferRender :public RenderBase
{
public:
	ShaderRef GBufferShaderref;
	ShaderRef SkyBoxShaderref;

	DynamicTexture2DArrayRef GBufferref;
	DynamicTexture2DRef DepthBufferref;
	bgfx::FrameBufferHandle FBO;

	unsigned int size;
	
	//统一值Handle
	bgfx::UniformHandle s_texColor;
	bgfx::UniformHandle s_texNormal;
	bgfx::UniformHandle s_texAorm;

	bgfx::UniformHandle s_texCube;

	GBufferRender(bgfx::ViewId id, unsigned int s)
		:RenderBase(id)
		, size(s)
		, GBufferShaderref(new GbufferShader)
		, SkyBoxShaderref(new SkyBoxShader)
		, GBufferref(new DynamicTexture2DArray(s, s, GBUFFERSIZE, TextureFormat::COLOR))
		, DepthBufferref(new DynamicTexture2D(s, s, TextureFormat::DEPTH))
	{
	}
	~GBufferRender() {}

	virtual void InitRHI() override
	{
		GBufferShaderref->InitRHI();
		SkyBoxShaderref->InitRHI();

		GBufferref->InitRHI();
		DepthBufferref->InitRHI();
		
		bgfx::Attachment gbufferAt[GBUFFERSIZE + 1];
		for (int i = 0; i < GBUFFERSIZE; i++)
		{
			gbufferAt[i].init(GBufferref->textures[i]->m_texh);
		}
		gbufferAt[GBUFFERSIZE].init(DepthBufferref->m_texh);
		FBO = bgfx::createFrameBuffer(GBUFFERSIZE + 1, gbufferAt, false);

		s_texColor = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
		s_texNormal = bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);
		s_texAorm = bgfx::createUniform("s_texAorm", bgfx::UniformType::Sampler);
		s_texCube = bgfx::createUniform("s_texCube", bgfx::UniformType::Sampler);
	}
	virtual void ReleaseRHI() override
	{
		GBufferShaderref->ReleaseRHI();
		SkyBoxShaderref->ReleaseRHI();

		GBufferref->ReleaseRHI();
		DepthBufferref->ReleaseRHI();

		bgfx::destroy(FBO);

		bgfx::destroy(s_texColor);
		bgfx::destroy(s_texNormal);
		bgfx::destroy(s_texAorm);
		bgfx::destroy(s_texCube);
	}


public:
	void BeforeRender(const View& view)
	{
		bgfx::setViewFrameBuffer(m_viewid, FBO);


		bgfx::setViewClear(m_viewid
			, BGFX_CLEAR_COLOR
			, 0x00000000
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

	void Render(const std::vector<ObjectRef>& render_objects, SkyBoxRef render_sky, const View& view)
	{
		//天空盒
		bgfx::setTexture(0, s_texCube, render_sky->CubeMapLOD->m_texh);

		Eigen::Matrix4f Model;
		Model <<
			1.0f, 0.0f, 0.0f, view.ViewPoint.x,
			0.0f, 1.0f, 0.0f, view.ViewPoint.y,
			0.0f, 0.0f, 1.0f, view.ViewPoint.z,
			0.0f, 0.0f, 0.0f, 1.0f;
		float mtx[16];
		GetMat4WithMatrix4f(Model, mtx);
		bgfx::setTransform(mtx, 1);
		bgfx::setVertexBuffer(0, render_sky->m_vbh);
		bgfx::setIndexBuffer(render_sky->m_ibh);

		bgfx::setState(0
			| BGFX_STATE_WRITE_RGB
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_DEPTH_TEST_LEQUAL
			| BGFX_STATE_CULL_CW
		);
		bgfx::submit(m_viewid, SkyBoxShaderref->m_program);

		//遍历所有的object并提交
		for (int i = 0; i < render_objects.size(); i++)
		{
			ObjectRef curr = render_objects[i];

			bgfx::setTexture(0, s_texColor, curr->materialref->texture_color->m_texh);
			bgfx::setTexture(1, s_texNormal, curr->materialref->texture_normal->m_texh);
			bgfx::setTexture(2, s_texAorm, curr->materialref->texture_aorm->m_texh);

			float mtx[32];
			GetMat4WithMatrix4f(curr->Model, mtx);
			GetMat4WithMatrix4f(curr->Model.inverse().eval().transpose(), mtx + 16);
			bgfx::setTransform(mtx, 2);

			bgfx::setVertexBuffer(0, curr->staticmeshref->renderdata.m_vbh[1]);
			bgfx::setIndexBuffer(curr->staticmeshref->renderdata.m_ibh[1]);

			bgfx::setState(0
				| BGFX_STATE_WRITE_RGB
				| BGFX_STATE_WRITE_A
				| BGFX_STATE_DEPTH_TEST_LEQUAL
				| BGFX_STATE_CULL_CW
			);
			bgfx::submit(m_viewid, GBufferShaderref->m_program);
		}
	}
};
typedef CountRef<GBufferRender> GBufferRenderRef;