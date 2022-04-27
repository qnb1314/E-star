$input  a_position
$output v_worldpos
#include "../../bgfx/examples/common/common.sh"

void main()
{
	vec4 p = mul(u_model[0], vec4(a_position, 1.0));
	v_worldpos = p.xyz;
	gl_Position =mul(u_viewProj, p);
}
