#include "Renderer.h"

RendererGL* RendererGL::Renderer = nullptr;
RendererGL::RendererGL() :
   Window( nullptr ), UseBumpMapping( true ), ClickedPoint{ -1, -1 }, 
   MainCamera( nullptr ), LightTheta( 0.0f ), WallObjects{ 9 }
{
   Renderer = this;

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   glfwTerminate();
}

void RendererGL::printOpenGLInformation() const
{
   cout << "****************************************************************" << endl;
   cout << " - GLFW version supported: " << glfwGetVersionString() << endl;
   cout << " - GLEW version supported: " << glewGetString( GLEW_VERSION ) << endl;
   cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << endl;
   cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << endl;
   cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl  ;
   cout << "****************************************************************" << endl << endl;
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      cout << "Cannot Initialize OpenGL..." << endl;
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   const int width = 1920;
   const int height = 1080;
   Window = glfwCreateWindow( width, height, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );
   glewInit();
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

   MainCamera = new CameraGL();
   MainCamera->updateWindowSize( width, height );
   ObjectShader.setShader(
      "Shaders/VertexShaderForBumpMapping.glsl",
      "Shaders/FragmentShaderForBumpMapping.glsl"
   );
}

void RendererGL::cleanup(GLFWwindow* window)
{
   glDeleteProgram( ObjectShader.ShaderProgram );
   for (auto& object : WallObjects) {
      glDeleteVertexArrays( 1, &object.ObjVAO );
      glDeleteBuffers( 1, &object.ObjVBO );
   }
   glfwSetWindowShouldClose( window, GLFW_TRUE );
}

void RendererGL::cleanupWrapper(GLFWwindow* window)
{
   Renderer->cleanup( window );
}

void RendererGL::keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   if (action != GLFW_PRESS) return;

   switch (key) {
      case GLFW_KEY_UP:
         MainCamera->moveForward();
         break;
      case GLFW_KEY_DOWN:
         MainCamera->moveBackward();
         break;
      case GLFW_KEY_LEFT:
         MainCamera->moveLeft();
         break;
      case GLFW_KEY_RIGHT:
         MainCamera->moveRight();
         break;
      case GLFW_KEY_W:
         MainCamera->moveUp();
         break;
      case GLFW_KEY_S:
         MainCamera->moveDown();
         break;
      case GLFW_KEY_I:
         MainCamera->resetCamera();
         break;
      case GLFW_KEY_L:
         Lights.toggleLightSwitch();
         cout << "Light Turned " << (Lights.isLightOn() ? "On!" : "Off!") << endl;
         break;
      case GLFW_KEY_B:
         UseBumpMapping = !UseBumpMapping;
         cout << "Bump Mapping Turned " << (UseBumpMapping ? "On!" : "Off!") << endl;
         break;
      case GLFW_KEY_Q:
      case GLFW_KEY_ESCAPE:
         cleanupWrapper( window );
         break;
      default:
         return;
   }
}

void RendererGL::keyboardWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   Renderer->keyboard( window, key, scancode, action, mods );
}

void RendererGL::cursor(GLFWwindow* window, double xpos, double ypos)
{
   if (MainCamera->getMovingState()) {
      const auto x = static_cast<float>(round( xpos ));
      const auto y = static_cast<float>(round( ypos ));
      const int dx = static_cast<int>(x - ClickedPoint.x);
      const int dy = static_cast<int>(y - ClickedPoint.y);
      MainCamera->moveForward( -dy );
      MainCamera->rotateAroundWorldY( -dx );

      if (glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS) {
         MainCamera->pitch( -dy );
      }

      ClickedPoint.x = x;
      ClickedPoint.y = y;
   }
}

void RendererGL::cursorWrapper(GLFWwindow* window, double xpos, double ypos)
{
   Renderer->cursor( window, xpos, ypos );
}

