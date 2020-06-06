#include "Object.h"

#include "opencv2/core/matx.hpp"

ObjectGL::ObjectGL() :
   ImageBuffer( nullptr ), VAO( 0 ), VBO( 0 ), DrawMode( 0 ), VerticesCount( 0 ),
   EmissionColor( 0.0f, 0.0f, 0.0f, 1.0f ),
   AmbientReflectionColor( 0.2f, 0.2f, 0.2f, 1.0f ),
   DiffuseReflectionColor( 0.8f, 0.8f, 0.8f, 1.0f ),
   SpecularReflectionColor( 0.0f, 0.0f, 0.0f, 1.0f ), SpecularReflectionExponent( 0.0f )
{
}

ObjectGL::~ObjectGL()
{
   if (VAO != 0) {
      glDeleteVertexArrays( 1, &VAO );
      glDeleteBuffers( 1, &VBO );
   }
   for (const auto& texture_id : TextureID) {
      if (texture_id != 0) glDeleteTextures( 1, &texture_id );
   }
   for (const auto& buffer : CustomBuffers) {
      if (buffer.second != 0) glDeleteBuffers( 1, &buffer.second );
   }
   delete [] ImageBuffer;
}

void ObjectGL::setEmissionColor(const glm::vec4& emission_color)
{
   EmissionColor = emission_color;
}

void ObjectGL::setAmbientReflectionColor(const glm::vec4& ambient_reflection_color)
{
   AmbientReflectionColor = ambient_reflection_color;
}

void ObjectGL::setDiffuseReflectionColor(const glm::vec4& diffuse_reflection_color)
{
   DiffuseReflectionColor = diffuse_reflection_color;
}

void ObjectGL::setSpecularReflectionColor(const glm::vec4& specular_reflection_color)
{
   SpecularReflectionColor = specular_reflection_color;
}

void ObjectGL::setSpecularReflectionExponent(const float& specular_reflection_exponent)
{
   SpecularReflectionExponent = specular_reflection_exponent;
}

bool ObjectGL::prepareTexture2DUsingFreeImage(const std::string& file_path, bool is_grayscale) const
{
   const FREE_IMAGE_FORMAT format = FreeImage_GetFileType( file_path.c_str(), 0 );
   FIBITMAP* texture = FreeImage_Load( format, file_path.c_str() );
   if (!texture) return false;

   FIBITMAP* texture_converted;
   const uint n_bits_per_pixel = FreeImage_GetBPP( texture );
   const uint n_bits = is_grayscale ? 8 : 32;
   if (is_grayscale) {
      texture_converted = n_bits_per_pixel == n_bits ? texture : FreeImage_GetChannel( texture, FICC_RED );
   }
   else {
      texture_converted = n_bits_per_pixel == n_bits ? texture : FreeImage_ConvertTo32Bits( texture );
   }

   const GLsizei width = FreeImage_GetWidth( texture_converted );
   const GLsizei height = FreeImage_GetHeight( texture_converted );
   GLvoid* data = FreeImage_GetBits( texture_converted );
   glTextureStorage2D( TextureID.back(), 1, is_grayscale ? GL_R8 : GL_RGBA8, width, height );
   glTextureSubImage2D( TextureID.back(), 0, 0, 0, width, height, is_grayscale ? GL_RED : GL_BGRA, GL_UNSIGNED_BYTE, data );

   FreeImage_Unload( texture_converted );
   if (n_bits_per_pixel != n_bits) FreeImage_Unload( texture );
   return true;
}

