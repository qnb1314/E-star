$input a_position,a_normal,a_texcoord0,a_tangent,a_bitangent 
$output v_interpos,v_normal,v_texcoord0,v_tangent,v_bitangent

#include "../../bgfx/examples/common/common.sh"

void main()
{
	vec4 p=mul(u_viewProj,mul(u_model[0], vec4(a_position, 1.0)));
	gl_Position=p;
	v_interpos=p;
	v_normal = normalize(mul(u_model[1] , vec4(a_normal,0.0f)).xyz);
	v_texcoord0 = a_texcoord0;
	v_tangent = normalize(mul(u_model[0] , vec4(a_tangent,0.0f)).xyz);
	v_bitangent = normalize(mul(u_model[0] , vec4(a_bitangent,0.0f)).xyz);
}
