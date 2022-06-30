if(${VERBOSE})
  logStatus("    ** Generate GumpCompiler.exe")
endif()
	
add_custom_command(
  OUTPUT  ${OutputPath}/GumpCompiler.exe
  COMMAND ${XBUILD} 
  ARGS    ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/GumpCompiler/GumpCompiler.csproj ${XBUILD_ARGS} 
  DEPENDS ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/GumpCompiler/GumpCompiler.csproj
  )
   
bundle(
  BUNDLE ${PROJECT_BINARY_DIR}/bin/GumpCompiler.exe 
  EXE    ${OutputPath}/GumpCompiler.exe
  )

add_custom_command(
  OUTPUT  ${PROJECT_BINARY_DIR}/share/gump.xsd
  COMMAND ${CMAKE_COMMAND} -E 
  copy_if_different ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/GumpCompiler/Gump.xsd ${PROJECT_BINARY_DIR}/share/gump.xsd
  DEPENDS ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/GumpCompiler/Gump.xsd
  )

# génération de GumpCompiler conditionnelle au début
add_custom_target(
  gump ALL DEPENDS 
  ${PROJECT_BINARY_DIR}/bin/GumpCompiler.exe
  ${PROJECT_BINARY_DIR}/share/gump.xsd
  )

# on crée une target pour pouvoir écrire 
# /> make GumpCompiler
add_custom_target(dotnet_GumpCompiler 
  COMMAND ${XBUILD} ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/GumpCompiler/GumpCompiler.csproj ${XBUILD_ARGS}
  COMMENT "generate GumpCompiler tools")

install(FILES ${PROJECT_BINARY_DIR}/bin/GumpCompiler.exe DESTINATION bin)

# répertoire de sortie des gump
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/share/gump)

# installation de la xsd des fichiers gump
install(FILES ${PROJECT_BINARY_DIR}/share/gump.xsd DESTINATION share)

set(GUMPCOMPILER ${PROJECT_BINARY_DIR}/bin/GumpCompiler.exe)