int ObjectGL::addTexture(const std::string& texture_file_path, bool is_grayscale)
{
   GLuint texture_id = 0;
   glCreateTextures( GL_TEXTURE_2D, 1, &texture_id );
   TextureID.emplace_back( texture_id );
   if (!prepareTexture2DUsingFreeImage( texture_file_path, is_grayscale )) {
      glDeleteTextures( 1, &texture_id );
      TextureID.erase( TextureID.end() - 1 );
      std::cerr << "Could not read image file " << texture_file_path.c_str() << "\n";
      return -1;
   }

   glTextureParameteri( texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glGenerateTextureMipmap( texture_id );
   return static_cast<int>(TextureID.size() - 1);
}

void ObjectGL::addTexture(int width, int height, bool is_grayscale)
{
   GLuint texture_id = 0;
   glCreateTextures( GL_TEXTURE_2D, 1, &texture_id );
   glTextureStorage2D(
      texture_id,
      1,
      is_grayscale ? GL_R8 : GL_RGBA8,
      width,
      height
   );
   glTextureParameteri( texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT );
   glGenerateTextureMipmap( texture_id );
   TextureID.emplace_back( texture_id );
}

int ObjectGL::addTexture(const uint8_t* image_buffer, int width, int height, bool is_grayscale)
{
   addTexture( width, height, is_grayscale );
   glTextureSubImage2D(
      TextureID.back(),
      0,
      0,
      0,
      width,
      height,
      is_grayscale ? GL_RED : GL_RGBA,
      GL_UNSIGNED_BYTE,
      image_buffer
   );
   return static_cast<int>(TextureID.size() - 1);
}

int ObjectGL::addTexture(const float* image_buffer, int width, int height)
{
   GLuint texture_id = 0;
   glCreateTextures( GL_TEXTURE_2D, 1, &texture_id );
   glTextureStorage2D( texture_id, 1, GL_RGB32F, width, height );
   glTextureSubImage2D( texture_id, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, image_buffer );

   glTextureParameteri( texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_S, GL_REPEAT );
   glTextureParameteri( texture_id, GL_TEXTURE_WRAP_T, GL_REPEAT );
   TextureID.emplace_back( texture_id );
   return static_cast<int>(TextureID.size() - 1);
}

void ObjectGL::prepareTexture(bool normals_exist) const
{
   const uint offset = normals_exist ? 6 : 3;
   glVertexArrayAttribFormat( VAO, TextureLoc, 2, GL_FLOAT, GL_FALSE, offset * sizeof( GLfloat ) );
   glEnableVertexArrayAttrib( VAO, TextureLoc );
   glVertexArrayAttribBinding( VAO, TextureLoc, 0 );
}

void ObjectGL::prepareTangent() const
{
   glVertexArrayAttribFormat( VAO, TangentLoc, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( GLfloat ) );
   glVertexArrayAttribBinding( VAO, TangentLoc, 0 );
   glEnableVertexArrayAttrib( VAO, TangentLoc );	
}

void ObjectGL::prepareNormal() const
{
   glVertexArrayAttribFormat( VAO, NormalLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ) );
   glEnableVertexArrayAttrib( VAO, NormalLoc );
   glVertexArrayAttribBinding( VAO, NormalLoc, 0 );
}

void ObjectGL::prepareVertexBuffer(int n_bytes_per_vertex)
{
   glCreateBuffers( 1, &VBO );
   glNamedBufferStorage( VBO, sizeof( GLfloat ) * DataBuffer.size(), DataBuffer.data(), GL_DYNAMIC_STORAGE_BIT );

   glCreateVertexArrays( 1, &VAO );
   glVertexArrayVertexBuffer( VAO, 0, VBO, 0, n_bytes_per_vertex );
   glVertexArrayAttribFormat( VAO, VertexLoc, 3, GL_FLOAT, GL_FALSE, 0 );
   glEnableVertexArrayAttrib( VAO, VertexLoc );
   glVertexArrayAttribBinding( VAO, VertexLoc, 0 );
}

void ObjectGL::getSquareObject(
   std::vector<glm::vec3>& vertices,
   std::vector<glm::vec3>& normals,
   std::vector<glm::vec2>& textures
)
{
   vertices = {
      { 1.0f, 0.0f, 0.0f },
      { 1.0f, 1.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },

      { 1.0f, 0.0f, 0.0f },
      { 0.0f, 1.0f, 0.0f },
      { 0.0f, 0.0f, 0.0f }
   };
   normals = {
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },

      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f },
      { 0.0f, 0.0f, 1.0f }
   };
   textures = {
      { 1.0f, 0.0f },
      { 1.0f, 1.0f },
      { 0.0f, 1.0f },

      { 1.0f, 0.0f },
      { 0.0f, 1.0f },
      { 0.0f, 0.0f }
   };
}

