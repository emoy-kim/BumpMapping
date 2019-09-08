#include "Object.h"

LightGL::LightGL() :
   TurnLightOn( true ), GlobalAmbientColor( 0.2f, 0.2f, 0.2f, 1.0f ), TotalLightNum( 0 )
{
}

bool LightGL::isLightOn() const
{
   return TurnLightOn;
}

void LightGL::toggleLightSwitch()
{
   TurnLightOn = !TurnLightOn;
}

void LightGL::addLight(
   const vec4& light_position,
   const vec4& ambient_color,
   const vec4& diffuse_color,
   const vec4& specular_color,
   const vec3& spotlight_direction,
   const float& spotlight_exponent,
   const float& spotlight_cutoff_angle_in_degree,
   const vec3& attenuation_factor
)
{
   Positions.emplace_back( light_position );
   
   AmbientColors.emplace_back( ambient_color );
   DiffuseColors.emplace_back( diffuse_color );
   SpecularColors.emplace_back( specular_color );

   SpotlightDirections.emplace_back( spotlight_direction );
   SpotlightExponents.emplace_back( spotlight_exponent );
   SpotlightCutoffAngles.emplace_back( spotlight_cutoff_angle_in_degree );

   AttenuationFactors.emplace_back( attenuation_factor );

   IsActivated.emplace_back( true );

   TotalLightNum = Positions.size();
}

void LightGL::setGlobalAmbientColor(const vec4& global_ambient_color)
{
   GlobalAmbientColor = global_ambient_color;
}

void LightGL::setAmbientColor(const vec4& ambient_color, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   AmbientColors[light_index] = ambient_color;
}

void LightGL::setDiffuseColor(const vec4& diffuse_color, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   DiffuseColors[light_index] = diffuse_color;
}

void LightGL::setSpecularColor(const vec4& specular_color, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   SpecularColors[light_index] = specular_color;
}

void LightGL::setSpotlightDirection(const vec3& spotlight_direction, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   SpotlightDirections[light_index] = spotlight_direction;
}

void LightGL::setSpotlightExponent(const float& spotlight_exponent, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   SpotlightExponents[light_index] = spotlight_exponent;
}

void LightGL::setSpotlightCutoffAngle(const float& spotlight_cutoff_angle_in_degree, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   SpotlightCutoffAngles[light_index] = spotlight_cutoff_angle_in_degree;
}

void LightGL::setAttenuationFactor(const vec3& attenuation_factor, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   AttenuationFactors[light_index] = attenuation_factor;
}

void LightGL::setLightPosition(const vec4& light_position, const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   Positions[light_index] = light_position;
}

void LightGL::activateLight(const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   IsActivated[light_index] = true;
}

void LightGL::deactivateLight(const uint& light_index)
{
   if (light_index >= TotalLightNum) return;
   IsActivated[light_index] = false;
}

void LightGL::transferUniformsToShader(ShaderGL& shader)
{
   glUniform1i( shader.Location.UseLight, TurnLightOn ? 1 : 0 );
   glUniform1i( shader.Location.LightNum, TotalLightNum );
   glUniform4fv( shader.Location.GlobalAmbient, 1, &GlobalAmbientColor[0] );

   for (uint i = 0; i < TotalLightNum; ++i) {
      glUniform1i( shader.Location.Lights[i].LightSwitch, IsActivated[0] ? 1 : 0 );
      glUniform4fv( shader.Location.Lights[i].LightPosition, 1, &Positions[i][0] );
      glUniform4fv( shader.Location.Lights[i].LightAmbient, 1, &AmbientColors[i][0] );
      glUniform4fv( shader.Location.Lights[i].LightDiffuse, 1, &DiffuseColors[i][0] );
      glUniform4fv( shader.Location.Lights[i].LightSpecular, 1, &SpecularColors[i][0] );
      glUniform3fv( shader.Location.Lights[i].SpotlightDirection, 1, &SpotlightDirections[i][0] ); 
      glUniform1f( shader.Location.Lights[i].SpotlightExponent, SpotlightExponents[i] );
      glUniform1f( shader.Location.Lights[i].SpotlightCutoffAngle, SpotlightCutoffAngles[i] );
      glUniform3fv( shader.Location.Lights[i].LightAttenuationFactors, 1, &AttenuationFactors[i][0] );
   }
}



