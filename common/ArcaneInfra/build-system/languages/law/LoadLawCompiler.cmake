if(${VERBOSE})
  logStatus("    ** Generate LawCompiler.exe")
endif()
	
add_custom_command(
  OUTPUT  ${OutputPath}/LawCompiler.exe
  COMMAND ${XBUILD} 
  ARGS    ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/LawCompiler/LawCompiler.csproj ${XBUILD_ARGS} 
  DEPENDS ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/LawCompiler/LawCompiler.csproj
  )
   
bundle(
  BUNDLE ${PROJECT_BINARY_DIR}/bin/LawCompiler.exe 
  EXE    ${OutputPath}/LawCompiler.exe
  )

add_custom_command(
  OUTPUT  ${PROJECT_BINARY_DIR}/share/law.xsd
  COMMAND ${CMAKE_COMMAND} -E 
  copy_if_different ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/LawCompiler/Law.xsd ${PROJECT_BINARY_DIR}/share/law.xsd
  DEPENDS ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/LawCompiler/Law.xsd
  )

# generation de LawCompiler conditionnelle au debut
add_custom_target(
  law ALL DEPENDS 
  ${PROJECT_BINARY_DIR}/bin/LawCompiler.exe
  ${PROJECT_BINARY_DIR}/share/law.xsd
  )

# on cree une target pour pouvoir ecrire 
# /> make LawCompiler
add_custom_target(dotnet_LawCompiler 
  COMMAND ${XBUILD} ${ARCGEOSIM_BUILD_SYSTEM_PATH}/csharp/LawCompiler/LawCompiler.csproj ${XBUILD_ARGS}
  COMMENT "generate LawCompiler tools")

install(FILES ${PROJECT_BINARY_DIR}/bin/LawCompiler.exe DESTINATION bin)

# repertoire de sortie des law
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/share/law)

# installation de la xsd des fichiers law
install(FILES ${PROJECT_BINARY_DIR}/share/law.xsd DESTINATION share)

set(LAWCOMPILER ${PROJECT_BINARY_DIR}/bin/LawCompiler.exe)
