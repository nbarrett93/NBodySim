__constant float N_GRAV_CONST = -0.01f;

__kernel void calc_next(global const float  *masses,
					    global const float3 *prev_pos,
					    global const float3 *cur_pos,
					    global       float3 *new_pos,
					    global       float  *color_out,
					           const float   c1,		// needs to be calculated by CPU
							   const float   c2)		// similarly
{ 
	size_t g_size = get_global_size(0);
	size_t g_id   = get_global_id(0);

	float3 accel = (float3)(0.0f);

	float3 my_pos = cur_pos[g_id];

	for (size_t i = 0; i < g_id; ++i)
	{
		float3 diff = my_pos - cur_pos[i];
		float len = max(length(diff), 0.0001f);
		accel += masses[i] * pown(len, -3) * diff;
	}

	for (size_t i = g_id + 1; i < g_size; ++i)
	{ 
		float3 diff = my_pos - cur_pos[i];
		float len = max(length(diff), 0.0001f);
		accel += masses[i] * pown(len, -3) * diff;
	}

	accel *= N_GRAV_CONST;

	//float c1 = time_step1 / time_step2;
	//float c2 = 0.5f * (time_step1 + time_step2) * time_step1;
	new_pos[g_id] = my_pos + 
		(my_pos - prev_pos[g_id]) * c1 + 
		c2 * accel;
}
