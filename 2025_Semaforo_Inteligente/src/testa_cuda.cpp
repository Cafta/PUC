#include <opencv2/core.hpp>
#include <opencv2/core/cuda.hpp>
#include <iostream>

int main() {
    int num_devices = cv::cuda::getCudaEnabledDeviceCount();
    std::cout << "GPUs com CUDA disponíveis: " << num_devices << std::endl;

    if (num_devices > 0) {
        cv::cuda::printShortCudaDeviceInfo(0);
    } else {
        std::cout << "⚠️ OpenCV atual foi compilado sem suporte a CUDA, ou não há GPU CUDA disponível." << std::endl;
    }

    return 0;
}