void ObjectGL::setObject(GLenum draw_mode, const std::vector<glm::vec3>& vertices)
{
   DrawMode = draw_mode;
   VerticesCount = 0;
   DataBuffer.clear();
   for (const auto& vertex : vertices) {
      DataBuffer.push_back( vertex.x );
      DataBuffer.push_back( vertex.y );
      DataBuffer.push_back( vertex.z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 3 * sizeof( GLfloat );
   prepareVertexBuffer( n_bytes_per_vertex );
}

void ObjectGL::setObject(
   GLenum draw_mode,
   const std::vector<glm::vec3>& vertices,
   const std::vector<glm::vec3>& normals
)
{
   DrawMode = draw_mode;
   VerticesCount = 0;
   DataBuffer.clear();
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 6 * sizeof( GLfloat );
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareNormal();
}

void ObjectGL::setObject(
   GLenum draw_mode,
   const std::vector<glm::vec3>& vertices,
   const std::vector<glm::vec2>& textures,
   const std::string& texture_file_path,
   bool is_grayscale
)
{
   DrawMode = draw_mode;
   VerticesCount = 0;
   DataBuffer.clear();
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( textures[i].x );
      DataBuffer.push_back( textures[i].y );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 5 * sizeof( GLfloat );
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareTexture( false );
   addTexture( texture_file_path, is_grayscale );
}

void ObjectGL::setObject(
   GLenum draw_mode,
   const std::vector<glm::vec3>& vertices,
   const std::vector<glm::vec3>& normals,
   const std::vector<glm::vec2>& textures
)
{
   DrawMode = draw_mode;
   VerticesCount = 0;
   DataBuffer.clear();
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      DataBuffer.push_back( textures[i].x );
      DataBuffer.push_back( textures[i].y );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 8 * sizeof( GLfloat );
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareNormal();
   prepareTexture( true );
}

void ObjectGL::setObject(
   GLenum draw_mode,
   const std::vector<glm::vec3>& vertices,
   const std::vector<glm::vec3>& normals,
   const std::vector<glm::vec2>& textures,
   const std::string& texture_file_path,
   bool is_grayscale
)
{
   setObject( draw_mode, vertices, normals, textures );
   addTexture( texture_file_path, is_grayscale );
}

void ObjectGL::setSquareObject(GLenum draw_mode, bool use_texture)
{
   std::vector<glm::vec3> square_vertices, square_normals;
   std::vector<glm::vec2> square_textures;
   getSquareObject( square_vertices, square_normals, square_textures );
   if (use_texture) setObject( draw_mode, square_vertices, square_normals, square_textures );
   else setObject( draw_mode, square_vertices, square_normals );
}

void ObjectGL::setSquareObject(
   GLenum draw_mode,
   const std::string& texture_file_path,
   bool is_grayscale
)
{
   std::vector<glm::vec3> square_vertices, square_normals;
   std::vector<glm::vec2> square_textures;
   getSquareObject( square_vertices, square_normals, square_textures );
   setObject( draw_mode, square_vertices, square_normals, square_textures, texture_file_path, is_grayscale );
}

void ObjectGL::calculateTangent(
   std::vector<glm::vec3>& tangents, 
   const std::vector<glm::vec3>& vertices, 
   const std::vector<glm::vec2>& textures
) const
{
   for (size_t i = 0; i < vertices.size(); i += 3) {
      const glm::vec3 edge1 = vertices[i + 1] - vertices[i];
      const glm::vec3 edge2 = vertices[i + 2] - vertices[i];
      const glm::vec2 delta_uv1 = textures[i + 1] - textures[i];
      const glm::vec2 delta_uv2 = textures[i + 2] - textures[i];
      const float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv1.y * delta_uv2.x);
      const glm::vec3 tangent = normalize( f * (delta_uv2.y * edge1 - delta_uv1.y * edge2) );
      tangents.emplace_back( tangent );
      tangents.emplace_back( tangent );
      tangents.emplace_back( tangent );
   }
}

void ObjectGL::calculateNormalMap(cv::Mat& normal_map, const std::string& texture_file_path) const
{
   const cv::Mat image = cv::imread( texture_file_path );
   cv::Mat gray_image;
   cv::cvtColor( image, gray_image, cv::COLOR_BGR2GRAY );

   cv::Mat blurred;
   cv::GaussianBlur( gray_image, blurred, cv::Size(5, 5), 1.0 );
   blurred.convertTo( blurred, CV_32FC1 );
   cv::flip( blurred, blurred, 0 );

   cv::Mat dx, dy;
   cv::Sobel( blurred, dx, CV_32FC1, 1, 0 );
   cv::Sobel( blurred, dy, CV_32FC1, 0, 1 );

   normal_map.create( image.size(), CV_32FC3 );
   for (int j = 0; j < normal_map.rows; ++j) {
      const auto* dx_ptr = dx.ptr<float>(j);
      const auto* dy_ptr = dy.ptr<float>(j);
      auto* normal_ptr = normal_map.ptr<cv::Vec3f>(j);
      for (int i = 0; i < normal_map.cols; ++i) {
         const glm::vec3 x(1.0f, 0.0f, dx_ptr[i] / 255.0f);
         const glm::vec3 y(0.0f, 1.0f, dy_ptr[i] / 255.0f);
         const glm::vec3 n = normalize( cross( x, y ) ) * 0.5f + 0.5f;
         normal_ptr[i] = cv::Vec3f(n.x, n.y, n.z);
      }
   }
}

void ObjectGL::setSquareObjectForNormalMap(GLenum draw_mode, const std::string& texture_file_path)
{
   std::vector<glm::vec3> square_vertices, square_normals;
   std::vector<glm::vec2> square_textures;
   getSquareObject( square_vertices, square_normals, square_textures );

   std::vector<glm::vec3> tangents;
   calculateTangent( tangents, square_vertices, square_textures );

   cv::Mat normal_map;
   calculateNormalMap( normal_map, texture_file_path );

   DrawMode = draw_mode;
   for (size_t i = 0; i < square_vertices.size(); ++i) {
      DataBuffer.push_back( square_vertices[i].x );
      DataBuffer.push_back( square_vertices[i].y );
      DataBuffer.push_back( square_vertices[i].z );
      DataBuffer.push_back( square_normals[i].x );
      DataBuffer.push_back( square_normals[i].y );
      DataBuffer.push_back( square_normals[i].z );
      DataBuffer.push_back( square_textures[i].x );
      DataBuffer.push_back( square_textures[i].y );
      DataBuffer.push_back( tangents[i].x );
      DataBuffer.push_back( tangents[i].y );
      DataBuffer.push_back( tangents[i].z );
      VerticesCount++;
   }
   const int n_bytes_per_vertex = 11 * sizeof(GLfloat);
   prepareVertexBuffer( n_bytes_per_vertex );
   prepareNormal();
   prepareTexture( true );
   addTexture( texture_file_path );
   prepareTangent();
   addTexture( reinterpret_cast<float*>(normal_map.data), normal_map.cols, normal_map.rows );
}

void ObjectGL::transferUniformsToShader(const ShaderGL* shader)
{
   glUniform4fv( shader->getMaterialEmissionLocation(), 1, &EmissionColor[0] );
   glUniform4fv( shader->getMaterialAmbientLocation(), 1, &AmbientReflectionColor[0] );
   glUniform4fv( shader->getMaterialDiffuseLocation(), 1, &DiffuseReflectionColor[0] );
   glUniform4fv( shader->getMaterialSpecularLocation(), 1, &SpecularReflectionColor[0] );
   glUniform1f( shader->getMaterialSpecularExponentLocation(), SpecularReflectionExponent );
}

void ObjectGL::updateDataBuffer(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals)
{
   assert( VBO != 0 );

   VerticesCount = 0;
   DataBuffer.clear();
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      VerticesCount++;
   }
   glNamedBufferSubData( VBO, 0, sizeof( GLfloat ) * DataBuffer.size(), DataBuffer.data() );
}

