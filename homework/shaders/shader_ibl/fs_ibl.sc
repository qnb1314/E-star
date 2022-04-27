$input v_texcoord0
#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texGBuffer0,  0);
SAMPLER2D(s_texGBuffer1,  1);
SAMPLER2D(s_texGBuffer2,  2);
SAMPLER2D(s_texGBuffer3,  3);
SAMPLER2D(s_texGBuffer4,  4);

SAMPLERCUBE(s_texCube, 5);
SAMPLERCUBE(s_texCubeIrr, 6);
SAMPLER2D(s_texbrdfLUT, 7);

uniform vec4 viewpoint;

vec3 FresnelSchlick(vec3 wi, vec3 wh, vec3 color, float metalness, float roughness)
{
	vec3 F0 = mix(vec3_splat(0.04), color, metalness);
	return F0 + (max(vec3_splat(1.0-roughness),F0) - F0) * pow(1.0f - dot(wi, wh), 5.0f);
}

void main()
{
	//统一值格式转换
	vec3 ViewPoint = viewpoint.xyz;

	//1.读取数据
	vec4 pp = mul(u_invViewProj, (texture2D(s_texGBuffer0, v_texcoord0) * 2.0f - 1.0f));
	vec3 m_point = pp.xyz / pp.w;
	vec3 normal = normalize(texture2D(s_texGBuffer1, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 tvec = normalize(texture2D(s_texGBuffer2, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 bvec = cross(normal, tvec);
	vec3 color = texture2D(s_texGBuffer3, v_texcoord0).xyz;
	vec4 material = texture2D(s_texGBuffer4, v_texcoord0);

	float ao = material.x;
	float roughness = material.y;
	float metalness = material.z;
	float if_renderlight = material.w;
	
	//2.计算中间量
	mat3 TBN = mtxFromCols(tvec, bvec, normal);
	mat3 TBN_inv = mtxFromRows(tvec, bvec, normal);
	vec3 wo = mul(TBN_inv , normalize(ViewPoint - m_point));
	vec3 wi = 2.0 * vec3(0.0,0.0,wo.z) - wo;
	vec3 wi_world = mul(TBN, wi);
	
	float miplevel = 1.0 + 8.0 * roughness;
	vec3 ks = FresnelSchlick(wi, vec3(0.0,0.0,1.0), color, metalness,roughness);	
	vec3 kd = (vec3_splat(1.0) - ks) * (1.0 - metalness);

	//3.计算辐射度
	vec3 irradiance = toLinear(textureCube(s_texCubeIrr, normal).xyz);
	vec3 radiance =  toLinear(textureCubeLod(s_texCube, wi_world, miplevel).xyz);

	vec2 envBRDF = texture2D(s_texbrdfLUT,vec2(min(0.99,max(0.01,wi.z)),roughness)).xy;	

	vec3 diffuse = kd * color * irradiance;
	vec3 specular = radiance * (ks * envBRDF.x + envBRDF.y);

	vec3 indirect = step(0.5, if_renderlight) * max(diffuse + specular, 0.0) * ao + step(if_renderlight, 0.5) * color;

	//4.输出辐射度
	gl_FragData[0] = vec4(indirect, 1.0f);
}
