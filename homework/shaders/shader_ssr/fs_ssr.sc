$input v_texcoord0
#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texGBuffer0,  0);
SAMPLER2D(s_texGBuffer1,  1);
SAMPLER2D(s_texGBuffer2,  2);
SAMPLER2D(s_texGBuffer3,  3);
SAMPLER2D(s_texGBuffer4,  4);
SAMPLER2D(s_texDirLight,  5);
SAMPLER2D(s_texDepth,  6);

uniform vec4 viewpoint;
uniform vec4 indirlightinfo;//size_inv,max_level,num_sample,range

//Torrance-BRDF
float D(float NoH, float a)
{
	float PI = 3.14159f;
	float a2 = a * a;
	float tt_inv = 1.0 / (NoH * NoH * (a2 - 1.0) + 1.0);
	return a2 * tt_inv * tt_inv / PI;
}
float GeometrySchlickGGX(float costheta, float k){return costheta / (costheta * (1.0 - k) + k);}
float G(float NoV, float NoL, float k)
{
	return GeometrySchlickGGX(NoV, k) * GeometrySchlickGGX(NoL, k);
}
vec3 FresnelSchlick(vec3 wi, vec3 wh, vec3 color, float metalness, float roughness)
{
	vec3 F0 = mix(vec3_splat(0.04), color, metalness);
	return F0 + (max(vec3_splat(1.0-roughness), F0) - F0) * pow(1.0f - dot(wi, wh), 5.0f);
}
float fr_DG(vec3 wi, vec3 wo, vec3 wh, float roughness)
{
	float a = roughness * roughness;
	float k = 0.5 * a * a;
	float NoV = max(wo.z, 0.001);
	float NoL = max(wi.z, 0.001);
	float NoH = max(wh.z, 0.0);
	return D(NoH, a) * G(NoV, NoL, k)  / (4.0 * NoV * NoL);
}


//获取一个TBN坐标系下的采样方向
vec3 GetSample(int i, vec3 wo)
{
	return 2.0 * vec3(0.0,0.0,wo.z) - wo;
}

//获取采样的PDF
float GetPdf(vec3 wo, vec3 wh, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NoV = max(wo.z, 0.001);
	float NoH = max(wh.z, 0.0);
	float G_SmithV = 2.0 * NoV / (NoV + sqrt(NoV * (NoV - NoV * a2) + a2));
	float pdf = G_SmithV * dot(wo, wh)  * D(NoH, a) / NoV;
	return pdf;
}

//视锥体裁剪
vec3 GetViewCutEnd(vec3 start, vec3 end)
{
	vec3 res = end;
	vec3 start_view = mul(u_view, vec4(start, 1.0)).xyz;
	vec3 end_view = mul(u_view, vec4(end, 1.0)).xyz;
	if(end_view.z>=0.0)
	{
		vec3 dir = normalize(end_view - start_view);
		float tt = -start_view.z / dir.z;
		vec3 res_view = start_view + (tt - 0.1) * dir;
		res = mul(u_invView, vec4(res_view, 1.0)).xyz;
	}
	return res;
}
//世界空间映射到屏幕空间[0,1],其中z仍为标准深度格式
vec3 WorldToScene(vec3 world_pos)
{
	vec4 inter_pos = mul(u_viewProj, vec4(world_pos, 1.0));
	vec3 m_pos = inter_pos.xyz / inter_pos.w;
	vec3 scene_pos = vec3((m_pos.xy + vec2_splat(1.0)) * 0.5, m_pos.z);
	return scene_pos;
}
//屏幕空间线段裁剪
vec3 GetCutEnd(vec3 scene_start, vec3 scene_end)
{
	vec3 p1 = scene_start;
	vec3 p2 = scene_end;
	float error = 0.001;
	if(p2.x < 0.0)p2 = p1 + (p1.x - error) / (p1.x - p2.x) * (p2 - p1);
	if(p2.y > 1.0)p2 = p1 + (1.0 - error - p1.y) / (p2.y - p1.y) * (p2 - p1);
	if(p2.x > 1.0)p2 = p1 + (1.0 - error - p1.x) / (p2.x - p1.x) * (p2 - p1);
	if(p2.y < 0.0)p2 = p1 + (p1.y - error) / (p1.y - p2.y) * (p2 - p1);
	return p2;
}
//扫描线算法计算step-屏幕投影长度
float GetStep(vec3 scene_start, vec3 scene_end)
{
	float len_x = abs(scene_start.x - scene_end.x);
	float len_y = abs(scene_start.y - scene_end.y);

	float size_inv = indirlightinfo.x;
	float dd = 0.0;
	float error = 0.0001;
	if(len_x < len_y)dd = len_x / (len_y + error) * size_inv;
	else dd = len_y / (len_x + error) * size_inv;	

	float step = sqrt(dd * dd + size_inv * size_inv);
	return step;
}

