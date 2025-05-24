# Projeto: Semáforo Inteligente com Contagem de Veículos

**Plataforma-alvo:** NVIDIA Jetson Nano
**Objetivo:** Utilizar visão computacional para contabilizar veículos em diferentes vias de um cruzamento, alimentando a lógica de um semáforo adaptativo.

---

## ⚙️ Ferramentas e Tecnologias Escolhidas (versão atualizada)

| Componente                           | Ferramenta                           | Justificativa                                         |
| ------------------------------------ | ------------------------------------ | ----------------------------------------------------- | 
| **Linguagem**                        | C++17                                | Desempenho nativo e integração direta com OpenCV; fácil portar para embarcados |                                                        
| **Visão Computacional & Inferência** | **OpenCV DNN + YOLOv4‑tiny**         | Carrega rede **uma vez**, processa vídeo em tempo real (≥ 18 FPS na Jetson Nano), sem dependência de frameworks externos; API simples |
| **Aceleração GPU**                   | CUDA via OpenCV (backend `DNN_CUDA`) | OpenCV compila nativamente na Jetson Nano; ganho de 2‑3× no FPS |
| **Gerência de dependências**         | CMake + pkg‑config                   | Padrão de mercado para C++ e OpenCV |                                                                                                                  |
| **Ambiente de Dev**                  | VS Code + Ubuntu WSL (x86)           | Facilita debug no desktop e depois cross‑compile |                                                                                                         |

---

## 🚫 Tecnologias Avaliadas mas Não Adotadas

| Alternativa                                 | Motivo da Rejeição                                                                                                               |
| ------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| **Darknet CLI** (`./darknet detector test`) | Recarrega pesos a cada frame ⇒ <3 FPS; exige I/O de arquivos intermediários; pouca flexibilidade para overlay ao vivo            |
| **Darknet como lib C**                      | Rápida, mas build complexo, documentação esparsa e dependente de macros; OpenCV DNN já entrega desempenho suficiente sem esforço |
| **YOLOv5/v8 (PyTorch)**                     | Requer Python + Torch (≈1 GB RAM extra); Nano roda mas com FPS baixo sem TensorRT; port para C++ não oficial                     |
| **MobileNet‑SSD, NanoDet**                  | Mais leves, porém menor precisão em cenários noturnos e distante; comunidade menor                                               |
| **Background Subtraction (MOG2)**           | Extremamente leve, mas não distingue objetos individuais e falha com oclusão                                                     |
| **TensorRT**                                | Ótimo para produção, mas demanda pipeline ONNX → TRT. Também é uma excelente escolha como solução deste problema |

---

## 📁 Estrutura do Projeto

```
2025_Semaforo_Inteligente/
├── cfg/
│   ├── yolov4-tiny.cfg
│   └── obj.names              ← classes (car, bus, truck…)
├── yolov4-tiny.weights        ← Pesos do modelo
├── src/
│   └── contador_via_opencv.cpp
├── build/                     ← (gerado pelo CMake)
└── README.md
```

