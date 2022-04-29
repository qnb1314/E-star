set( HOMEWORK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/homework")

# 在此处添加文件
file(GLOB_RECURSE HOMEWORK_SOURCES CONFIGURE_DEPENDS ${HOMEWORK_DIR}/Source/*.*
GLOB_RECURSE HOMEWORK_SOURCES CONFIGURE_DEPENDS ${HOMEWORK_DIR}/shaders/*.*)

#添加Eigen库
include_directories("${HOMEWORK_DIR}/shared/eigen")


add_executable( homework ${HOMEWORK_SOURCES} )
target_link_libraries( homework example-common )
target_include_directories( homework PRIVATE ${HOMEWORK_DIR} )


#Copy shaders&meshes
add_custom_command(TARGET homework
  COMMAND ${CMAKE_COMMAND} -E make_directory "shaders" 
  COMMAND ${CMAKE_COMMAND} -E copy_directory  "${HOMEWORK_DIR}/CopyFile/shaders" "shaders" 

  COMMAND ${CMAKE_COMMAND} -E make_directory "meshes" 
  COMMAND ${CMAKE_COMMAND} -E copy_directory  "${HOMEWORK_DIR}/CopyFile/meshes" "meshes" 

  COMMAND ${CMAKE_COMMAND} -E make_directory "textures" 
  COMMAND ${CMAKE_COMMAND} -E copy_directory  "${HOMEWORK_DIR}/CopyFile/textures" "textures" 
)

# Special Visual Studio Flags
if( MSVC )
	target_compile_definitions( homework PRIVATE "_CRT_SECURE_NO_WARNINGS" )
endif()
