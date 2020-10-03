# Compilation step for static resources
FUNCTION(ADD_RESOURCES out_var path file)
    SET(result)
    SET(result "${PROJECT_BINARY_DIR}/${file}.o")
    ADD_CUSTOM_COMMAND(OUTPUT ${result}
      COMMAND ld -m elf_i386 -L"${path}" -r -b binary -o ${result} ${file}
      DEPENDS ${path}/${file}
      WORKING_DIRECTORY ${path}
      COMMENT "Building binary data object ${result}"
      VERBATIM)
    SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()
