
# export CMAKE_PREFIX_PATH=./libtorch
# export CUDA_CUDART_LIBRARY=/usr/local/cuda/lib64/libcudart.so
# export CMAKE_CUDA_COMPILER=/usr/local/cuda/bin/nvcc
# export CUDNN_LIBRARY_PATH=/usr/local/cuda/lib64/ 
# export CUDNN_INCLUDE_DIR=/usr/local/cuda/include/

rm -r build
mkdir build
cd build
cmake -G Ninja -DCMAKE_PREFIX_PATH=./libtorch -DCMAKE_CUDA_COMPILER=/usr/local/cuda-12.0/bin/nvcc -DCUDNN_LIBRARY_PATH=/usr/local/cuda-12.0/lib64/ -DCUDNN_INCLUDE_PATH=/usr/local/cuda-12.0/include/ -DLD_LIBRARY_PATH=/usr/local/cuda-12.0/targets/x86_64-linux/lib ..
cd ..
cmake --build build --config Release 
