$input  a_texcoord0
$output  v_texcoord0

#include "../../bgfx/examples/common/common.sh"

void main()
{
	v_texcoord0 = a_texcoord0;
	gl_Position = vec4(a_texcoord0*2.0f-1.0f,0.5,1.0);
}
