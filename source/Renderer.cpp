#include "Renderer.h"

RendererGL::RendererGL() : 
   Window( nullptr ), FrameWidth( 1920 ), FrameHeight( 1080 ), UseBumpMapping( true ), LightTheta( 0.0f ),
   ClickedPoint( -1, -1 ), MainCamera( std::make_unique<CameraGL>() ),
   ObjectShader( std::make_unique<ShaderGL>() ), Lights( std::make_unique<LightGL>() )
{
   Renderer = this;

   WallObjects.resize( 9 );
   for (auto& wall : WallObjects) {
      wall = std::make_unique<ObjectGL>();
   }

   initialize();
   printOpenGLInformation();
}

RendererGL::~RendererGL()
{
   glfwTerminate();
}

void RendererGL::printOpenGLInformation() const
{
   std::cout << "****************************************************************\n";
   std::cout << " - GLFW version supported: " << glfwGetVersionString() << "\n";
   std::cout << " - OpenGL renderer: " << glGetString( GL_RENDERER ) << "\n";
   std::cout << " - OpenGL version supported: " << glGetString( GL_VERSION ) << "\n";
   std::cout << " - OpenGL shader version supported: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << "\n";
   std::cout << "****************************************************************\n\n";
}

void RendererGL::initialize()
{
   if (!glfwInit()) {
      std::cout << "Cannot Initialize OpenGL...\n";
      return;
   }
   glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
   glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
   glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );
   glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

   Window = glfwCreateWindow( FrameWidth, FrameHeight, "Main Camera", nullptr, nullptr );
   glfwMakeContextCurrent( Window );

   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return;
   }
   
   registerCallbacks();
   
   glEnable( GL_DEPTH_TEST );
   glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

   MainCamera->updateWindowSize( FrameWidth, FrameHeight );

   const std::string shader_directory_path = std::string(CMAKE_SOURCE_DIR) + "/shaders";
   ObjectShader->setShader(
      std::string(shader_directory_path + "/BumpMapping.vert").c_str(),
      std::string(shader_directory_path + "/BumpMapping.frag").c_str()
   );
}

void RendererGL::error(int error, const char* description) const
{
   puts( description );
}

void RendererGL::errorWrapper(int error, const char* description)
{
   Renderer->error( error, description );
}

void RendererGL::cleanup(GLFWwindow* window)
{
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
         Lights->toggleLightSwitch();
         std::cout << "Light Turned " << (Lights->isLightOn() ? "On!\n" : "Off!\n");
         break;
      case GLFW_KEY_B:
         UseBumpMapping = !UseBumpMapping;
         std::cout << "Bump Mapping Turned " << (UseBumpMapping ? "On!\n" : "Off!\n");
         break;
      case GLFW_KEY_P: {
         const glm::vec3 pos = MainCamera->getCameraPosition();
         std::cout << "Camera Position: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
      } break;
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
      const auto x = static_cast<int>(round( xpos ));
      const auto y = static_cast<int>(round( ypos ));
      const int dx = x - ClickedPoint.x;
      const int dy = y - ClickedPoint.y;
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
         ClickedPoint.x = static_cast<int>(round( x ));
         ClickedPoint.y = static_cast<int>(round( y ));
      }
      MainCamera->setMovingState( moving_state );
   }
}

void RendererGL::mouseWrapper(GLFWwindow* window, int button, int action, int mods)
{
   Renderer->mouse( window, button, action, mods );
}

void RendererGL::mousewheel(GLFWwindow* window, double xoffset, double yoffset) const
{
   if (yoffset >= 0.0) MainCamera->zoomIn();
   else MainCamera->zoomOut();
}

void RendererGL::mousewheelWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
   Renderer->mousewheel( window, xoffset, yoffset );
}

void RendererGL::reshape(GLFWwindow* window, int width, int height) const
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
   glfwSetErrorCallback( errorWrapper );
   glfwSetWindowCloseCallback( Window, cleanupWrapper );
   glfwSetKeyCallback( Window, keyboardWrapper );
   glfwSetCursorPosCallback( Window, cursorWrapper );
   glfwSetMouseButtonCallback( Window, mouseWrapper );
   glfwSetScrollCallback( Window, mousewheelWrapper );
   glfwSetFramebufferSizeCallback( Window, reshapeWrapper );
}

