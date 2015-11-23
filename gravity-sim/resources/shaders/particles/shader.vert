#version 330 core
layout (location = 0) in vec4 offset;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 position;

uniform mat4 mvp_matrix;
uniform mat4 v_matrix;
uniform vec4 light_pos_world;

out vec3 light_dir_cam;
out vec3 normal_cam;
out vec3 vert_pos_cam;

void main()
{
    vec4 total_position = position + offset;
    vec4 normal_better = vec4(normal.xyz, 0.0);

    gl_Position = mvp_matrix * total_position;
    
    vec3 vertex_pos_cam = vec3(v_matrix * total_position);
    vec3 light_pos_cam = vec3(v_matrix * light_pos_world);
    
    light_dir_cam = light_pos_cam - vertex_pos_cam;
    normal_cam = vec3(v_matrix * normal_better);
    // flip it around since it needs to be pointing away from the vertex
    vert_pos_cam = -1.0 * vertex_pos_cam;
}