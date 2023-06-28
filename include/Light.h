#pragma once

#include "Shader.h"

class LightGL final
{
public:
   LightGL();
   ~LightGL() = default;

   [[nodiscard]] bool isLightOn() const;
   void toggleLightSwitch();
   void addLight(
      const glm::vec4& light_position,
      const glm::vec4& ambient_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
      const glm::vec4& diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
      const glm::vec4& specular_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
      const glm::vec3& spotlight_direction = glm::vec3(0.0f, 0.0f, -1.0f),
      float spotlight_cutoff_angle_in_degree = 180.0f,
      float spotlight_feather = 0.0f,
      float falloff_radius = 1000.0f
   );
   void setLightPosition(const glm::vec4& light_position, int index)
   {
      assert( 0 <= index && index < Positions.size() );
      Positions[index] = light_position;
   }
   void activateLight(const int& light_index);
   void deactivateLight(const int& light_index);
   void transferUniformsToShader(const ShaderGL* shader);
   [[nodiscard]] int getTotalLightNum() const { return TotalLightNum; }
   [[nodiscard]] glm::vec4 getLightPosition(int light_index) { return Positions[light_index]; }

private:
   bool TurnLightOn;
   int TotalLightNum;
   glm::vec4 GlobalAmbientColor;
   std::vector<bool> IsActivated;
   std::vector<glm::vec4> Positions;
   std::vector<glm::vec4> AmbientColors;
   std::vector<glm::vec4> DiffuseColors;
   std::vector<glm::vec4> SpecularColors;
   std::vector<glm::vec3> SpotlightDirections;
   std::vector<float> SpotlightCutoffAngles;
   std::vector<float> SpotlightFeathers;
   std::vector<float> FallOffRadii;
};