/*************************************************

Author:����

Date:2022-3.29

Description:������,��ͨ����������ͬ����

**************************************************/
#pragma once
#include "../RenderResource.h"
#include "../Texture/StaticTexture.h"

class Material
{
public:
	StaticTexture2DRef texture_color;
	StaticTexture2DRef texture_normal;
	StaticTexture2DRef texture_aorm;

	Material(StaticTexture2DRef tex_color, StaticTexture2DRef tex_normal, StaticTexture2DRef tex_aorm)
		:texture_color(tex_color), texture_normal(tex_normal), texture_aorm(tex_aorm) {}
	~Material() {}
};
typedef CountRef<Material> MaterialRef;