/*************************************************

Author:乔南

Date:2022-4.01

Description:网格体的GPU顶点流类

**************************************************/
#pragma once
#include "bgfx_utils.h"
#include "../../Math/Graph.h"

#undef MAX_POINT_NUM
#define MAX_POINT_NUM 100000

struct PosVertex
{
	float x;
	float y;
	float z;
};

struct PosNormalTexcoordTangentVertex
{
	float x, y, z;
	float nx, ny, nz;
	float vtx, vty;
	float tx, ty, tz;
	float bx, by, bz;
};

class StaticMeshRenderData
{
public:

	bgfx::VertexBufferHandle m_vbh[2];//顶点流handle-顶点&顶点+法线+纹理坐标+切线T
	PosVertex* p_buffer;
	PosNormalTexcoordTangentVertex* pntt_buffer;

	bgfx::IndexBufferHandle m_ibh[2];//索引流handle-线&面
	uint16_t* Indices[2];

	StaticMeshRenderData() {}
	~StaticMeshRenderData() {}

	void InitRHI(Graph* graph)
	{
		InitRHI_VertexBuffer_Pos(graph);
		InitRHI_VertexBuffer_PosNormalTexcoordTangent(graph);
		InitRHI_IndexBuffer_Line(graph);
		InitRHI_IndexBuffer_Face(graph);
	}
	void ReleaseRHI()
	{
		bgfx::destroy(m_vbh[0]);
		bgfx::destroy(m_vbh[1]);
		bgfx::destroy(m_ibh[0]);
		bgfx::destroy(m_ibh[1]);

		delete[] p_buffer;
		delete[] pntt_buffer;
		delete[] Indices[0];
		delete[] Indices[1];
	}



private:
	void InitRHI_VertexBuffer_Pos(Graph* graph)
	{
		bgfx::VertexLayout layout;
		layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.end();
		p_buffer = new PosVertex[MAX_POINT_NUM];
		int num = 0;
		
		for (int i = 0; i < graph->Triangles.size(); i++)
		{
			Triangle temp = graph->Triangles[i];
			p_buffer[num++] = { temp.A.x, temp.A.y, temp.A.z };
			p_buffer[num++] = { temp.B.x, temp.B.y, temp.B.z };
			p_buffer[num++] = { temp.C.x, temp.C.y, temp.C.z };
		}
		m_vbh[0] = bgfx::createVertexBuffer(bgfx::makeRef(p_buffer, num * sizeof(PosVertex)), layout);
	}

	void InitRHI_VertexBuffer_PosNormalTexcoordTangent(Graph* graph)
	{
		
		bgfx::VertexLayout layout;
		layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
			.end();
		pntt_buffer = new PosNormalTexcoordTangentVertex[MAX_POINT_NUM];
		int num = 0;

		for (int i = 0; i < graph->Triangles.size(); i++)
		{
			Triangle temp = graph->Triangles[i];
			Vector3f t_vec, b_vec;
			temp.GetTB(t_vec, b_vec);
			pntt_buffer[num++] = { temp.A.x,temp.A.y,temp.A.z,temp.An.x,temp.An.y,temp.An.z,temp.At.x,temp.At.y,t_vec.x,t_vec.y,t_vec.z,b_vec.x,b_vec.y,b_vec.z };
			pntt_buffer[num++] = { temp.B.x,temp.B.y,temp.B.z,temp.Bn.x,temp.Bn.y,temp.Bn.z,temp.Bt.x,temp.Bt.y,t_vec.x,t_vec.y,t_vec.z,b_vec.x,b_vec.y,b_vec.z };
			pntt_buffer[num++] = { temp.C.x,temp.C.y,temp.C.z,temp.Cn.x,temp.Cn.y,temp.Cn.z,temp.Ct.x,temp.Ct.y,t_vec.x,t_vec.y,t_vec.z,b_vec.x,b_vec.y,b_vec.z };
		}

		m_vbh[1] = bgfx::createVertexBuffer(bgfx::makeRef(pntt_buffer, num * sizeof(PosNormalTexcoordTangentVertex)), layout);
	}

	void InitRHI_IndexBuffer_Line(Graph* graph)
	{
		Indices[0] = new uint16_t[2 * MAX_POINT_NUM];
		int num = 0;
		for (int i = 0; i < graph->Triangles.size(); i++)
		{
			uint16_t index_a = i * 3;
			uint16_t index_b = i * 3 + 1;
			uint16_t index_c = i * 3 + 2;
			Indices[0][num++] = index_a;
			Indices[0][num++] = index_b;
			Indices[0][num++] = index_b;
			Indices[0][num++] = index_c;
			Indices[0][num++] = index_c;
			Indices[0][num++] = index_a;
		}
		m_ibh[0] = bgfx::createIndexBuffer(bgfx::makeRef(Indices[0], num * sizeof(uint16_t)));
	}

	void InitRHI_IndexBuffer_Face(Graph* graph)
	{
		Indices[1] = new uint16_t[MAX_POINT_NUM];
		int num = 0;
		for (int i = 0; i < graph->Triangles.size(); i++)
		{
			uint16_t index_a = i * 3;
			uint16_t index_b = i * 3 + 1;
			uint16_t index_c = i * 3 + 2;

			Indices[1][num++] = index_a;
			Indices[1][num++] = index_b;
			Indices[1][num++] = index_c;
		}
		m_ibh[1] = bgfx::createIndexBuffer(bgfx::makeRef(Indices[1], num * sizeof(uint16_t)));
	}
};

