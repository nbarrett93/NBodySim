#version 330 core

in vec3 light_dir_cam;
in vec3 normal_cam;

out vec3 color;

const float AMBIENT = 0.1;
const vec3 AMBIENT_COLOR = vec3(1.0, 1.0, 1.0);

const vec3 DIFFUSE_COLOR = vec3(0.0, 0.0, 1.0);

void main()
{
    vec3 light_dir_normalized = normalize(light_dir_cam);
    vec3 normal_cam_normalized = normalize(normal_cam);
    
    float diffuse_ratio = max(0.0, dot(normal_cam_normalized, light_dir_normalized));

    color = AMBIENT * AMBIENT_COLOR + diffuse_ratio * DIFFUSE_COLOR;
}