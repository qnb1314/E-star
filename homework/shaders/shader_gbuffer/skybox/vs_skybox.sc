$input  a_position
$output v_modelpos
#include "../../bgfx/examples/common/common.sh"

void main()
{
	v_modelpos = a_position;
	vec4 p = mul(u_viewProj,mul(u_model[0], vec4(a_position, 1.0)));
	p.z = p.w;
	gl_Position = p;
}