ObjectGL::ObjectGL() : 
   ObjVAO( 0 ), ObjVBO( 0 ), DrawMode( 0 ), TextureID( 0 ), VerticesCount( 0 ),
   EmissionColor( 0.0f, 0.0f, 0.0f, 1.0f ), AmbientReflectionColor( 0.2f, 0.2f, 0.2f, 1.0f ),
   DiffuseReflectionColor( 0.8f, 0.8f, 0.8f, 1.0f ), SpecularReflectionColor( 0.0f, 0.0f, 0.0f, 1.0f ),
   SpecularReflectionExponent( 0.0f )
{
}

void ObjectGL::setEmissionColor(const vec4& emission_color)
{
   EmissionColor = emission_color;
}

void ObjectGL::setAmbientReflectionColor(const vec4& ambient_reflection_color)
{
   AmbientReflectionColor = ambient_reflection_color;   
}

void ObjectGL::setDiffuseReflectionColor(const vec4& diffuse_reflection_color)
{
   DiffuseReflectionColor = diffuse_reflection_color;
}

void ObjectGL::setSpecularReflectionColor(const vec4& specular_reflection_color)
{
   SpecularReflectionColor = specular_reflection_color;
}

void ObjectGL::setSpecularReflectionExponent(const float& specular_reflection_exponent)
{
   SpecularReflectionExponent = specular_reflection_exponent;
}

bool ObjectGL::prepareTexture2DFromFile(const string& file_name) const
{
   const FREE_IMAGE_FORMAT format = FreeImage_GetFileType( file_name.c_str(), 0 );
   FIBITMAP* texture = FreeImage_Load( format, file_name.c_str() );
   if (!texture) return false;

   const uint n_bits_per_pixel = FreeImage_GetBPP( texture );
   const uint n_bits = 32;
   FIBITMAP* texture_converted = n_bits_per_pixel == n_bits ? texture : FreeImage_ConvertTo32Bits( texture );

   const uint width = FreeImage_GetWidth( texture_converted );
   const uint height = FreeImage_GetHeight( texture_converted );
   GLvoid* data = FreeImage_GetBits( texture_converted );
   glTextureStorage2D( TextureID.back(), 1, GL_RGBA8, width, height );
   glTextureSubImage2D( TextureID.back(), 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, data );

   FreeImage_Unload( texture_converted );
   if (n_bits_per_pixel != n_bits) FreeImage_Unload( texture );
   return true;
}