void RendererGL::setLights() const
{  
   glm::vec4 light_position(0.5f, 0.5f, 0.2f, 1.0f);
   glm::vec4 ambient_color(0.9f, 0.9f, 0.9f, 1.0f);
   glm::vec4 diffuse_color(0.9f, 0.9f, 0.9f, 1.0f);
   glm::vec4 specular_color(0.9f, 0.9f, 0.9f, 1.0f);
   Lights->addLight( light_position, ambient_color, diffuse_color, specular_color );

   light_position = glm::vec4(1.5f, 1.5f, 15.0f, 1.0f);
   ambient_color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
   diffuse_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
   specular_color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
   glm::vec3 spotlight_direction(0.0f, 0.0f, -1.0f);
   float spotlight_exponent = 128;
   float spotlight_cutoff_angle_in_degree = 10.0f;
   Lights->addLight( 
      light_position, 
      ambient_color, 
      diffuse_color, 
      specular_color,
      spotlight_direction,
      spotlight_exponent,
      spotlight_cutoff_angle_in_degree
   );  
}

void RendererGL::setWallObject(int object_index)
{
   const std::string sample_directory_path = std::string(CMAKE_SOURCE_DIR) + "/samples/";
   const std::string texture_path = sample_directory_path + std::to_string( object_index ) + ".jpg";
   WallObjects[object_index]->setSquareObjectForNormalMap( GL_TRIANGLES, texture_path );
   WallObjects[object_index]->setDiffuseReflectionColor( { 1.0f, 1.0f, 1.0f, 1.0f } );
}

void RendererGL::drawWallObject(const glm::mat4& to_world, int object_index)
{
   glUseProgram( ObjectShader->getShaderProgram() );

   ObjectShader->transferBasicTransformationUniforms( to_world, MainCamera.get(), true );
   glUniform1i( ObjectShader->getLocation( "UseBumpMapping" ), UseBumpMapping ? 1 : 0 );

   WallObjects[object_index]->transferUniformsToShader( ObjectShader.get() );
   Lights->transferUniformsToShader( ObjectShader.get() );

   glBindTextureUnit( 0, WallObjects[object_index]->getTextureID( 0 ) );
   glBindTextureUnit( 1, WallObjects[object_index]->getTextureID( 1 ) );
   glBindVertexArray( WallObjects[object_index]->getVAO() );
   glDrawArrays( WallObjects[object_index]->getDrawMode(), 0, WallObjects[object_index]->getVertexNum() );
}

void RendererGL::render()
{
   glClear( OPENGL_COLOR_BUFFER_BIT | OPENGL_DEPTH_BUFFER_BIT );

   const float light_x = 1.25f * cosf( LightTheta ) + 1.5f;
   const float light_y = 1.25f * sinf( LightTheta ) + 1.5f;
   Lights->setLightPosition( glm::vec4(light_x, light_y, 0.2f, 1.0f), 0 );

   const glm::mat4 to_up = translate( glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
   glm::mat4 to_world_3rd_row = glm::mat4(1.0f);
   glm::mat4 to_world_2nd_row = to_up;
   glm::mat4 to_world_1st_row = to_world_2nd_row * to_up;

   drawWallObject( to_world_3rd_row, 0 );
   drawWallObject( to_world_2nd_row, 1 );
   drawWallObject( to_world_1st_row, 2 );

   const glm::mat4 to_right = translate(glm:: mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f) );
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
   for (int i = 0; i < 9; ++i) setWallObject( i );
   ObjectShader->setUniformLocations( Lights->getTotalLightNum() );
   ObjectShader->addUniformLocation( "UseBumpMapping" );

   while (!glfwWindowShouldClose( Window )) {
      render();

      LightTheta += 0.05f;
      if (LightTheta >= 360.0f) LightTheta -= 360.0f;
      glfwSwapBuffers( Window );
      glfwPollEvents();
   }
   glfwDestroyWindow( Window );
}