//由miplevel读取深度图
float ReadDepthMap_Linear(vec3 scene_pos, int miplevel)
{
	return texture2DLod(s_texDepth, scene_pos.xy, miplevel).x * 2.0 - 1.0;
}
float ReadDepthMap_Near(vec3 scene_pos, int miplevel)
{
	float base_size = indirlightinfo.x * pow(2.0, float(miplevel));
	int pixel_x = int(scene_pos.x / base_size);
	int pixel_y = int(scene_pos.y / base_size);
	float x = (float(pixel_x) + 0.5) * base_size;
	float y = (float(pixel_y) + 0.5) * base_size;
	float d = texture2DLod(s_texDepth, vec2(x,y), miplevel).x * 2.0 - 1.0;
	return d;
}
float ReadDepthMap_Min(vec3 scene_pos, int miplevel)
{
	float base_size = indirlightinfo.x * pow(2.0, float(miplevel));
	int pixel_x = int((scene_pos.x - 0.5 * base_size) / base_size);
	int pixel_y = int((scene_pos.y - 0.5 * base_size) / base_size);
	
	float x0 = (float(pixel_x) + 0.5) * base_size;
	float x1 = (float(pixel_x) + 1.5) * base_size;
	float y0 = (float(pixel_y) + 0.5) * base_size;
	float y1 = (float(pixel_y) + 1.5) * base_size;

	if(miplevel == 0)
	{
		x0 = scene_pos.x;
		x1 = scene_pos.x;
		y0 = scene_pos.y;
		y1 = scene_pos.y;
	}

	float d00 = texture2DLod(s_texDepth, vec2(x0,y0), miplevel).x * 2.0 - 1.0;
	float d01 = texture2DLod(s_texDepth, vec2(x0,y1), miplevel).x * 2.0 - 1.0;
	float d10 = texture2DLod(s_texDepth, vec2(x1,y0), miplevel).x * 2.0 - 1.0;
	float d11 = texture2DLod(s_texDepth, vec2(x1,y1), miplevel).x * 2.0 - 1.0;

	return min(min(d00, d01),min(d10,d11));
}

//返回该方向上的radiance
vec3 CastRay(vec3 start, vec3 end)
{
	//转换到屏幕空间
	vec3 scene_start = WorldToScene(start);
	vec3 scene_end = WorldToScene(GetViewCutEnd(start,end));

	//裁剪
	scene_end = GetCutEnd(scene_start, scene_end);

	//得到步进向量和步数
	vec3 start_to_end = scene_end - scene_start;
	vec3 dir = normalize(start_to_end);
	float len_step_2 = GetStep(scene_start, scene_end);
	float len_max_2 = length(start_to_end.xy);
	float len_max_3 = length(start_to_end);
	float len_step_3 = len_step_2 / len_max_2 * len_max_3;
	vec3 step = len_step_3 * dir;
	int num_max_steps = int(len_max_3 / len_step_3);
	
	vec3 curr_pos = scene_start;
	int flag = 0;//是否找到blocker
	float depth_error = 0.0001;
	int level = 0;
	int num_step = 0;
	int max_level = int(indirlightinfo.y);
	
	for(int i = 0; i < 256 && num_step < num_max_steps; i++)
	{
		float curr_num_step = pow(2.0, float(level));
		vec3 test_pos = curr_pos + step * curr_num_step;
		if(num_step + int(curr_num_step) > num_max_steps)
		{
			level = level - 1;
			max_level = level;
			if(level < 0)break;
			continue;
		}

		float depth_object = ReadDepthMap_Min(0.5 * (test_pos + curr_pos), level);
		float depth_pos =  max(test_pos.z, curr_pos.z);

		if(depth_object + depth_error < depth_pos)
		{
			level = level - 1;
			if(level < 0)
			{
				flag =1;
				break;
			}
		}
		else
		{
			if(level < max_level)level = level + 1;
			curr_pos = test_pos;
			num_step = num_step + int(curr_num_step);
		}
	}
	int ifobj = int(texture2D(s_texGBuffer4, curr_pos.xy).w);
	vec3 Li_indir = texture2D(s_texDirLight, curr_pos.xy).xyz;
	if(flag ==1 && ifobj ==1)return Li_indir;
	else return vec3_splat(0.0);
}

void main()
{
	//读取数据
	vec4 inter_point = texture2D(s_texGBuffer0, v_texcoord0);
	vec4 pp = mul(u_invViewProj, (inter_point * 2.0f - 1.0f));
	vec3 m_point= pp.xyz/pp.w;
	vec3 normal = normalize(texture2D(s_texGBuffer1, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 tvec = normalize(texture2D(s_texGBuffer2, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 bvec = cross(normal, tvec);
	vec3 color = texture2D(s_texGBuffer3, v_texcoord0).xyz;
	vec4 material = texture2D(s_texGBuffer4, v_texcoord0);
	
	float roughness = material.y;
	float metalness = material.z;
	float if_renderlight = material.w;

	vec3 ViewPoint = viewpoint.xyz;	
	int num_sample = int(indirlightinfo.z);
	float range = indirlightinfo.w;

	//计算中间量
	mat3 TBN = mtxFromCols(tvec, bvec, normal);
	mat3 TBN_inv = mtxFromRows(tvec, bvec, normal);
	vec3 wo = mul(TBN_inv , normalize(ViewPoint - m_point));
	
	//着色
	vec3 radiance = vec3_splat(0.0);

	for(int i = 0; i<1 && i < num_sample; i++)
	{
		vec3 wi = GetSample(i, wo);
		vec3 wi_world = mul(TBN, wi);

		vec3 start = m_point + 0.1 * normal;
		vec3 end = start + wi_world * range;
		vec3 sample_Li = CastRay(start, end);

		vec3 wh = normalize(wo + wi);
		vec3 ks = FresnelSchlick(wi, wh, color, metalness, roughness);
		vec3 fr = ks * fr_DG(wi, wo, wh, roughness);
		float PDF = GetPdf(wo, wh, roughness);
		radiance = radiance + max(vec3_splat(0.0), sample_Li * fr * max(0.0, wi.z) / PDF);
	}
	if(if_renderlight < 0.5)radiance = vec3_splat(0.0);
	radiance = min(vec3_splat(16.0), radiance / float(num_sample) + texture2D(s_texDirLight, v_texcoord0).xyz);
	gl_FragData[0] = vec4(radiance, 1.0f);
}