size_t ObjectGL::addTexture(const string& texture_file_name)
{
   TextureID.emplace_back( 0 );
   glCreateTextures( GL_TEXTURE_2D, 1, &TextureID.back() );

   if (!prepareTexture2DFromFile( texture_file_name )) {
      cout << "Error: Image file cannot be loaded\n";
      glDeleteTextures( 1, &TextureID.back() );
      TextureID.erase( TextureID.end() );
      return 0;
   }

   glTextureParameteri( TextureID.back(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_T, GL_REPEAT );
   return TextureID.size() - 1;
}

size_t ObjectGL::addTexture(const uchar* image_buffer, const uint& width, const uint& height)
{
   TextureID.emplace_back( 0 );
   glCreateTextures( GL_TEXTURE_2D, 1, &TextureID.back() );
   glTextureStorage2D( TextureID.back(), 1, GL_RGBA8, width, height );
   glTextureSubImage2D( TextureID.back(), 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, image_buffer );

   glTextureParameteri( TextureID.back(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_T, GL_REPEAT );
   return TextureID.size() - 1;
}

size_t ObjectGL::addTexture(const float* image_buffer, const uint& width, const uint& height)
{
   TextureID.emplace_back( 0 );
   glCreateTextures( GL_TEXTURE_2D, 1, &TextureID.back() );
   glTextureStorage2D( TextureID.back(), 1, GL_RGB32F, width, height );
   glTextureSubImage2D( TextureID.back(), 0, 0, 0, width, height, GL_RGB, GL_FLOAT, image_buffer );

   glTextureParameteri( TextureID.back(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( TextureID.back(), GL_TEXTURE_WRAP_T, GL_REPEAT );
   return TextureID.size() - 1;
}


void ObjectGL::prepareTexture(const bool& normals_exist) const
{
   const uint offset = normals_exist ? 6 : 3;
   glVertexArrayAttribFormat( ObjVAO, TextureLoc, 2, GL_FLOAT, GL_FALSE, offset * sizeof( GLfloat ) );
   glEnableVertexArrayAttrib( ObjVAO, TextureLoc );
   glVertexArrayAttribBinding( ObjVAO, TextureLoc, 0 );
}

void ObjectGL::prepareTangent() const
{
   glVertexArrayAttribFormat( ObjVAO, TangentLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat) );
   glVertexArrayAttribBinding( ObjVAO, TangentLoc, 0 );
   glEnableVertexArrayAttrib( ObjVAO, TangentLoc );	
}

void ObjectGL::prepareNormal() const
{
   glVertexArrayAttribFormat( ObjVAO, NormalLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat) );
   glVertexArrayAttribBinding( ObjVAO, NormalLoc, 0 );
   glEnableVertexArrayAttrib( ObjVAO, NormalLoc );	
}

void ObjectGL::prepareVertexBuffer(const int& n_bytes_per_vertex)
{
   glCreateBuffers( 1, &ObjVBO );
   glNamedBufferStorage( ObjVBO, sizeof(GLfloat) * DataBuffer.size(), DataBuffer.data(), GL_DYNAMIC_STORAGE_BIT );

   glCreateVertexArrays( 1, &ObjVAO );
   glVertexArrayVertexBuffer( ObjVAO, 0, ObjVBO, 0, n_bytes_per_vertex );
   glVertexArrayAttribFormat( ObjVAO, VertexLoc, 3, GL_FLOAT, GL_FALSE, 0 );
   glVertexArrayAttribBinding( ObjVAO, VertexLoc, 0 );
   glEnableVertexArrayAttrib( ObjVAO, VertexLoc );
}

void ObjectGL::setObject(
   const GLenum& draw_mode, 
   const vector<vec3>& vertices, 
   const vector<vec3>& normals, 
   const vector<vec2>& textures,
   const string& texture_file_name
)
{
   DrawMode = draw_mode;
   
   vector<vec3> tangents;
   calculateTangent( tangents, vertices, textures );

   Mat normal_map;
   calculateNormalMap( normal_map, texture_file_name );

   for (uint i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      DataBuffer.push_back( textures[i].x );
      DataBuffer.push_back( textures[i].y );
      DataBuffer.push_back( tangents[i].x );
      DataBuffer.push_back( tangents[i].y );
      DataBuffer.push_back( tangents[i].z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 11 * sizeof(GLfloat);
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareNormal();
   prepareTexture( true );
   addTexture( texture_file_name );
   prepareTangent();
   addTexture( reinterpret_cast<float*>(normal_map.data), normal_map.cols, normal_map.rows );
}

bool ObjectGL::readObjectFile(
   vector<vec3>& vertices,
   vector<vec3>& normals, 
   vector<vec2>& textures, 
   const string& file_path
) const
{
   ifstream file(file_path);
   if (!file.is_open()) {
      cout << "The object file is not correct." << endl;
      return false;
   }

   vector<vec3> vertex_buffer, normal_buffer;
   vector<vec2> texture_buffer;
   vector<int> vertex_indices, normal_indices, texture_indices;
   while (!file.eof()) {
      string word;
      file >> word;
      
      if (word == "v") {
         vec3 vertex;
         file >> vertex.x >> vertex.y >> vertex.z;
         vertex_buffer.emplace_back( vertex );
      }
      else if (word == "vt") {
         vec2 uv;
         file >> uv.x >> uv.y;
         texture_buffer.emplace_back( uv );
      }
      else if (word == "vn") {
         vec3 normal;
         file >> normal.x >> normal.y >> normal.z;
         normal_buffer.emplace_back( normal );
      }
      else if (word == "f") {
         char c;
         vertex_indices.emplace_back(); file >> vertex_indices.back(); file >> c;
         texture_indices.emplace_back(); file >> texture_indices.back(); file >> c;
         normal_indices.emplace_back(); file >> normal_indices.back();
         vertex_indices.emplace_back(); file >> vertex_indices.back(); file >> c;
         texture_indices.emplace_back(); file >> texture_indices.back(); file >> c;
         normal_indices.emplace_back(); file >> normal_indices.back();
         vertex_indices.emplace_back(); file >> vertex_indices.back(); file >> c;
         texture_indices.emplace_back(); file >> texture_indices.back(); file >> c;
         normal_indices.emplace_back(); file >> normal_indices.back();
      }
      else getline( file, word );
   }

   for (uint i = 0; i < vertex_indices.size(); ++i) {
      vertices.emplace_back( vertex_buffer[vertex_indices[i] - 1] );
      normals.emplace_back( normal_buffer[normal_indices[i] - 1] );
      textures.emplace_back( texture_buffer[texture_indices[i] - 1] );
   }
   return true;
}

void ObjectGL::calculateTangent(
   vector<vec3>& tangents, 
   const vector<vec3>& vertices, 
   const vector<vec2>& textures
) const
{
   for (size_t i = 0; i < vertices.size(); i += 3) {
      const vec3 edge1 = vertices[i + 1] - vertices[i];
      const vec3 edge2 = vertices[i + 2] - vertices[i];
      const vec2 delta_uv1 = textures[i + 1] - textures[i];
      const vec2 delta_uv2 = textures[i + 2] - textures[i];
      const float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
      const vec3 tangent = glm::normalize( f * (delta_uv2.y * edge1 - delta_uv1.y * edge2) );
      tangents.emplace_back( tangent );
      tangents.emplace_back( tangent );
      tangents.emplace_back( tangent );
   }
}

void ObjectGL::calculateNormalMap(Mat& normal_map, const string& texture_file_name) const
{
   const Mat image = imread( texture_file_name );
   Mat gray_image;
   cvtColor( image, gray_image, CV_BGR2GRAY );

   Mat blurred;
   GaussianBlur( gray_image, blurred, Size(5, 5), 1.0 );
   blurred.convertTo( blurred, CV_32FC1 );
   flip( blurred, blurred, 0 );

   Mat dx, dy;
   Sobel( blurred, dx, CV_32FC1, 1, 0 );
   Sobel( blurred, dy, CV_32FC1, 0, 1 );

   normal_map.create( image.size(), CV_32FC3 );
   for (int j = 0; j < normal_map.rows; ++j) {
      const auto* dx_ptr = dx.ptr<float>(j);
      const auto* dy_ptr = dy.ptr<float>(j);
      auto* normal_ptr = normal_map.ptr<Vec3f>(j);
      for (int i = 0; i < normal_map.cols; ++i) {
         const vec3 x(1.0f, 0.0f, dx_ptr[i] / 255.0f);
         const vec3 y(0.0f, 1.0f, dy_ptr[i] / 255.0f);
         const vec3 n = normalize( cross( x, y ) ) * 0.5f + 0.5f;
         normal_ptr[i] = Vec3f(n.x, n.y, n.z);
      }
   }
}

void ObjectGL::setObject(
   const GLenum& draw_mode, 
   const string& obj_file_path, 
   const string& texture_file_name
)
{
   DrawMode = draw_mode;
   vector<vec3> vertices, normals;
   vector<vec2> textures;
   readObjectFile( vertices, normals, textures, obj_file_path );
   
   vector<vec3> tangents;
   calculateTangent( tangents, vertices, textures );

   Mat normal_map;
   calculateNormalMap( normal_map, texture_file_name );

   for (uint i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      DataBuffer.push_back( textures[i].x );
      DataBuffer.push_back( textures[i].y );
      DataBuffer.push_back( tangents[i].x );
      DataBuffer.push_back( tangents[i].y );
      DataBuffer.push_back( tangents[i].z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 11 * sizeof(GLfloat);
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareNormal();
   prepareTexture( true );
   addTexture( texture_file_name );
   prepareTangent();
   addTexture( reinterpret_cast<float*>(normal_map.data), normal_map.cols, normal_map.rows );
}

void ObjectGL::transferUniformsToShader(ShaderGL& shader)
{
   glUniform4fv( shader.Location.MaterialEmission, 1, &EmissionColor[0] );
   glUniform4fv( shader.Location.MaterialAmbient, 1, &AmbientReflectionColor[0] );
   glUniform4fv( shader.Location.MaterialDiffuse, 1, &DiffuseReflectionColor[0] );
   glUniform4fv( shader.Location.MaterialSpecular, 1, &SpecularReflectionColor[0] );
   glUniform1f( shader.Location.MaterialSpecularExponent, SpecularReflectionExponent );

   for (const auto& texture : shader.Location.Texture) {
      glUniform1i( texture.second, texture.first );
   }
}