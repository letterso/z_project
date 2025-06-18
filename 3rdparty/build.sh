# spdlog
echo "Configuring and building 3rdparty/spdlog ..."
tar -zxvf spdlog-1.15.3.tar.gz
mkdir -p spdlog-1.15.3/build
pushd spdlog-1.15.3/build
cmake -DCMAKE_BUILD_TYPE=Release -DSPDLOG_BUILD_PIC=ON -DCMAKE_INSTALL_PREFIX:PATH=../release ..
make -j8
make install
popd
rm -rf spdlog/lib
rm -rf spdlog/include
mkdir -p spdlog/lib
cp -r spdlog-1.15.3/release/include spdlog
cp spdlog-1.15.3/release/lib/libspdlog.a spdlog/lib/libspdlog.a
rm -rf spdlog-1.15.3

# yaml-cpp
echo "Configuring and building 3rdparty/yaml-cpp"
tar -zxvf yaml-cpp-0.8.0.tar.gz
mkdir -p yaml-cpp-0.8.0/build
pushd yaml-cpp-0.8.0/build
cmake -DCMAKE_BUILD_TYPE=Release -DYAML_BUILD_SHARED_LIBS=on -DCMAKE_INSTALL_PREFIX=../release ..
make -j8
make install
popd
rm -rf yaml-cpp/lib
rm -rf yaml-cpp/include
mkdir -p yaml-cpp/lib
cp -r yaml-cpp-0.8.0/release/include yaml-cpp
cp yaml-cpp-0.8.0/release/lib/libyaml-cpp.so.0.8.0 yaml-cpp/lib/libyaml-cpp.so.0.8
rm -rf yaml-cpp-0.8.0