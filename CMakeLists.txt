# This file is in large parts a reproduction of https://gist.github.com/kwk/9617978

cmake_minimum_required (VERSION 3.0)

project(interface_class_printer)
project(TypedefToUsing)

option(BUILD_TYPEDEF_TO_USING "Build the tool that converts a typedef to a using statement." ON)
mark_as_advanced(BUILD_TYPEDEF_TO_USING)
if (BUILD_TYPEDEF_TO_USING)
          
    # Find LLVM Config binary
    find_program(LLVM_CONFIG_PATH "llvm-config")    
    if(NOT LLVM_CONFIG_PATH)
       message(FATAL_ERROR "llvm-config not found -- ${LLVM_CONFIG_PATH}")
    endif()    
    message(STATUS "Found LLVM_CONFIG_PATH as ${LLVM_CONFIG_PATH}")
     
    # This function saves the output of the llvm-config command with the given
    # switch to the variable named VARNAME.
    #
    # Example usage: llvm_config(LLVM_CXXFLAGS "--cxxflags")
    function(llvm_config VARNAME switch)
        set(CONFIG_COMMAND "${LLVM_CONFIG_PATH}" "${switch}")
                
        execute_process(
            COMMAND ${CONFIG_COMMAND}
            RESULT_VARIABLE HAD_ERROR
            OUTPUT_VARIABLE CONFIG_OUTPUT
        )
            
        if (HAD_ERROR)
            string(REPLACE ";" " " CONFIG_COMMAND_STR "${CONFIG_COMMAND}")
            #message(STATUS "${CONFIG_COMMAND_STR}")
            message(FATAL_ERROR "llvm-config failed with status ${HAD_ERROR}")
        endif()
                
        # replace linebreaks with semicolon
        string(REGEX REPLACE
            "[ \t]*[\r\n]+[ \t]*" ";"
            CONFIG_OUTPUT ${CONFIG_OUTPUT})
                
        # make result available outside
        set(${VARNAME} ${CONFIG_OUTPUT} PARENT_SCOPE)
                
        # Optionally output the configured value
        #message(STATUS "llvm_config(${VARNAME})=>${CONFIG_OUTPUT}")
                
        # cleanup
        unset(CONFIG_COMMAND)
    endfunction(llvm_config)
            
    llvm_config(LLVM_CXXFLAGS "--cxxflags")
    llvm_config(LLVM_LDFLAGS "--ldflags")
    llvm_config(LLVM_LIBS "--libs")
    llvm_config(LLVM_LIBDIR "--libdir")
    llvm_config(LLVM_INCLUDE_DIR "--includedir")
    llvm_config(LLVM_SYSTEM_LIBS "--system-libs")
    llvm_config(LLVM_SRC_ROOT "--src-root")
    llvm_config(LLVM_OBJ_ROOT "--obj-root")
     
    add_definitions(${LLVM_CXXFLAGS})
    add_definitions(-fno-rtti)
    include_directories(${LLVM_INCLUDE_DIR})
    include_directories("${LLVM_OBJ_ROOT}/tools/clang/include")
    include_directories("${LLVM_SRC_ROOT}/tools/clang/include")
    link_directories(${LLVM_LIBDIR})
         
    add_executable(ttu
        main.cpp
        )
     
    # Libraries migh only be found when building with:
    # LD_LIBRARY_PATH=$(llvm-config --libdir):$LD_LIBRARY_PATH make
    target_link_libraries(ttu
        ${LLVM_LDFLAGS}
        #        -lLLVM-3.5
        -lclangTooling
        -lclangFrontendTool
        -lclangFrontend
        -lclangDriver
        -lclangSerialization
        -lclangParse
        -lclangSema
        -lclangAnalysis
        -lclangEdit
        -lclangAST
        -lclangLex
        -lclangBasic
        -lclangASTMatchers
        -lclangRewrite
        -lclangRewriteFrontend
        ${LLVM_LIBS}
        -pthread
        -ldl
        -ltinfo
   )
endif()

