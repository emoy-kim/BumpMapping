target_link_libraries(OpenGL-Example glad glfw3dll)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
   target_link_libraries(OpenGL-Example FreeImaged opencv_cored opencv_imgprocd opencv_imgcodecsd)
else()
   target_link_libraries(OpenGL-Example FreeImage opencv_core opencv_imgproc opencv_imgcodecs)
endif()