void ObjectGL::updateDataBuffer(
   const std::vector<glm::vec3>& vertices,
   const std::vector<glm::vec3>& normals,
   const std::vector<glm::vec2>& textures
)
{
   assert( VBO != 0 );

   VerticesCount = 0;
   DataBuffer.clear();
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer.push_back( vertices[i].x );
      DataBuffer.push_back( vertices[i].y );
      DataBuffer.push_back( vertices[i].z );
      DataBuffer.push_back( normals[i].x );
      DataBuffer.push_back( normals[i].y );
      DataBuffer.push_back( normals[i].z );
      DataBuffer.push_back( textures[i].x );
      DataBuffer.push_back( textures[i].y );
      VerticesCount++;
   }
   glNamedBufferSubData( VBO, 0, sizeof( GLfloat ) * DataBuffer.size(), DataBuffer.data() );
}

void ObjectGL::replaceVertices(
   const std::vector<glm::vec3>& vertices,
   bool normals_exist,
   bool textures_exist
)
{
   assert( VBO != 0 );

   VerticesCount = 0;
   int step = 3;
   if (normals_exist) step += 3;
   if (textures_exist) step += 2;
   for (size_t i = 0; i < vertices.size(); ++i) {
      DataBuffer[i * step] = vertices[i].x;
      DataBuffer[i * step + 1] = vertices[i].y;
      DataBuffer[i * step + 2] = vertices[i].z;
      VerticesCount++;
   }
   glNamedBufferSubData( VBO, 0, sizeof( GLfloat ) * VerticesCount * step, DataBuffer.data() );
}

void ObjectGL::replaceVertices(
   const std::vector<float>& vertices,
   bool normals_exist,
   bool textures_exist
)
{
   assert( VBO != 0 );

   VerticesCount = 0;
   int step = 3;
   if (normals_exist) step += 3;
   if (textures_exist) step += 2;
   for (size_t i = 0, j = 0; i < vertices.size(); i += 3, ++j) {
      DataBuffer[j * step] = vertices[i];
      DataBuffer[j * step + 1] = vertices[i + 1];
      DataBuffer[j * step + 2] = vertices[i + 2];
      VerticesCount++;
   }
   glNamedBufferSubData( VBO, 0, sizeof( GLfloat ) * VerticesCount * step, DataBuffer.data() );
}