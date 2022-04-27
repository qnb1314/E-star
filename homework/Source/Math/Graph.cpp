#include "Graph.h"
#include "fstream"

GraphBall::GraphBall(int num_x, int num_y) :Graph()
{
	float delta_theta = 180.0f / (num_y + 1);
	float delta_phy = 360.0f / num_x;
	//顶部
	for (int i = 0; i < num_x; i++)
	{
		Vector3f A, B, C;
		float
			A_theta = .0f, A_phy = i * delta_phy,
			B_theta = delta_theta, B_phy = (i + 1) * delta_phy,
			C_theta = delta_theta, C_phy = i * delta_phy;
		GetVectorWithAngle(A_theta, A_phy, A);
		GetVectorWithAngle(B_theta, B_phy, B);
		GetVectorWithAngle(C_theta, C_phy, C);
		Triangles.push_back(Triangle(A, B, C, A, B, C, Vector3f(1.0f - A_phy / 360.0f, 1.0f - A_theta / 180.0f, .0f), Vector3f(1.0f - B_phy / 360.0f, 1.0f - B_theta / 180.0f, .0f), Vector3f(1.0f - C_phy / 360.0f, 1.0f - C_theta / 180.0f, .0f)));
	}

	//中间部分
	for (int j = 1; j < num_y; j++)
	{
		for (int i = 0; i < num_x; i++)
		{
			Vector3f A, B, C, D;
			float
				A_theta = j * delta_theta, A_phy = i * delta_phy,
				B_theta = j * delta_theta, B_phy = (i + 1) * delta_phy,
				C_theta = (j + 1) * delta_theta, C_phy = (i + 1) * delta_phy,
				D_theta = (j + 1) * delta_theta, D_phy = i * delta_phy;
			GetVectorWithAngle(A_theta, A_phy, A);
			GetVectorWithAngle(B_theta, B_phy, B);
			GetVectorWithAngle(C_theta, C_phy, C);
			GetVectorWithAngle(D_theta, D_phy, D);
			Triangles.push_back(Triangle(A, B, C, A, B, C, Vector3f(1.0f - A_phy / 360.0f, 1.0f - A_theta / 180.0f, .0f), Vector3f(1.0f - B_phy / 360.0f, 1.0f - B_theta / 180.0f, .0f), Vector3f(1.0f - C_phy / 360.0f, 1.0f - C_theta / 180.0f, .0f)));
			Triangles.push_back(Triangle(A, C, D, A, C, D, Vector3f(1.0f - A_phy / 360.0f, 1.0f - A_theta / 180.0f, .0f), Vector3f(1.0f - C_phy / 360.0f, 1.0f - C_theta / 180.0f, .0f), Vector3f(1.0f - D_phy / 360.0f, 1.0f - D_theta / 180.0f, .0f)));
		}
	}

	//底部
	for (int i = 0; i < num_x; i++)
	{
		Vector3f A, B, C;
		float
			A_theta = 180.0f, A_phy = i * delta_phy,
			B_theta = 180.0f - delta_theta, B_phy = i * delta_phy,
			C_theta = 180.0f - delta_theta, C_phy = (i + 1) * delta_phy;
		GetVectorWithAngle(A_theta, A_phy, A);
		GetVectorWithAngle(B_theta, B_phy, B);
		GetVectorWithAngle(C_theta, C_phy, C);
		Triangles.push_back(Triangle(A, B, C, A, B, C, Vector3f(1.0f - A_phy / 360.0f, 1.0f - A_theta / 180.0f, .0f), Vector3f(1.0f - B_phy / 360.0f, 1.0f - B_theta / 180.0f, .0f), Vector3f(1.0f - C_phy / 360.0f, 1.0f - C_theta / 180.0f, .0f)));
	}
}



GraphRectangle::GraphRectangle(Vector3f point, Vector3f vectorx, Vector3f vectory)
{
	Vector3f normal = CrossProduction(vectorx, vectory).normalized();

	Vector3f A = point;
	Vector3f B = point + vectorx;
	Vector3f C = point + vectorx + vectory;
	Vector3f D = point + vectory;

	Vector3f At(0.0f, 0.0f, 0.0f), Bt(1.0f, 0.0f, 0.0f), Ct(1.0f, 1.0f, 0.0f), Dt(0.0f, 1.0f, 0.0f);

	Triangles.push_back(Triangle(A, B, C, normal, normal, normal, At, Bt, Ct));
	Triangles.push_back(Triangle(A, C, D, normal, normal, normal, At, Ct, Dt));
}

