# ================================
# C++ 静态检查配置
# ================================
# 功能：对项目源代码进行静态分析，排除第三方库
# 使用：make run_cppcheck 或 make all_cppcheck_reports
# ================================

# 生成编译数据库
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============ 查找工具 ============
find_program(CPPCHECK_EXECUTABLE cppcheck)
if(NOT CPPCHECK_EXECUTABLE)
    message(FATAL_ERROR "Cppcheck executable not found. Please install Cppcheck or set its path.")
endif()

find_package(Python3 COMPONENTS Interpreter)
if(NOT Python3_FOUND)
    message(WARNING "Python3 not found. Will use unfiltered compile_commands.json")
endif()

find_program(XSLTPROC_EXECUTABLE xsltproc)
if(NOT XSLTPROC_EXECUTABLE)
    message(FATAL_ERROR "xsltproc executable not found. Please install xsltproc.")
endif()

# ============ 文件路径配置 ============
set(FILTER_SCRIPT ${CMAKE_SOURCE_DIR}/cmake/modules/filter_compile_commands.py)
set(FILTERED_COMPILE_COMMANDS ${CMAKE_BINARY_DIR}/compile_commands_filtered.json)
set(CPPCHECK_SUPPRESSIONS ${CMAKE_BINARY_DIR}/cppcheck_suppressions.txt)
set(CPPCHECK_XML_REPORT ${CMAKE_BINARY_DIR}/cppcheck_report.xml)
set(CPPCHECK_HTML_REPORT ${CMAKE_BINARY_DIR}/cppcheck_report.html)
set(CPPCHECK_XSLT_STYLESHEET ${CMAKE_SOURCE_DIR}/xslt/cppcheck_report.xslt)

# 确保 XSLT 样式表存在
if(NOT EXISTS ${CPPCHECK_XSLT_STYLESHEET})
    message(FATAL_ERROR "Cppcheck XSLT stylesheet not found: ${CPPCHECK_XSLT_STYLESHEET}")
endif()

# ============ 生成抑制规则文件 ============
# 排除常见第三方库的头文件检查（ROS、Eigen3、PCL、Boost等）
file(WRITE ${CPPCHECK_SUPPRESSIONS} "# Suppress all checks in third-party libraries
# ROS libraries
*:/opt/ros/*
*:*/ros/*
*:*/catkin/*
*:*/roscpp/*
*:*/rosbag/*
*:*/std_msgs/*
*:*/sensor_msgs/*
*:*/geometry_msgs/*
*:*/nav_msgs/*
*:*/tf/*
*:*/tf2/*

# Eigen library
*:*/eigen3/*
*:*/Eigen/*

# PCL library
*:*/pcl/*
*:*/pcl-*/include/*

# TBB library
*:*/tbb/*
*:*/oneapi/*

# Boost library
*:*/boost/*

# OpenCV library
*:*/opencv/*
*:*/opencv2/*
*:*/opencv4/*

# System libraries
*:/usr/include/*
*:/usr/local/include/*

# Project third-party directories
*:*/thirdparty/*
*:*/_deps/*
*:*/external/*
*:*/third_party/*
*:*/vendor/*
")

# ============ 过滤编译数据库 ============
if(Python3_FOUND)
    add_custom_target(filter_compile_commands
        COMMAND ${Python3_EXECUTABLE} ${FILTER_SCRIPT} 
                ${CMAKE_BINARY_DIR}/compile_commands.json
                ${FILTERED_COMPILE_COMMANDS}
                ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Filtering compile_commands.json to exclude third-party libraries..."
        VERBATIM
    )
    set(CPPCHECK_COMPILE_DB ${FILTERED_COMPILE_COMMANDS})
else()
    set(CPPCHECK_COMPILE_DB ${CMAKE_BINARY_DIR}/compile_commands.json)
endif()

# ============ 运行 Cppcheck ============
add_custom_target(run_cppcheck
    COMMAND ${CPPCHECK_EXECUTABLE}
            --inline-suppr
            --std=c++17
            --enable=all
            --quiet
            --force
            --inconclusive
            --language=c++
            --project=${CPPCHECK_COMPILE_DB}
            --suppressions-list=${CPPCHECK_SUPPRESSIONS}
            --suppress=noValidConfiguration
            --suppress=missingIncludeSystem
            --max-configs=1
            -i${CMAKE_SOURCE_DIR}/3rdparty
            -i${CMAKE_BINARY_DIR}/_deps
            --xml
            --xml-version=2
            --output-file=${CPPCHECK_XML_REPORT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Cppcheck static analysis..."
    VERBATIM
)

if(Python3_FOUND)
    add_dependencies(run_cppcheck filter_compile_commands)
endif()

# ============ 生成 HTML 报告 ============
add_custom_target(generate_cppcheck_html
    COMMAND ${XSLTPROC_EXECUTABLE} ${CPPCHECK_XSLT_STYLESHEET} ${CPPCHECK_XML_REPORT} > ${CPPCHECK_HTML_REPORT}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating HTML report from Cppcheck XML..."
    VERBATIM
)

add_dependencies(generate_cppcheck_html run_cppcheck)

# ============ 组合目标 ============
add_custom_target(all_cppcheck_reports)
add_dependencies(all_cppcheck_reports generate_cppcheck_html)
