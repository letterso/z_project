# APP_NAME: 对外的名称，可以和进程名一致
# PKG_NAME: 包名, 用公司域名倒过来 + app_name保证唯一性
# VERSION： 版本名，X.Y.Z
# 1、X为主版本，不需要向下兼；
# 2、Y为次版本，向下兼容，属于增量升级, 例如增加新功能，新接口；
# 3、Z为修订版本, 向下兼容, 比如修复某个函数里的某个逻辑;

set(APP_VERSION "1.0.0")
set(DESCRIPTION "模板工程")
set(APP_NAME "${PROJECT_NAME}")
set(PKG_NAME "cn.z.${APP_NAME}")
set(ARCH "amd64")
# set(APP_ROOT_DIR "$ENV{HOME}/.Workspace/${PKG_NAME}")
set(APP_ROOT_DIR "${TOP}/install")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCH})
set(CPACK_PACKAGE_NAME "${PKG_NAME}")
set(CPACK_PACKAGE_VERSION "${APP_VERSION}")
set(CPACK_PACKAGE_CONTACT "z@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "z@gmail.com")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${DESCRIPTION}")
set(CPACK_PACKAGING_INSTALL_PREFIX "${APP_ROOT_DIR}")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${TOP}/postinst")

# pack specific file
# install(
#     PROGRAMS ${CMAKE_BINARY_DIR}/bin/timer_node
#     DESTINATION ${APP_ROOT_DIR}/bin
#     USE_SOURCE_PERMISSIONS
# )
# install(
#     FILES ${TOP}/config/params.yaml
#     DESTINATION ${APP_ROOT_DIR}/config
# )
# install(
#     FILES ${TOP}/service/z-zproject.service
#     DESTINATION /lib/systemd/system
# )

# pack 3rdparty file
install(
  DIRECTORY ${TOP}/3rdparty/yaml-cpp/lib/
  DESTINATION ${APP_ROOT_DIR}/lib
)

# pack all file
install(
  DIRECTORY ${CMAKE_BINARY_DIR}/bin/
  DESTINATION ${APP_ROOT_DIR}/bin
  USE_SOURCE_PERMISSIONS
)
install(
  DIRECTORY ${CMAKE_BINARY_DIR}/lib/
  DESTINATION ${APP_ROOT_DIR}/lib
  FILES_MATCHING PATTERN "*.so"        # 可选：只拷贝 .so 文件
  # PATTERN "*.a" EXCLUDE              # 可选：排除 .a 文件
)
install(
  DIRECTORY ${TOP}/config/
  DESTINATION ${APP_ROOT_DIR}/config
)

include(CPack)