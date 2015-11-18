
__kernel void calc_next(global const float  *masses,
					    global       float3 *pos,
					           const float   c1)		// time
{ 
	size_t g_size = get_global_size(0);
	size_t g_id   = get_global_id(0);

	float3 my_pos = pos[g_id];

	float c = ((float)g_id) * 2.0f * M_PI_F / ((float)g_size);

	pos[g_id] = (float3)(my_pos.x, sin(c * c1), my_pos.z);
}
