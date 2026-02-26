# yaml-cpp
echo "Configuring and building 3rdparty/yaml-cpp"
tar -zxvf yaml-cpp-0.9.0.tar.gz
mkdir -p yaml-cpp-0.9.0/build
pushd yaml-cpp-0.9.0/build
cmake -DCMAKE_BUILD_TYPE=Release -DYAML_BUILD_SHARED_LIBS=on -DCMAKE_INSTALL_PREFIX=../release ..
make -j8
make install
popd
rm -rf yaml-cpp/lib
rm -rf yaml-cpp/include
mkdir -p yaml-cpp/lib
cp -r yaml-cpp-0.9.0/release/include yaml-cpp
cp yaml-cpp-0.9.0/release/lib/libyaml-cpp.so.0.9.0 yaml-cpp/lib/libyaml-cpp.so.0.9
rm -rf yaml-cpp-0.9.0