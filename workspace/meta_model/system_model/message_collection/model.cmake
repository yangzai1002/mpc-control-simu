# 防止重复 include
include_guard(GLOBAL)

# 1. 本模块需要的源文件
set(MODULE1_SOURCES
    
    CACHE STRING "module1 source files" FORCE)

# 2. 本模块需要的头文件搜索路径（可选）
set(MODULE1_INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/adsfi_interface/
    ${CMAKE_CURRENT_LIST_DIR}
    
    CACHE PATH "module1 include dirs" FORCE)

# 3. 如果还需要链接第三方库
set(MODULE1_LIBS
    pthread
    dl
    rt
    rtf_com
    rtf_cm
    rtf_maintaind
    rtftools
    vrtf_vcc
    securec
    logging
    ara_core
    CACHE STRING "module1 link libraries" FORCE)
