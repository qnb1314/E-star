/*************************************************

Author:乔南

Date:2022-4.01

Description:光源类,目前只完成了点光源

**************************************************/
#pragma once
#include "../Math/Vector.h"
#include "../RenderResource/Texture/DynamicTexture.h"

#undef SHADOW_MAP_SIZE
#define SHADOW_MAP_SIZE 1024

enum class LightType
{
	POINT,
	PARALLEL,
	SPOT
};

class Light
{
public:
	DynamicTextureRef shadowmap;
	DynamicTexture2DArrayRef shadowmap_cw;
	DynamicTexture2DArrayRef shadowmap_ccw;
	LightType lighttype;
	float far;

	Light(DynamicTextureRef texture, DynamicTexture2DArrayRef texture_cw, DynamicTexture2DArrayRef texture_ccw, LightType type, float f)
		:shadowmap(texture), shadowmap_cw(texture_cw), shadowmap_ccw(texture_ccw), lighttype(type), far(f) {}
	~Light() {}
	virtual void InitRHI()
	{ 
		shadowmap->InitRHI(); 
		shadowmap_cw->InitRHI();
		shadowmap_ccw->InitRHI();
	}
	virtual void ReleaseRHI()
	{ 
		shadowmap->ReleaseRHI(); 
		shadowmap_cw->ReleaseRHI();
		shadowmap_ccw->ReleaseRHI();
	}

	virtual Eigen::Matrix4f GetInformation() = 0;
};
typedef CountRef<Light> LightRef;

class PointLight :public Light
{
public:
	Vector3f Point;
	Vector3f Power;//定义为Li*Area
	float Area;
	
	PointLight(Vector3f point, Vector3f power, float area, float far)
		:Light(DynamicTextureRef(new DynamicTextureCubeMap(SHADOW_MAP_SIZE, TextureFormat::DEPTH))
			, DynamicTexture2DArrayRef(new DynamicTexture2DArray(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 6, TextureFormat::DEPTH))
			, DynamicTexture2DArrayRef(new DynamicTexture2DArray(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 6, TextureFormat::DEPTH))
			, LightType::POINT, far)
		, Point(point), Power(power), Area(area) {}

	virtual Eigen::Matrix4f GetInformation() override
	{
		Eigen::Matrix4f res;
		res <<
			Point.x, Point.y, Point.z, 0.0f,
			Power.x, Power.y, Power.z, 0.0f,
			Area, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f;
		return res;
	}
};