void RendererGL::mouse(GLFWwindow* window, int button, int action, int mods)
{
   if (button == GLFW_MOUSE_BUTTON_LEFT) {
      const bool moving_state = action == GLFW_PRESS;
      if (moving_state) {
         double x, y;
         glfwGetCursorPos( window, &x, &y );
         ClickedPoint.x = static_cast<float>(round( x ));
         ClickedPoint.y = static_cast<float>(round( y ));
      }

      MainCamera->setMovingState( moving_state );
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset)
{
   if (yoffset >= 0.0) MainCamera->zoomIn();
   else MainCamera->zoomOut();
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height)
{
   MainCamera->updateWindowSize( width, height );
   glViewport( 0, 0, width, height );
}

void RendererGL::reshapeWrapper(GLFWwindow* window, int width, int height)
{
   Renderer->reshape( window, width, height );
}

void RendererGL::registerCallbacks() const
{
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setLights()
{  
   vec4 light_position(0.5f, 0.5f, 0.2f, 1.0f);
   vec4 ambient_color(0.9f, 0.9f, 0.9f, 1.0f);
   vec4 diffuse_color(0.9f, 0.9f, 0.9f, 1.0f);
   vec4 specular_color(0.9f, 0.9f, 0.9f, 1.0f);
   Lights.addLight( light_position, ambient_color, diffuse_color, specular_color );

   light_position = vec4(1.5f, 1.5f, 15.0f, 1.0f);
   ambient_color = vec4(0.2f, 0.2f, 0.2f, 1.0f);
   diffuse_color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
   specular_color = vec4(0.8f, 0.8f, 0.8f, 1.0f);
   vec3 spotlight_direction(0.0f, 0.0f, -1.0f);
   float spotlight_exponent = 128;
   float spotlight_cutoff_angle_in_degree = 10.0f;
   Lights.addLight( 
      light_position, 
      ambient_color, 
      diffuse_color, 
      specular_color,
      spotlight_direction,
      spotlight_exponent,
      spotlight_cutoff_angle_in_degree
   );  
}

void RendererGL::setWallObject(const uint& object_index)
{
   vector<vec3> square_vertices;
   square_vertices.emplace_back( 0.0f, 1.0f, 0.0f );
   square_vertices.emplace_back( 0.0f, 0.0f, 0.0f );
   square_vertices.emplace_back( 1.0f, 0.0f, 0.0f );
   
   square_vertices.emplace_back( 0.0f, 1.0f, 0.0f );
   square_vertices.emplace_back( 1.0f, 0.0f, 0.0f );
   square_vertices.emplace_back( 1.0f, 1.0f, 0.0f );

   vector<vec3> square_normals;
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );
   
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );
   square_normals.emplace_back( 0.0f, 0.0f, 1.0f );

   vector<vec2> square_textures;
   square_textures.emplace_back( 0.0f, 1.0f );
   square_textures.emplace_back( 0.0f, 0.0f );
   square_textures.emplace_back( 1.0f, 0.0f );
   
   square_textures.emplace_back( 0.0f, 1.0f );
   square_textures.emplace_back( 1.0f, 0.0f );
   square_textures.emplace_back( 1.0f, 1.0f );

   const string texture_path = "Samples/" + to_string( object_index ) + ".jpg";
   WallObjects[object_index].setObject( GL_TRIANGLES, square_vertices, square_normals, square_textures, texture_path );
   WallObjects[object_index].setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void RendererGL::drawWallObject(const mat4& to_world, const uint& object_index)
{
   glUseProgram( ObjectShader.ShaderProgram );

   const mat4 model_view_projection = MainCamera->ProjectionMatrix * MainCamera->ViewMatrix * to_world;
   glUniformMatrix4fv( ObjectShader.Location.World, 1, GL_FALSE, &to_world[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.View, 1, GL_FALSE, &MainCamera->ViewMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.Projection, 1, GL_FALSE, &MainCamera->ProjectionMatrix[0][0] );
   glUniformMatrix4fv( ObjectShader.Location.ModelViewProjection, 1, GL_FALSE, &model_view_projection[0][0] );
   glUniform1i( ObjectShader.CustomLocations["UseBumpMapping"], UseBumpMapping ? 1 : 0 );

   Lights.transferUniformsToShader( ObjectShader );
   WallObjects[object_index].transferUniformsToShader( ObjectShader );

   glBindTextureUnit( ObjectShader.Location.Texture[0].first, WallObjects[object_index].TextureID[0] );
   glBindTextureUnit( ObjectShader.Location.Texture[1].first, WallObjects[object_index].TextureID[1] );
   glBindVertexArray( WallObjects[object_index].ObjVAO );
   glDrawArrays( WallObjects[object_index].DrawMode, 0, WallObjects[object_index].VerticesCount );
}

void RendererGL::render()
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   const float light_x = 1.25f * cos( LightTheta ) + 1.5f;
   const float light_y = 1.25f * sin( LightTheta ) + 1.5f;
   Lights.setLightPosition( vec4(light_x, light_y, 0.2f, 1.0f), 0 );

   const mat4 to_up = translate( mat4(1.0f), vec3(0.0f, 1.0f, 0.0f));
   mat4 to_world_3rd_row = mat4(1.0f);
   mat4 to_world_2nd_row = to_up;
   mat4 to_world_1st_row = to_world_2nd_row * to_up;

   drawWallObject( to_world_3rd_row, 0 );
   drawWallObject( to_world_2nd_row, 1 );
   drawWallObject( to_world_1st_row, 2 );

   const mat4 to_right = translate( mat4(1.0f), vec3(1.0f, 0.0f, 0.0f));
   to_world_3rd_row *= to_right;
   to_world_2nd_row *= to_right;
   to_world_1st_row *= to_right;

   drawWallObject( to_world_3rd_row, 3 );
   drawWallObject( to_world_2nd_row, 4 );
   drawWallObject( to_world_1st_row, 5 );

   to_world_3rd_row *= to_right;
   to_world_2nd_row *= to_right;
   to_world_1st_row *= to_right;

   drawWallObject( to_world_3rd_row, 6 );
   drawWallObject( to_world_2nd_row, 7 );
   drawWallObject( to_world_1st_row, 8 );

   glBindVertexArray( 0 );
   glUseProgram( 0 );
}

void RendererGL::play()
{
   if (glfwWindowShouldClose( Window )) initialize();

   setLights();
   for (uint i = 0; i < 9; ++i) setWallObject( i );
   ObjectShader.setUniformLocations( Lights.TotalLightNum );
   ObjectShader.addUniformLocation( ObjectShader.ShaderProgram, "UseBumpMapping" );

   while (!glfwWindowShouldClose( Window )) {
      render();

      LightTheta += 0.05f;
      if (LightTheta >= 360.0f) LightTheta -= 360.0f;
      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}