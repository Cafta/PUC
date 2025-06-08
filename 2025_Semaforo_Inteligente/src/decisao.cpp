#include <iostream>
#include <chrono>
#include <thread>

int via = 0;

float getRelacao() {
    return 0.3;
}

auto calculaTempo() -> std::chrono::steady_clock::time_point {
    auto retorno = std::chrono::steady_clock::now();
    float ajuste = 60 * (getRelacao() + 1);
    retorno += std::chrono::seconds(static_cast<int>(ajuste));
    return retorno;
}

int main() {
    using namespace std::chrono;
    auto deadline = calculaTempo();
    
    std::cout << "Esperando acabar o tempo";

    while (steady_clock::now() < deadline) {
        std::this_thread::sleep_for(milliseconds(500));
        std::cout << ".";
        std::cout.flush();
    }

    std::cout << "\nAcabou o tempo.\n";

    return 0;
}