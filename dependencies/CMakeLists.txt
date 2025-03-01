# LIEF
set(LIEF_C_API       OFF CACHE BOOL "")
set(LIEF_EXAMPLES    OFF CACHE BOOL "")
set(LIEF_USE_CCACHE  OFF CACHE BOOL "")
set(LIEF_ENABLE_JSON OFF CACHE BOOL "")
# set(LIEF_ELF         OFF CACHE BOOL "") # Disabling elf breaks the build
set(LIEF_MACHO       OFF CACHE BOOL "")
set(LIEF_DEX         OFF CACHE BOOL "")
set(LIEF_ART         OFF CACHE BOOL "")
add_subdirectory(LIEF)
