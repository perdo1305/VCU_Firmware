# The following variables contains the files used by the different stages of the build process.
set(VCU_firmware_default_default_XC32_FILE_TYPE_assemble)
set(VCU_firmware_default_default_XC32_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${VCU_firmware_default_default_XC32_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE C)
set(VCU_firmware_default_default_XC32_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Documents/git/CANSART/Controller Library/PIC32/Library/cansart.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../Documents/git/CANSART/Controller Library/PIC32/Library/cansartP.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/VCU2.0.X/TorqueControl.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../firmware/src/config/default/peripheral/mcpwm/plib_mcpwm.c")
set(VCU_firmware_default_default_XC32_FILE_TYPE_compile_cpp)
set(VCU_firmware_default_default_XC32_FILE_TYPE_link)

# The (internal) path to the resulting build image.
set(VCU_firmware_default_internal_image_name "${CMAKE_CURRENT_SOURCE_DIR}/../../../_build/VCU_firmware/default/default.elf")

# The name of the resulting image, including namespace for configuration.
set(VCU_firmware_default_image_name "VCU_firmware_default_default.elf")

# The name of the image, excluding the namespace for configuration.
set(VCU_firmware_default_original_image_name "default.elf")

# The output directory of the final image.
set(VCU_firmware_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/VCU_firmware")
