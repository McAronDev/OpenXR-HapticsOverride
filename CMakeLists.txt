# CMakeList.txt : CMake project for ${PROJECT_NAME}, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.22)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("OpenXR-HapticsOverride" VERSION 1.0)


include(FetchContent)

FetchContent_Declare(
    OpenXR-SDK
    URL https://github.com/KhronosGroup/OpenXR-SDK/archive/refs/tags/release-1.1.46.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(OpenXR-SDK)

set(VENDOR "McAronDev")

string(TOUPPER "${VENDOR}" VENDOR_UPPER)


set(LAYER_NAME "XR_APILAYER_${VENDOR_UPPER}_haptics_override")

string(TOUPPER "DISABLE_${LAYER_NAME}" DISABLE_LAYER_ENV)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/layer.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/layer.json
    @ONLY
)



# Add source to this project's output lib.
add_library (${PROJECT_NAME} SHARED "src/openxr_api_layer/entry.cpp" "src/openxr_api_layer/utils/log.cpp" "src/openxr_api_layer/utils/log.h" "src/openxr_api_layer/layer.cpp" "src/openxr_api_layer/layer.h") 

target_link_libraries(${PROJECT_NAME} PRIVATE openxr_loader)


#pass variables to sources
target_compile_definitions(
    ${PROJECT_NAME} PRIVATE 
    PROJECT_NAME="${PROJECT_NAME}"
    VENDOR="${VENDOR}"
    LAYER_NAME="${LAYER_NAME}"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${openxr-sdk_SOURCE_DIR}/include)

file(DOWNLOAD https://www.gnu.org/licenses/gpl-3.0.txt "${CMAKE_CURRENT_BINARY_DIR}/license.txt")

if (CMAKE_VERSION VERSION_GREATER 3.19)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION / COMPONENT layer)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/layer.json DESTINATION / COMPONENT layer)

#again just to create field in Choose Components to install list
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/layer.json DESTINATION / COMPONENT debug)
set (CPACK_NSIS_debug_INSTALL_DIRECTORY)

# Use CPack for creating NSIS exe installer


set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "McAronDev")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}") 
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} Installer")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_BINARY_DIR}/license.txt)
set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/McAronDev/${PROJECT_NAME}")

# Add uninstall symlink to start menu
set(CPACK_NSIS_MENU_LINKS "Uninstall.exe" "Uninstall ${PROJECT_NAME}")

set(CPACK_COMPONENT_layer_DESCRIPTION "Installing ${PROJECT_NAME}")

set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

set(CPACK_VERBATIM_VARIABLES TRUE)


set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS "
    !include x64.nsh

    \${IfNot} \${RunningX64}
      MessageBox MB_OK 'This installer is for 64-bit Windows!'
      Quit
    \${EndIf}

")

set(REG_PATH "SOFTWARE\\${VENDOR}\\${PROJECT_NAME}")


set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "

    SetRegView 64 
    
    \${If} \${SectionIsSelected} \${debug} 
        WriteRegDWORD HKLM '${REG_PATH}' 'enableDebug' 1
    \${Else}
        WriteRegDWORD HKLM '${REG_PATH}' 'enableDebug' 0
    \${EndIf}

    WriteRegDWORD HKLM 'SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit' '$INSTDIR\\layer.json' 0

    SetRegView lastused
")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    SetRegView 64

    RMDir /r '$LocalAppdata\\${PROJECT_NAME}'

    DeleteRegValue HKLM 'SOFTWARE\\Khronos\\OpenXR\\1\\ApiLayers\\Implicit' '$INSTDIR\\layer.json'
    DeleteRegKey HKLM '${REG_PATH}'
    DeleteRegKey /ifempty HKLM 'SOFTWARE\\${VENDOR}'

    SetRegView lastused
")

include(CPack)

cpack_add_component(
    layer 
    DISPLAY_NAME
    "${PROJECT_NAME}"
    DESCRIPTION "Installing ${PROJECT_NAME}" REQUIRED
)

cpack_add_component(
    debug 
    DISPLAY_NAME "Enable debug output"
    DESCRIPTION "Will write debug output to %localappdata%"
    DISABLED
)


 
