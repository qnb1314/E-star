$input v_interpos,v_normal,v_texcoord0,v_tangent,v_bitangent

#include "../../bgfx/examples/common/common.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);
SAMPLER2D(s_texAorm, 2);

void main()
{
	//处理法向
	vec3 normal_TBN = normalize(texture2D(s_texNormal, v_texcoord0).xyz * 2.0f - 1.0f);
	vec3 N = normalize(v_normal);
	vec3 T = normalize(v_tangent);
	vec3 B = normalize(v_bitangent);
	T = T - dot(N, T) * N;
	B = B - dot(N, B) * N - dot(T, B) * T;
	T = normalize(T);
	B = normalize(B);
	mat3 TBN = mtxFromCols(T, B, N);
	N = mul(TBN , normal_TBN);
	//处理切向
	T = T - dot(N, T) * N;
	T = normalize(T);

	vec3 color = texture2D(s_texColor, v_texcoord0).xyz;
	vec3 aorm = texture2D(s_texAorm, v_texcoord0).xyz;

	gl_FragData[0] = (v_interpos / v_interpos.w) * 0.5f + 0.5f;//顶点
	gl_FragData[1] = vec4(N, .0f) * 0.5f + 0.5f;//法线
	gl_FragData[2] = vec4(T, .0f) * 0.5f + 0.5f;//T->之后B可以通过cross(N,T)得到
	gl_FragData[3] = vec4(color, 1.0f);//颜色信息
	gl_FragData[4] = vec4(aorm,1.0f);//材质信息
}
