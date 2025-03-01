include(CheckCSourceCompiles)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
check_c_source_compiles(
    "#pragma extern_absolute(\"_\", \"0\")"
    HAS_EXTERN_ABSOLUTE
)
if (NOT HAS_EXTERN_ABSOLUTE)
    message(FATAL_ERROR "pragma extern_absolute not supported. Try a newer version of MSVC.")
endif()

add_executable(patcher
    src/main.cpp
)

target_link_libraries(patcher
    LIEF::LIEF
)

function(add_patched ADD_PATCHED_TARGET)
    set(options)
    set(oneValueArgs ORIGINAL ORIGINAL_SHA256 PATCHED SOURCE)
    set(multiValueArgs SYMBOLS PATCHES)
    cmake_parse_arguments(PARSE_ARGV 1 ADD_PATCHED "${options}" "${oneValueArgs}" "${multiValueArgs}")
    if (NOT ADD_PATCHED_ORIGINAL)
        message(FATAL_ERROR "No original file specified")
    endif()
    if (NOT ADD_PATCHED_ORIGINAL_SHA256)
        message(FATAL_ERROR "No original SHA256 specified")
    endif()
    if (NOT ADD_PATCHED_PATCHED)
        message(FATAL_ERROR "No patched file specified")
    endif()
    if (NOT ADD_PATCHED_SOURCE)
        message(FATAL_ERROR "No source file specified")
    endif()
    if (ADD_PATCHED_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "Unparsed arguments: ${ADD_PATCHED_UNPARSED_ARGUMENTS}")
    endif()
    file(SHA256 ${ADD_PATCHED_ORIGINAL} ADD_PATCHED_ORIGINAL_CALCULATED_SHA256)
    if (NOT ADD_PATCHED_ORIGINAL_SHA256 STREQUAL ADD_PATCHED_ORIGINAL_CALCULATED_SHA256)
        message(FATAL_ERROR "Original file SHA256 mismatch")
    endif()
    execute_process(
        COMMAND dumpbin /HEADERS ${ADD_PATCHED_ORIGINAL}
        OUTPUT_VARIABLE ADD_PATCHED_ORIGINAL_HEADERS
        COMMAND_ERROR_IS_FATAL ANY
    )
    string(REGEX MATCH "image base \\(([0-9A-F]+) to ([0-9A-F]+)\\)" ADD_PATCHED_ORIGINAL_HEADERS_MATCH "${ADD_PATCHED_ORIGINAL_HEADERS}")
    if (NOT ADD_PATCHED_ORIGINAL_HEADERS_MATCH)
        message(FATAL_ERROR "Could not find image base in headers")
    endif()
    set(ADD_PATCHED_BASE 0x${CMAKE_MATCH_1})
    math(EXPR ADD_PATCHED_START "0x${CMAKE_MATCH_2} + 1" OUTPUT_FORMAT HEXADECIMAL)
    set(ADD_PATCHED_ALIGN "0x1000")
    math(EXPR WIDBERG_FILLER "${ADD_PATCHED_START} - ${ADD_PATCHED_BASE} - ${ADD_PATCHED_ALIGN}" OUTPUT_FORMAT HEXADECIMAL)    
    set(WIDBERG_SYMBOLS "")
    set(WIDBERG_SYMBOLS_NO_DECL "")
    foreach(SYMBOL ${ADD_PATCHED_SYMBOLS})
        list(LENGTH SYMBOL SYMBOL_LENGTH)
        if(NOT SYMBOL_LENGTH EQUAL 3 AND NOT SYMBOL_LENGTH EQUAL 4)
            message(FATAL_ERROR "Invalid symbol: ${SYMBOL}")
        endif()
        list(GET SYMBOL 0 SYMBOL_DNAME)
        list(GET SYMBOL 1 SYMBOL_DECL)
        list(GET SYMBOL 2 SYMBOL_ABS)
        set(SYMBOL_ADDR_TYPE "")
        if (SYMBOL_LENGTH EQUAL 4)
            list(GET SYMBOL 3 SYMBOL_ADDR_TYPE)
            if (NOT SYMBOL_ADDR_TYPE STREQUAL "ABS")
                message(FATAL_ERROR "Invalid symbol type: ${SYMBOL_ADDR_TYPE}")
            endif()
        endif()
        if (SYMBOL_ADDR_TYPE STREQUAL "ABS")
            set(SYMBOL_ADDR "${SYMBOL_ABS}")
        else()
            math(EXPR SYMBOL_ADDR "${SYMBOL_ABS} - ${ADD_PATCHED_BASE}" OUTPUT_FORMAT HEXADECIMAL)
        endif()
        set(WIDBERG_SYMBOLS "${WIDBERG_SYMBOLS}#pragma extern_absolute(\"${SYMBOL_DNAME}\", \"${SYMBOL_ADDR}\")\nextern ${SYMBOL_DECL};\n")
        set(WIDBERG_SYMBOLS_NO_DECL "${WIDBERG_SYMBOLS_NO_DECL}${SYMBOL_DNAME};${SYMBOL_ABS}\n")
    endforeach()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_symbols.txt "${ADD_PATCHED_BASE}\n${ADD_PATCHED_START}\n${WIDBERG_SYMBOLS_NO_DECL}")
    set(WIDBERG_PATCHES "")
    foreach(PATCH ${ADD_PATCHED_PATCHES})
        set(WIDBERG_PATCHES "${WIDBERG_PATCHES}${PATCH}\n")
    endforeach()
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_patches.txt "${WIDBERG_PATCHES}")
    add_library(${ADD_PATCHED_TARGET}_patch SHARED
        ${ADD_PATCHED_SOURCE}
    )
    target_compile_options(${ADD_PATCHED_TARGET}_patch PRIVATE /wd4414)
    target_link_options(${ADD_PATCHED_TARGET}_patch PRIVATE
    /BASE:${ADD_PATCHED_BASE}
    /FIXED
    /RELEASE
    /SECTION:.widberg,RWE,ALIGN=${ADD_PATCHED_ALIGN}
    /MERGE:.widberg.data=.widberg
    /MERGE:.widberg.rdata=.widberg
    /NODEFAULTLIB
    /NOENTRY
    /INCREMENTAL:NO
    /ALIGN:0x10
    /FILEALIGN:0x10
    /INCLUDE:___widberg_filler
    )
    configure_file(src/prologue.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/${ADD_PATCHED_TARGET}_prologue.h @ONLY)
    target_compile_options(${ADD_PATCHED_TARGET}_patch PRIVATE /FI${CMAKE_CURRENT_BINARY_DIR}/src/${ADD_PATCHED_TARGET}_prologue.h)
    add_custom_command(
        COMMAND $<TARGET_FILE:patcher> ${ADD_PATCHED_ORIGINAL} $<TARGET_FILE:${ADD_PATCHED_TARGET}_patch> ${ADD_PATCHED_PATCHED} ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_symbols.txt ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_patches.txt
        DEPENDS patcher ${ADD_PATCHED_TARGET}_patch ${ADD_PATCHED_ORIGINAL} ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_symbols.txt ${CMAKE_CURRENT_BINARY_DIR}/${ADD_PATCHED_TARGET}_patches.txt
        OUTPUT ${ADD_PATCHED_PATCHED}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    add_custom_target(${ADD_PATCHED_TARGET} ALL DEPENDS ${ADD_PATCHED_PATCHED})
endfunction()
