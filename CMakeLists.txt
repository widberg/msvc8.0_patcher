cmake_minimum_required(VERSION 3.14)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "")
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF CACHE BOOL "")

project(msvc8.0_patcher VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(dependencies)

set(WS_VER_MAJOR "${PROJECT_VERSION_MAJOR}")
set(WS_VER_MINOR "${PROJECT_VERSION_MINOR}")
set(WS_VER_PATCH "${PROJECT_VERSION_PATCH}")
math(EXPR WS_VER "${WS_VER_MAJOR} * 10000 + ${WS_VER_MINOR} * 100 + ${WS_VER_PATCH} * 1")
configure_file(src/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h @ONLY)

include(cmake/add_patched.cmake)

add_patched(cl_patched
    SOURCE src/cl_patched.c
    ORIGINAL "${MSVC_ORIGINAL_BIN}/cl.exe"
    ORIGINAL_SHA256 "3cbf4306526c06a07d09fd4785ac290c7ac7b52355944e7b377e08f5d8e7ece7"
    PATCHED "${MSVC_PATCHED_BIN}/cl.exe"
    SYMBOLS
        "_PREP_START;void PREP_START(void);0x00405aad"
        "_PREP_END;void PREP_END(void);0x00405ab5"
        "_DEFINE_STUFF;void DEFINE_STUFF(void);0x004051c2"
        "_FIX_OVERFLOW;void FIX_OVERFLOW(void);0x00407151;ABS"
    PATCHES
        "NEAR_JMP;_PREP_START;prep"
        "BYTE;_FIX_OVERFLOW;0x7"
)
target_include_directories(cl_patched_patch PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

add_patched(c1_patched
    SOURCE src/c1_patched.c
    ORIGINAL "${MSVC_ORIGINAL_BIN}/c1.dll"
    ORIGINAL_SHA256 "be84b91b22bf67435525904e63db3c33afa6ea64dae9200e73db23439804254a"
    PATCHED "${MSVC_PATCHED_BIN}/c1.dll"
    SYMBOLS
        "_TOKEN_INPUT_STACK;void TOKEN_INPUT_STACK(void);0x10aba508;ABS"
        "_real_TOKEN_INPUT_STACK_PEAK_TOKEN;void *real_TOKEN_INPUT_STACK_PEAK_TOKEN(void);0x10a0f8e0"
        "@TOKEN_INPUT_STACK_GET_TOKEN@4;void *__fastcall TOKEN_INPUT_STACK_GET_TOKEN(void *tok_input_stack);0x10a0f6e0"
        "_PARSER_OPTYPE_START;void PARSER_OPTYPE_START(void);0x10a1e417"
        "_PARSER_OPTYPE_END;void PARSER_OPTYPE_END(void);0x10a1e41d"
        "_YYVAL;void YYVAL(void);0x10aa38a8;ABS"
        "_WRITE_EXTRA_START;void WRITE_EXTRA_START(void);0x10a38528"
        "_WRITE_EXTRA_END;void WRITE_EXTRA_END(void);0x10a38562"
        "_WRITE_ATTR2;void WRITE_ATTR2(void);0x10a377a0"
        "_LOWRITE;void LOWRITE(void);0x10a1c0d0"
        "_ADD_FUNC_START;void ADD_FUNC_START(void);0x10a43b46"
        "_ADD_FUNC_END;void ADD_FUNC_END(void);0x10a43b4c"
        "_SAVE_EXTRA_START;void SAVE_EXTRA_START(void);0x10a38083"
        "_SAVE_EXTRA_END;void SAVE_EXTRA_END(void);0x10a3808a"
        "_YYLEX;void YYLEX(void);0x10a037e0"
        "_YYLEX_RESUME;void YYLEX_RESUME(void);0x10A037E5"
        "_TOKSTRINGS_SIZE;void TOKSTRINGS_SIZE(void);0x10a76008;ABS"
        "_VAARGS_START;void VAARGS_START(void);0x10a43e10"
        "_VAARGS_GOOD_END;void VAARGS_GOOD_END(void);0x10a43e49"
        "_VAARGS_BAD_END;void VAARGS_BAD_END(void);0x10a43e44"
        "_VAARGS_UGLY_END;void VAARGS_UGLY_END(void);0x10a43e21"
        "___imp__malloc;void (*__imp__malloc)(unsigned);0x10A7031C;ABS"
        "_FASTCALL_TOKSTRINGS_SIZE;void TOKSTRINGS_SIZE(void);0x10a7633c;ABS"
    PATCHES
        "NEAR_JMP;_SAVE_EXTRA_START;save_extra"
        "NEAR_JMP;_PARSER_OPTYPE_START;parser_optype"
        "NEAR_JMP;_WRITE_EXTRA_START;write_extra"
        "NEAR_JMP;_ADD_FUNC_START;add_func"
        "NEAR_JMP;_VAARGS_START;vaargs"
        "NEAR_JMP;_YYLEX;wrap_yylex"
        "BYTE;_FASTCALL_TOKSTRINGS_SIZE;0x8"
)

add_patched(c1xx_patched
    SOURCE src/c1xx_patched.c
    ORIGINAL "${MSVC_ORIGINAL_BIN}/c1xx.dll"
    ORIGINAL_SHA256 "e4890058a1f928538b27246af1f5c6f95551fc0ad380639b1bf1d6b143b80d09"
    PATCHED "${MSVC_PATCHED_BIN}/c1xx.dll"
    SYMBOLS
        "_TOKEN_INPUT_STACK;void *TOKEN_INPUT_STACK;0x10740a58;ABS"
        "@TOKEN_INPUT_STACK_PEAK_TOKEN@4;void *__fastcall TOKEN_INPUT_STACK_PEAK_TOKEN(void *tok_input_stack);0x10575fd0"
        "@TOKEN_INPUT_STACK_GET_TOKEN@4;void *__fastcall TOKEN_INPUT_STACK_GET_TOKEN(void *tok_input_stack);0x105578c0"
        "_YYVAL;void *YYVAL;0x10724d40;ABS"
        "_LOWRITE;void LOWRITE(void);0x105a7080"
        "_YYLEX;void YYLEX(void);0x10556f10"
        "_YYLEX_RESUME;void YYLEX_RESUME(void);0x10556F16"
        "_TOKSTRINGS_SIZE;void TOKSTRINGS_SIZE(void);0x106c9948;ABS"
        "_PARSER_OPTYPE_START;void PARSER_OPTYPE_START(void);0x1054f4b2"
        "_PARSER_OPTYPE_END;void PARSER_OPTYPE_END(void);0x1054f4b9"
        "_ADD_FUNC_START;void ADD_FUNC_START(void);0x1057ff7f"
        "_ADD_FUNC_END;void ADD_FUNC_END(void);0x1057febe"
        "_SAVE_EXTRA_START;void SAVE_EXTRA_START(void);0x1055da9b"
        "_SAVE_EXTRA_END;void SAVE_EXTRA_END(void);0x1055daa1"
        "_WRITE_EXTRA_START1;void WRITE_EXTRA_START1(void);0x1055e39a"
        "_WRITE_EXTRA_START2;void WRITE_EXTRA_START2(void);0x1055fd87"
        "_WRITE_EXTRA_START3;void WRITE_EXTRA_START3(void);0x1055ffed"
        "_WRITE_EXTRA_START4;void WRITE_EXTRA_START4(void);0x105601ef"
        "_WRITE_EXTRA_END;void WRITE_EXTRA_END(void);0x1055dbbf"
        "_ADD_FUNC2_START;void ADD_FUNC2_START(void);0x1054fa04"
        "_ADD_FUNC2_END;void ADD_FUNC2_END(void);0x1054e87a"
        "_VAARGS_START;void VAARGS_START(void);0x10504136"
        "_VAARGS_GOOD_END;void VAARGS_GOOD_END(void);0x1050411b"
        "_VAARGS_BAD_END;void VAARGS_BAD_END(void);0x105d31d2"
        "_VAARGS_UGLY_END;void VAARGS_UGLY_END(void);0x105d31b1"
        "_VAARGS2_START;void VAARGS2_START(void);0x105db7f8"
        "_VAARGS2_GOOD_END;void VAARGS2_GOOD_END(void);0x10533f27"
        "_VAARGS2_BAD_END;void VAARGS2_BAD_END(void);0x10534192"
        "_VAARGS2_UGLY_END;void VAARGS2_UGLY_END(void);0x105db802"
        "_PREP_START;void PREP_START(void);0x1050f495"
        "_PREP_END;void PREP_END(void);0x1050f49a"
        "_INSTALL_DEFINE;void INSTALL_DEFINE(void);0x1057d7d0"
        "_COMMAND_DEF;void COMMAND_DEF(void);0x1050f890"
        "_DoMalloc@4;void *__stdcall DoMalloc(unsigned);0x105AB820"
        "_FASTCALL_TOKSTRINGS_SIZE;unsigned *FASTCALL_TOKSTRINGS_SIZE;0x106c9ccc;ABS"
        "_VALIDATE_CONSTRUCTOR_CC;void VALIDATE_CONSTRUCTOR_CC(void);0x1056D2B3"
        "_END_VALIDATE_CONSTRUCTOR_CC;void END_VALIDATE_CONSTRUCTOR_CC(void);0x1056CF63"
    PATCHES
        "NEAR_JMP;_SAVE_EXTRA_START;save_extra"
        "NEAR_JMP;_PARSER_OPTYPE_START;parser_optype"
        "NEAR_JMP;_WRITE_EXTRA_START1;write_extra"
        "NEAR_JMP;_WRITE_EXTRA_START2;write_extra"
        "NEAR_JMP;_WRITE_EXTRA_START3;write_extra"
        "NEAR_JMP;_WRITE_EXTRA_START4;write_extra"
        "NEAR_JMP;_ADD_FUNC_START;add_func"
        "NEAR_JMP;_ADD_FUNC2_START;add_func2"
        "NEAR_JMP;_VAARGS_START;vaargs"
        "NEAR_JMP;_VAARGS2_START;vaargs2"
        "NEAR_JMP;_YYLEX;wrap_yylex"
        "BYTE;_FASTCALL_TOKSTRINGS_SIZE;0x8"
        "NEAR_JMP;_VALIDATE_CONSTRUCTOR_CC;_END_VALIDATE_CONSTRUCTOR_CC"
)

add_patched(c2_patched
    SOURCE src/c2_patched.c
    ORIGINAL "${MSVC_ORIGINAL_BIN}/c2.dll"
    ORIGINAL_SHA256 "ebca6ef587d6e028b50cc79fe71dd6155230ecb51014295399e8f00e8865ff86"
    PATCHED "${MSVC_PATCHED_BIN}/c2.dll"
    SYMBOLS
        "_START_PTR;void START_PTR(void);0x10C513B1"
        "_END_PTR;void END_PTR(void);0x10C513C7"
        "_READ_EXTRA_START_PTR;void READ_EXTRA_START_PTR(void);0x10BB829C"
        "_READ_EXTRA_END;void READ_EXTRA_END(void);0x10BB82A3"
        "_LOREAD;void LOREAD(void);0x10B03350"
        "_LONG_TYPE_INDEX;unsigned LONG_TYPE_INDEX;0x10D04FFC;ABS"
        "_FASTCALL_REGS;unsigned FASTCALL_REGS[];0x10CDB930;ABS"
        "_CALLEE_CLEAN_START;void CALLEE_CLEAN_START(void);0x10C4E81C"
        "_CALLEE_CLEAN_END;void CALLEE_CLEAN_END(void);0x10C4E823"
        "_SCRATCH_REGS;void SCRATCH_REGS(void);0x10C3A945"
        "_SCRATCH_REGS_END;void SCRATCH_REGS_END(void);0x10C3A94E"
        "_TUP_FIND_DST_REG_INTERF;void TUP_FIND_DST_REG_INTERF(void);0x10BF1EB0"
        "_SCRATCH_REGS_SKIP;void SCRATCH_REGS_SKIP(void);0x10C3A96A"
    PATCHES
        "NEAR_JMP;_START_PTR;usercall_allo_reg"
        "NEAR_JMP;_READ_EXTRA_START_PTR;read_extra"
        "NEAR_JMP;_CALLEE_CLEAN_START;callee_clean"
        "NEAR_JMP;_SCRATCH_REGS;scratch_regs"
)
