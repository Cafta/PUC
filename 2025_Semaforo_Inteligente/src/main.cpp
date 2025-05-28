#include <iostream>
#include <chrono>
#include <thread>
#include "contador.hpp"

int main() {
    iniciarContador();  // Inicia a thread de contagem

    while (true) {
        float relacao = relacaoCarros();
        std::cout << "Relação de carros: " << relacao << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
