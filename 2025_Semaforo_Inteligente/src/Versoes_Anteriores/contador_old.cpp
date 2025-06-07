#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/cudawarping.hpp>    // redimensionamento na GPU (opcional)
#include <opencv2/cudaarithm.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>

using namespace cv;
using namespace cv::dnn;
using namespace std;

// ----------------------------------------------------
// Parâmetros (mantive os seus)
float CONFIDENCE_THRESHOLD = 0.25;
float SCORE_THRESHOLD      = 0.25;
float NMS_THRESHOLD        = 0.45;
int   IMG_WIDTH            = 416;
int   IMG_HEIGHT           = 416;

// Estatísticas compartilhadas
atomic<uint32_t> carros_via_azul     = 0;
atomic<uint32_t> carros_via_vermelha = 0;

// ----------------------------------------------------
// Carrega rede YOLO
const string modelConfiguration = "cfg/yolov4-tiny.cfg";
const string modelWeights       = "yolov4-tiny.weights";
Net net = readNetFromDarknet(modelConfiguration, modelWeights);

// ---> GPU!
static void configurarDNNparaCUDA() {
    // Se OpenCV foi compilado com o módulo DNN/CUDA, isto coloca toda a
    // inferência na GPU.
    net.setPreferableBackend(DNN_BACKEND_CUDA);
    net.setPreferableTarget (DNN_TARGET_CUDA);
}

// ----------------------------------------------------
// Função de loop de contagem
void loopContagem() {
    configurarDNNparaCUDA();

    VideoCapture cap(0);   // câmera padrão
    if (!cap.isOpened()) {
        cerr << "Nao abriu camera.\n";
        return;
    }

    // Pré-aloca GpuMat (evita realloc)
    cv::cuda::GpuMat gpuFrame, gpuResized;

    while (true) {
        Mat frame;
        if (!cap.read(frame)) break;

        // ---- Parte opcional: transfere para GPU, redimensiona lá ----
        gpuFrame.upload(frame);
        cv::cuda::resize(gpuFrame, gpuResized,
                         Size(IMG_WIDTH, IMG_HEIGHT), 0, 0, cv::INTER_LINEAR);

        Mat resized;
        gpuResized.download(resized);   // blobFromImage ainda espera Mat CPU

        // Cria blob
        Mat blob = blobFromImage(resized, 1 / 255.0,
                                 Size(IMG_WIDTH, IMG_HEIGHT),
                                 Scalar(), true, false);

        // Inferência
        net.setInput(blob);
        vector<Mat> outs;
        net.forward(outs, net.getUnconnectedOutLayersNames());

        // Pós-processamento igual ao seu código original ----------------
        vector<int> classIds;
        vector<float> confidences;
        vector<Rect> boxes;

        for (auto &output : outs) {
            for (int i = 0; i < output.rows; ++i) {
                float *data = (float *)output.ptr(i);
                float confidence = data[4];
                if (confidence >= CONFIDENCE_THRESHOLD) {
                    Mat scores = output.row(i).colRange(5, output.cols);
                    Point classIdPoint;
                    double maxClassScore;
                    minMaxLoc(scores, nullptr, &maxClassScore, nullptr, &classIdPoint);

                    if (maxClassScore > SCORE_THRESHOLD) {
                        int centerX = (int)(data[0] * frame.cols);
                        int centerY = (int)(data[1] * frame.rows);
                        int width   = (int)(data[2] * frame.cols);
                        int height  = (int)(data[3] * frame.rows);
                        int left    = centerX - width / 2;
                        int top     = centerY - height / 2;

                        classIds.push_back(classIdPoint.x);
                        confidences.push_back((float)maxClassScore);
                        boxes.emplace_back(left, top, width, height);
                    }
                }
            }
        }

        // NMS
        vector<int> indices;
        NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);

        // Contagem simples (exemplo): classe 2 = carro
        uint32_t conta = 0;
        for (int idx : indices) {
            if (classIds[idx] == 2) conta++;
        }
        carros_via_vermelha = conta;   // só para demonstração

        // Opcional: mostrar fps / bounding boxes
        // ...
    }
}

// ----------------------------------------------------
// API exposta ao main.cpp
float relacaoCarros() {
    uint32_t total = carros_via_azul + carros_via_vermelha;
    if (total == 0) return 0.0f;
    return static_cast<float>(carros_via_vermelha - carros_via_azul) / total;
}

void iniciarContador() {
    std::thread t(loopContagem);
    t.detach();
}
