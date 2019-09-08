/*
 * Author: Emoy Kim
 * E-mail: emoy.kim_AT_gmail.com
 * 
 * This code is a free software; it can be freely used, changed and redistributed.
 * If you use any version of the code, please reference the code.
 * 
 */

#pragma once

#include "Shader.h"

class LightGL
{
   bool TurnLightOn;

   vec4 GlobalAmbientColor;

   vector<bool> IsActivated;
   vector<vec4> Positions;
   
   vector<vec4> AmbientColors;
   vector<vec4> DiffuseColors;
   vector<vec4> SpecularColors;

   vector<vec3> SpotlightDirections;
   vector<float> SpotlightExponents;
   vector<float> SpotlightCutoffAngles;

   vector<vec3> AttenuationFactors;

public:
   uint TotalLightNum;

   LightGL();

   bool isLightOn() const;
   void toggleLightSwitch();

   void addLight(
      const vec4& light_position,
      const vec4& ambient_color = vec4(0.0f, 0.0f, 0.0f, 1.0f),
      const vec4& diffuse_color = vec4(1.0f, 1.0f, 1.0f, 1.0f),
      const vec4& specular_color = vec4(1.0f, 1.0f, 1.0f, 1.0f),
      const vec3& spotlight_direction = vec3(0.0f, 0.0f, -1.0f),
      const float& spotlight_exponent = 0.0f,
      const float& spotlight_cutoff_angle_in_degree = 180.0f,
      const vec3& attenuation_factor = vec3(1.0f, 0.0f, 0.0f)
   );

   void setGlobalAmbientColor(const vec4& global_ambient_color);
   void setAmbientColor(const vec4& ambient_color, const uint& light_index);
   void setDiffuseColor(const vec4& diffuse_color, const uint& light_index);
   void setSpecularColor(const vec4& specular_color, const uint& light_index);
   void setSpotlightDirection(const vec3& spotlight_direction, const uint& light_index);
   void setSpotlightExponent(const float& spotlight_exponent, const uint& light_index);
   void setSpotlightCutoffAngle(const float& spotlight_cutoff_angle_in_degree, const uint& light_index);
   void setAttenuationFactor(const vec3& attenuation_factor, const uint& light_index);
   void setLightPosition(const vec4& light_position, const uint& light_index);
   void activateLight(const uint& light_index);
   void deactivateLight(const uint& light_index);

   void transferUniformsToShader(ShaderGL& shader);
};


class ObjectGL
{
   vector<GLfloat> DataBuffer; // 3 for vertex, 3 for normal, 2 for texture, and 3 for tangent

   void prepareTangent() const;
   
   bool prepareTexture2DFromFile(const string& file_name) const;
   void prepareTexture(const bool& normals_exist) const;

   void prepareVertexBuffer(const int& n_bytes_per_vertex);
   void prepareNormal() const;

   bool readObjectFile(vector<vec3>& vertices, vector<vec3>& normals, vector<vec2>& textures, const string& file_path) const;
   void calculateTangent(vector<vec3>& tangents, const vector<vec3>& vertices, const vector<vec2>& textures) const;
   void calculateNormalMap(Mat& normal_map, const string& texture_file_name) const;

public:
   enum LayoutLocation { VertexLoc=0, NormalLoc, TextureLoc, TangentLoc };

   GLuint ObjVAO, ObjVBO;
   GLenum DrawMode;
   vector<GLuint> TextureID;
   GLsizei VerticesCount;
   vec4 EmissionColor;
   vec4 AmbientReflectionColor; // It is usually set to the same color with DiffuseReflectionColor.
                                // Otherwise, it should be in balance with DiffuseReflectionColor.
   vec4 DiffuseReflectionColor; // the intrinsic color
   vec4 SpecularReflectionColor;
   float SpecularReflectionExponent;


   ObjectGL();

   void setEmissionColor(const vec4& emission_color);
   void setAmbientReflectionColor(const vec4& ambient_reflection_color);
   void setDiffuseReflectionColor(const vec4& diffuse_reflection_color);
   void setSpecularReflectionColor(const vec4& specular_reflection_color);
   void setSpecularReflectionExponent(const float& specular_reflection_exponent);

   void setObject(
      const GLenum& draw_mode, 
      const vector<vec3>& vertices, 
      const vector<vec3>& normals, 
      const vector<vec2>& textures,
      const string& texture_file_name
   );

   void setObject(
      const GLenum& draw_mode, 
      const string& obj_file_path, 
      const string& texture_file_name
   );

   size_t addTexture(const string& texture_file_name);
   size_t addTexture(const uchar* image_buffer, const uint& width, const uint& height);
   size_t addTexture(const float* image_buffer, const uint& width, const uint& height);
   void transferUniformsToShader(ShaderGL& shader);
};