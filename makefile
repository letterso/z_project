.PHONY: build install pack test help clean

define PRINT_HELP_PYSCRIPT
import re, sys
for line in sys.stdin:
	match = re.match(r'^([a-zA-Z_-]+):.*?## (.*)$$', line)
	if match:
		target, help = match.groups()
		print("%-20s %s" % (target, help))
endef
export PRINT_HELP_PYSCRIPT

# Release or Debug
build_type=Release
build_date=$(shell date +%Y-%m-%d)
project_dir=$(shell pwd)
build_dir=$(project_dir)/build
install_dir=$(project_dir)/install

help:
	@python3 -c "$$PRINT_HELP_PYSCRIPT" < $(MAKEFILE_LIST)

install: build ## 安装程序
	@cd $(build_dir) && make install && cd -

pack: build ## 打包程序
	@cd $(build_dir) && cpack && cd -

test:  ## 编译代码和单元测试
	@cmake -B $(build_dir) \
		  -DCMAKE_BUILD_TYPE=$(build_type) \
		  -DENABLE_TEST=ON
	@cmake --build $(build_dir) --parallel

build: ## 编译代码
	@cmake -B $(build_dir) \
		  -DBUILD_DATE=$(build_date) \
		  -DCMAKE_INSTALL_PREFIX=$(install_dir) \
		  -DCMAKE_BUILD_TYPE=$(build_type)
	@cmake --build $(build_dir) --parallel

check: ## 静态检测代码
	@cmake -B $(build_dir) \
		  -DCMAKE_BUILD_TYPE=$(build_type)
	@cmake --build $(build_dir) --target all_cppcheck_reports --parallel

clean: ## 删除编译缓存
	@rm -rf $(build_dir)
