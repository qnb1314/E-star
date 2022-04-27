/*************************************************

Author:乔南

Date:2022-4.02

Description:阴影渲染器,为场景的所有光源根据其类型渲染阴影map

**************************************************/
#pragma once
#include "RenderBase.h"

class ShadowRender :public RenderBase
{
public:
	ShaderRef PointShadowShaderref;
	bgfx::FrameBufferHandle FBO[256];
	int fbo_num = 0;

	bgfx::UniformHandle pointlightinfo;

	ShadowRender(bgfx::ViewId id) :RenderBase(id), PointShadowShaderref(new PointShadowShader) {}
	~ShadowRender() {}

	virtual void InitRHI() override
	{
		PointShadowShaderref->InitRHI();

		pointlightinfo = bgfx::createUniform("lightinfo", bgfx::UniformType::Vec4);
	}

	virtual void ReleaseRHI() override
	{
		PointShadowShaderref->ReleaseRHI();

		bgfx::destroy(pointlightinfo);
	}


public:
	void Render(const std::vector<LightRef>& render_lights, const std::vector<ObjectRef>& all_objects)
	{
		for (int i = 0; i < render_lights.size(); i++)
		{
			LightRef currlight = render_lights[i];
			std::vector<ObjectRef> render_objects;
			switch (currlight->lighttype)
			{
			case LightType::POINT:
				PointLightCullObjects(all_objects, render_objects, currlight);
				PointLightRender(currlight, render_objects);
				break;
			default:
				break;
			}
		}
	}
	void AfterRender()
	{
		for (int i = m_viewid; i < m_viewid + fbo_num; i++)
		{
			bgfx::setViewFrameBuffer(i, BGFX_INVALID_HANDLE);
			bgfx::destroy(FBO[i]);
		}
		fbo_num = 0;
	}
private:
	void PointLightRender(LightRef render_light, const std::vector<ObjectRef>& render_objects)
	{
		//得到VP矩阵
		Eigen::Matrix4f LightInfo = render_light->GetInformation();
		Vector3f LightPoint = Vector3f(LightInfo(0, 0), LightInfo(0, 1), LightInfo(0, 2));
		std::vector<View> ViewsOfPointLight;

		ViewsOfPointLight.push_back(View(LightPoint, 180, 180, 1, 1, 0.1f, render_light->far, 90));
		ViewsOfPointLight.push_back(View(LightPoint, 0, 180, 1, 1, 0.1f, render_light->far, 90));
		ViewsOfPointLight.push_back(View(LightPoint, 90, 90, 1, 1, 0.1f, render_light->far, 90));
		ViewsOfPointLight.push_back(View(LightPoint, 90, -90, 1, 1, 0.1f, render_light->far, 90));
		ViewsOfPointLight.push_back(View(LightPoint, 90, 180, 1, 1, 0.1f, render_light->far, 90));
		ViewsOfPointLight.push_back(View(LightPoint, -90, 180, 1, 1, 0.1f, render_light->far, 90));

		//遍历六个方向
		for (int k = 0; k < 6; k++)
		{
			//创建FBO
			int curr_viewid = m_viewid + fbo_num;
			fbo_num++;

			bgfx::Attachment gbufferAt[1];
			gbufferAt[0].init(render_light->shadowmap->m_texh, bgfx::Access::Write, k);
			FBO[curr_viewid] = bgfx::createFrameBuffer(1, gbufferAt, false);
			bgfx::setViewFrameBuffer(curr_viewid, FBO[curr_viewid]);

			bgfx::setViewClear(curr_viewid
				, BGFX_CLEAR_DEPTH
				, 0x000000ff
				, 1.0f
				, 0
			);
			bgfx::setViewRect(curr_viewid, 0, 0, uint16_t(render_light->shadowmap->width), uint16_t(render_light->shadowmap->height));
			bgfx::touch(curr_viewid);
			//视点变换矩阵
			float viewmat[16];
			float projmat[16];
			GetMat4WithMatrix4f(ViewsOfPointLight[k].GetViewMatrix(), viewmat);
			GetMat4WithMatrix4f(ViewsOfPointLight[k].GetProjectionMatrix(), projmat);
			bgfx::setViewTransform(curr_viewid, viewmat, projmat);

			//遍历object
			for (int j = 0; j < render_objects.size(); j++)
			{
				float lightinfo[4] = { LightPoint.x,LightPoint.y,LightPoint.z, render_light->far };
				bgfx::setUniform(pointlightinfo, lightinfo);

				ObjectRef currobj = render_objects[j];
				float mtx[16];
				GetMat4WithMatrix4f(currobj->Model, mtx);
				bgfx::setTransform(mtx);

				bgfx::setVertexBuffer(0, currobj->staticmeshref->renderdata.m_vbh[0]);
				bgfx::setIndexBuffer(currobj->staticmeshref->renderdata.m_ibh[1]);

				bgfx::setState(0
					| BGFX_STATE_WRITE_Z
					| BGFX_STATE_DEPTH_TEST_LEQUAL
					| BGFX_STATE_CULL_CCW
				);
				bgfx::submit(curr_viewid, PointShadowShaderref->m_program);
			}
		}
	}

	void PointLightCullObjects(const std::vector<ObjectRef>& all_objects, std::vector<ObjectRef>& render_objects, LightRef render_light)
	{
		for (int i = 0; i < all_objects.size(); i++)
		{
			ObjectRef curr_object = all_objects[i];
			Vector3f object_center = (curr_object->bound->PMax + curr_object->bound->PMin) * 0.5f;
			Eigen::Matrix4f light_info = render_light->GetInformation();
			Vector3f light_to_object = object_center - Vector3f(light_info(0, 0), light_info(0, 1), light_info(0, 2));
			float max_distance = render_light->far;
			if (DotProduction(light_to_object, light_to_object) < max_distance * max_distance)render_objects.push_back(curr_object);
		}
	}
};
typedef CountRef<ShadowRender> ShadowRenderRef;