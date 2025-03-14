include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(VCU_firmware_default_library_list )

# Handle files with suffix s, for group default-XC32
if(VCU_firmware_default_default_XC32_FILE_TYPE_assemble)
add_library(VCU_firmware_default_default_XC32_assemble OBJECT ${VCU_firmware_default_default_XC32_FILE_TYPE_assemble})
    VCU_firmware_default_default_XC32_assemble_rule(VCU_firmware_default_default_XC32_assemble)
    list(APPEND VCU_firmware_default_library_list "$<TARGET_OBJECTS:VCU_firmware_default_default_XC32_assemble>")
endif()

# Handle files with suffix S, for group default-XC32
if(VCU_firmware_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
add_library(VCU_firmware_default_default_XC32_assembleWithPreprocess OBJECT ${VCU_firmware_default_default_XC32_FILE_TYPE_assembleWithPreprocess})
    VCU_firmware_default_default_XC32_assembleWithPreprocess_rule(VCU_firmware_default_default_XC32_assembleWithPreprocess)
    list(APPEND VCU_firmware_default_library_list "$<TARGET_OBJECTS:VCU_firmware_default_default_XC32_assembleWithPreprocess>")
endif()

# Handle files with suffix [cC], for group default-XC32
if(VCU_firmware_default_default_XC32_FILE_TYPE_compile)
add_library(VCU_firmware_default_default_XC32_compile OBJECT ${VCU_firmware_default_default_XC32_FILE_TYPE_compile})
    VCU_firmware_default_default_XC32_compile_rule(VCU_firmware_default_default_XC32_compile)
    list(APPEND VCU_firmware_default_library_list "$<TARGET_OBJECTS:VCU_firmware_default_default_XC32_compile>")
endif()

# Handle files with suffix cpp, for group default-XC32
if(VCU_firmware_default_default_XC32_FILE_TYPE_compile_cpp)
add_library(VCU_firmware_default_default_XC32_compile_cpp OBJECT ${VCU_firmware_default_default_XC32_FILE_TYPE_compile_cpp})
    VCU_firmware_default_default_XC32_compile_cpp_rule(VCU_firmware_default_default_XC32_compile_cpp)
    list(APPEND VCU_firmware_default_library_list "$<TARGET_OBJECTS:VCU_firmware_default_default_XC32_compile_cpp>")
endif()

add_executable(${VCU_firmware_default_image_name} ${VCU_firmware_default_library_list})

target_link_libraries(${VCU_firmware_default_image_name} PRIVATE ${VCU_firmware_default_default_XC32_FILE_TYPE_link})

# Add the link options from the rule file.
VCU_firmware_default_link_rule(${VCU_firmware_default_image_name})

# Add bin2hex target for converting built file to a .hex file.
add_custom_target(VCU_firmware_default_Bin2Hex ALL
    ${MP_BIN2HEX} ${VCU_firmware_default_image_name})
add_dependencies(VCU_firmware_default_Bin2Hex ${VCU_firmware_default_image_name})

# Post build target to copy built file to the output directory.
add_custom_command(TARGET ${VCU_firmware_default_image_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${VCU_firmware_default_output_dir}
                    COMMAND ${CMAKE_COMMAND} -E copy ${VCU_firmware_default_image_name} ${VCU_firmware_default_output_dir}/${VCU_firmware_default_original_image_name}
                    BYPRODUCTS ${VCU_firmware_default_output_dir}/${VCU_firmware_default_original_image_name})
