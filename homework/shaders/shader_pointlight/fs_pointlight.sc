$input v_texcoord0
#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texGBuffer0,  0);
SAMPLER2D(s_texGBuffer1,  1);
SAMPLER2D(s_texGBuffer2,  2);
SAMPLER2D(s_texGBuffer3,  3);
SAMPLER2D(s_texGBuffer4,  4);
SAMPLERCUBE(s_texShadowMap, 5);

uniform vec4 viewpoint;

uniform vec4 lightpoint;
uniform vec4 lightpower;
uniform vec4 lightinfo;//area,far,size_inv

//微元面模型BRDF
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

vec3 FresnelSchlick(vec3 wi, vec3 wh, vec3 color, float metalness)
{
	vec3 F0 = mix(vec3_splat(0.04), color, metalness);
	return F0 + (vec3_splat(1.0) - F0) * pow(1.0f - dot(wi, wh), 5.0f);
}
float fr_DG(vec3 wi, vec3 wo, vec3 wh, float roughness)
{
	float a = roughness * roughness;
	float k = 0.125 * (a + 1.0) * (a + 1.0);
	float NoV = max(wo.z, 0.001);
	float NoL = max(wi.z, 0.001);
	float NoH = max(wh.z, 0.0);
	return D(NoH, a) * G(NoV, NoL, k)  / (4.0 * NoV * NoL);
}

//全漫反射BRDF
vec3 fr_diffuse(vec3 color)
{
	float PI = 3.14159f;
	return color / PI;
}


//PCSS
vec3 Getvec3(float nx, float ny, vec3 v)
{
	float PI = 3.14159f;
	float size_inv = lightinfo.z;
	float dtheta = PI * 0.5f * size_inv;
	
	float theta = acos(v.y);
	float phy = atan(v.z / v.x);
	phy += step(v.x, 0.0) * PI + step(0.0, v.x) * step(v.z, 0.0) * 2.0 * PI;

	theta -= nx * dtheta;
	phy -= ny * dtheta;

	return vec3(sin(theta) * cos(phy), cos(theta), sin(theta) * sin(phy));
}

float ReciprocalRoot(int i, int b)
{
	float res = 0.0f;
   	float b_inv = 1.0f / float(b);
    	float b_inv_product = b_inv;
    	while (i >= b)
    	{
        		int aa = i - i / b * b;
        		i = i / b;
        		res += float(aa) * b_inv_product;
        		b_inv_product *= b_inv;
    	}
    	res += float(i) * b_inv_product;
    	return res;
}


float GetDistanBlocker(vec3 LightToPoint)
{
	float far = lightinfo.y;
	//在5*5范围内寻找blocker
	float sum_dblocker = 0.0f;
	float num = 0.0f;

	vec3 light_to_point = normalize(LightToPoint);
	float distance_lp = length(LightToPoint); 

	for (int i = -2; i <= 2; i++)
	{
		for (int j = -2; j <= 2; j++)
		{
			vec3 curr_vec = Getvec3(float(i), float(j), light_to_point);
			float delta_dblocker = textureCube(s_texShadowMap, curr_vec).x * far;
			float checkifblock = step(delta_dblocker, distance_lp);
			num += checkifblock;
			sum_dblocker += checkifblock * delta_dblocker;
		}
	}
	
	return sum_dblocker / max(num, 0.1);
}

float GetLightPercent(vec3 LightToPoint)
{
	float ERROR = 0.0001f;
	float Area = lightinfo.x;
	float num_of_sample = max(10.0f, Area / 5.0f);
	float far = lightinfo.y;

	vec3 light_to_point = normalize(LightToPoint);
	float dblocker = GetDistanBlocker(LightToPoint);
	float distance_lp = length(LightToPoint); 
	
	float filter_size = step(ERROR, dblocker) * (distance_lp - dblocker) / max(dblocker, ERROR) * Area;

	float num_of_light = 0.0f;

	for(int i = 0; i<30&&i < int(num_of_sample); i++)
	{
		//先得到随机数
		float nx = (ReciprocalRoot(i + 1, 2) - 0.5f) * filter_size;
		float ny = (ReciprocalRoot(i + 1, 3) - 0.5f) * filter_size;
		vec3 curr_vec = Getvec3(nx, ny, light_to_point);
		float dis_temp = textureCube(s_texShadowMap, curr_vec).x * far;
		float checkiflight = step(distance_lp, dis_temp);
		num_of_light += checkiflight;
	}
	
	return step(ERROR, dblocker) * num_of_light / min(30.0, num_of_sample) + step(dblocker, ERROR);
}

void main()
{
	//1.读取数据
	vec4 pp = mul(u_invViewProj, (texture2D(s_texGBuffer0, v_texcoord0) * 2.0f - 1.0f));
	vec3 m_point= pp.xyz/pp.w;
	vec3 normal = normalize(texture2D(s_texGBuffer1, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 tvec = normalize(texture2D(s_texGBuffer2, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 bvec = cross(normal, tvec);
	vec3 color = texture2D(s_texGBuffer3, v_texcoord0).xyz;
	vec4 material = texture2D(s_texGBuffer4, v_texcoord0);
	
	float far = lightinfo.y;
	vec3 ViewPoint = viewpoint.xyz;
	vec3 LightPoint = lightpoint.xyz;
	vec3 LightPower = lightpower.xyz;	

	float roughness = material.y;
	float metalness = material.z;
	float if_renderlight = material.w;

	//2.计算中间量
	mat3 TBN = mtxFromCols(tvec, bvec, normal);
	mat3 TBN_inv = mtxFromRows(tvec, bvec, normal);
	vec3 wo = mul(TBN_inv , normalize(ViewPoint - m_point));
	vec3 point_to_Light = LightPoint - m_point;
	vec3 wi = mul(TBN_inv , normalize(point_to_Light));
	vec3 wh = normalize(wo + wi);

	vec3 ks = FresnelSchlick(wi, wh, color, metalness);
	vec3 kd = (vec3_splat(1.0) - ks) * (1.0 - metalness);

	//3.计算brdf
	vec3 fr_sum = ks * fr_DG(wi, wo, wh, roughness) + kd * fr_diffuse(color);


	//4.计算光照部分
	vec3 light = step(length(point_to_Light), far) * GetLightPercent(-point_to_Light) * LightPower * max(0.0f, wi.z) / dot(point_to_Light, point_to_Light);


	//5.计算辐射度
	vec3 radiance = step(0.5, if_renderlight) * max(fr_sum * light, 0.0);
	

	//6.输出辐射度
	gl_FragData[0] = vec4(radiance, 1.0f);
}