GraphCuboid::GraphCuboid(float lx, float ly, float lz)
{
	Vector3f pmin(-lx * 0.5f, -ly * 0.5f, -lz * 0.5f), pmax(lx * 0.5f, ly * 0.5f, lz * 0.5f);
	Vector3f
		t1(pmin.x, pmax.y, pmax.z),
		t2(pmax),
		t3(pmax.x, pmax.y, pmin.z),
		t4(pmin.x, pmax.y, pmin.z),
		d1(pmin.x, pmin.y, pmax.z),
		d2(pmax.x, pmin.y, pmax.z),
		d3(pmax.x, pmin.y, pmin.z),
		d4(pmin);

	GraphRectangle
		front(d1, d2 - d1, t1 - d1),
		back(d3, d4 - d3, t3 - d3),
		right(d2, d3 - d2, t2 - d2),
		left(d4, d1 - d4, t4 - d4),
		top(t1, t2 - t1, t4 - t1),
		down(d4, d3 - d4, d1 - d4);

	while (!front.Triangles.empty())
	{
		Triangles.push_back(front.Triangles.back());
		front.Triangles.pop_back();
	}
	while (!back.Triangles.empty())
	{
		Triangles.push_back(back.Triangles.back());
		back.Triangles.pop_back();
	}
	while (!right.Triangles.empty())
	{
		Triangles.push_back(right.Triangles.back());
		right.Triangles.pop_back();
	}
	while (!left.Triangles.empty())
	{
		Triangles.push_back(left.Triangles.back());
		left.Triangles.pop_back();
	}
	while (!top.Triangles.empty())
	{
		Triangles.push_back(top.Triangles.back());
		top.Triangles.pop_back();
	}
	while (!down.Triangles.empty())
	{
		Triangles.push_back(down.Triangles.back());
		down.Triangles.pop_back();
	}
}

GraphObj::GraphObj(std::string objpath)
{
	std::ifstream in(objpath.c_str());

	if (!in.good())
	{
		std::cout << "ERROR:" << objpath << " is error" << std::endl;
		exit(0);
	}

	char buffer[256];

	std::vector <Vector3f> vertices;
	std::vector <Vector3f> normals;
	std::vector <Vector3f> texcoords;

	int num = 0;

	while (!in.getline(buffer, 255).eof())
	{
		buffer[255] = '\0';
		//读取obj内容
		if (buffer[0] == 'v' && buffer[1] == ' ')
		{
			//顶点信息
			float vx, vy, vz;
			if (sscanf_s(buffer, "v %f %f %f", &vx, &vy, &vz) == 3)
			{
				vertices.push_back(Vector3f(vx, vy, vz));
			}
			else
			{
				std::cout << "ERROR: vertex is error" << std::endl;
				exit(0);
			}
		}
		else if (buffer[0] == 'v' && buffer[1] == 'n')
		{
			//法线信息
			float nx, ny, nz;
			if (sscanf_s(buffer, "vn %f %f %f", &nx, &ny, &nz) == 3)
			{
				normals.push_back(Vector3f(nx, ny, nz));
			}
			else
			{
				std::cout << "ERROR: normal is error" << std::endl;
				exit(0);
			}
		}
		else if (buffer[0] == 'v' && buffer[1] == 't')
		{
			//纹理信息
			float vtx, vty;
			if (sscanf_s(buffer, "vt %f %f", &vtx, &vty) == 2)
			{
				texcoords.push_back(Vector3f(vtx, 1.0f - vty, 0.0f));
			}
			else
			{
				std::cout << "ERROR: texture is error" << std::endl;
				exit(0);
			}
		}
		else if (buffer[0] == 'f' && buffer[1] == ' ')
		{
			//面元索引信息
			unsigned int v[4], n[4], vt[4];
			//四边形的情况
			if (sscanf_s(buffer, "f %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u", v, vt, n, v + 1, vt + 1, n + 1, v + 2, vt + 2, n + 2, v + 3, vt + 3, n + 3) == 12)
			{
				for (int i = 0; i <= 3; i++)
				{
					v[i]--;
					n[i]--;
					vt[i]--;
				}
				Triangles.push_back(Triangle(vertices[v[0]], vertices[v[1]], vertices[v[2]], normals[n[0]], normals[n[1]], normals[n[2]], texcoords[vt[0]], texcoords[vt[1]], texcoords[vt[2]]));
				Triangles.push_back(Triangle(vertices[v[0]], vertices[v[2]], vertices[v[3]], normals[n[0]], normals[n[2]], normals[n[3]], texcoords[vt[0]], texcoords[vt[2]], texcoords[vt[3]]));
			}
			else if (sscanf_s(buffer, "f %u/%u/%u %u/%u/%u %u/%u/%u", v, vt, n, v + 1, vt + 1, n + 1, v + 2, vt + 2, n + 2) == 9)
			{
				for (int i = 0; i <= 2; i++)
				{
					v[i]--;
					n[i]--;
					vt[i]--;
				}
				Triangles.push_back(Triangle(vertices[v[0]], vertices[v[1]], vertices[v[2]], normals[n[0]], normals[n[1]], normals[n[2]], texcoords[vt[0]], texcoords[vt[1]], texcoords[vt[2]]));
			}
		}
	}
}
