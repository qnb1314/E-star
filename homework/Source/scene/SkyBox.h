/*************************************************

Author:ÇÇÄÏ

Date:2022-4.01

Description:Ìì¿ÕºÐÀà

**************************************************/
#pragma once
#include "../RenderResource/Texture/StaticTexture.h"

class SkyBox
{
public:
	StaticTextureCubeMapRef CubeMapLOD;
    StaticTextureCubeMapRef CubeMapIRR;

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;

    uint16_t* Indices;

    SkyBox(StaticTextureCubeMapRef LOD, StaticTextureCubeMapRef IRR) :CubeMapLOD(LOD), CubeMapIRR(IRR) {}
	~SkyBox() {}

	void InitRHI()
	{
        static const float pos_buffer[]={
           -1.0f,  1.0f, -1.0f,
           -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,

           -1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f, -1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

           -1.0f, -1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f, -1.0f,  1.0f,
           -1.0f, -1.0f,  1.0f,

           -1.0f,  1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,
            1.0f,  1.0f,  1.0f,
            1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f,  1.0f,
           -1.0f,  1.0f, -1.0f,

           -1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
           -1.0f, -1.0f,  1.0f,
            1.0f, -1.0f,  1.0f
        };
        bgfx::VertexLayout layout;
        layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .end();
        m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(pos_buffer, 108 * sizeof(float)), layout);


        Indices = new uint16_t[36];
        for (int i = 0; i < 36; i++)Indices[i] = i;
        m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(Indices, 36 * sizeof(uint16_t)));
	}

	void ReleaseRHI()
	{
        bgfx::destroy(m_vbh);
        bgfx::destroy(m_ibh);
        delete[] Indices;
	}
};
typedef CountRef<SkyBox> SkyBoxRef;