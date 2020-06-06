#version 460

uniform mat4 WorldMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewProjectionMatrix;

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_tex_coord;
layout (location = 3) in vec3 v_tangent;  

out vec3 position_in_mc;
out vec2 tex_coord;

out vec3 normal_in_mc;
out vec3 tangent_in_mc;
out vec3 binormal_in_mc;

void main()
{   
   position_in_mc = (WorldMatrix * vec4(v_position, 1.0f)).xyz;
   tex_coord = v_tex_coord;    

   normal_in_mc = normalize( v_normal );
   tangent_in_mc = normalize( v_tangent );
   binormal_in_mc = cross( v_normal, v_tangent );
   
   gl_Position = ModelViewProjectionMatrix * vec4(v_position, 1.0f);
}