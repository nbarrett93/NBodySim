
__kernel void calc_next(global       float3 *pos,
					           const float   time)		// time
{ 
	size_t g_size = get_global_size(0);
	size_t g_id   = get_global_id(0);

	float3 my_pos = pos[g_id];

	float c = ((float)g_id + 1) * 2.0f * M_PI_F / ((float)g_size);

	//pos[g_id] = as_float3(my_pos.x, sin(c * time), my_pos.z);
    
    const float r = 3.0f;
    float x = 0.0f;
    float z = 0.0f;
    x = sincos(c * time, &z);
    
    pos[g_id] = (float3)(r * x, 0.0f, r * z);
}
