
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>
#include <numeric>

using namespace cv;
using namespace cv::dnn;
using namespace std;

// Parâmetros
float CONFIDENCE_THRESHOLD = 0.25;
vector<string> CLASSES;

string modelConfiguration = "cfg/yolov4-tiny.cfg";
string modelWeights = "yolov4-tiny.weights";
string classesFile = "cfg/obj.names";
string inputVideo = "data/video.mp4";  // Ou pode usar frame.jpg se for imagem única

Net net = readNetFromDarknet(modelConfiguration, modelWeights);

// Structs para guardar informações de detecção - importante para analizar a posição dos veículos nas areas de interesse
struct BoxDetectado {
    cv::Rect box;
    float confidence;
    int classId;
    cv::Point centro;
};

// Define as cores (BGR) para as areas de detecção
cv::Scalar corAzul(255, 0, 0);
cv::Scalar corVermelha(0, 0, 255);
cv::Scalar corAmarela(0, 255, 255);
cv::Scalar corVerde(0, 255, 0);
cv::Scalar corPreta(0, 0, 0);
cv::Scalar corBranca(255, 255, 255);

// Definição das areas das vias.  Azul (D/E) é uma via, e Lajana (D/E) é a outra via.
std::vector<cv::Point> regiao_azul_D = {
    cv::Point(1070, 347),
    cv::Point(810, 370),
    cv::Point(700, 290),
    cv::Point(1070, 260)
};
std::vector<cv::Point> regiao_azul_E = {
    cv::Point(290, 490),
    cv::Point(0, 515),
    cv::Point(0, 390),
    cv::Point(230, 370)
};
std::vector<cv::Point> regiao_vermelha_D = {
    cv::Point(1060, 590),
    cv::Point(750, 590),
    cv::Point(605, 460),
    cv::Point(819, 440)
};
std::vector<cv::Point> regiao_vermelha_E = {
    cv::Point(504, 322),
    cv::Point(256, 347),
    cv::Point(150, 170),
    cv::Point(275, 160)
};

// CLASSE DE OBJETO CarCounter
class CarCounter {
private:
    std::vector<uint8_t> readings;
    size_t maxSize;
    mutable std::mutex mtx;

public:
    CarCounter(size_t bufferSize = 5) : maxSize(bufferSize) {
        readings.reserve(bufferSize);
    }
    
    void add(uint8_t count) {
        std::lock_guard<std::mutex> lock(mtx);
        
        readings.push_back(count);
        
        // Remove o mais antigo se exceder o tamanho
        if (readings.size() > maxSize) {
            readings.erase(readings.begin());
        }
    }
    
    double get() const {
        std::lock_guard<std::mutex> lock(mtx);
        
        if (readings.empty()) return 0.0;
        
        uint32_t sum = std::accumulate(readings.begin(), readings.end(), 0u);
        return static_cast<double>(sum) / readings.size();
    }
};

// VARIÁVEIS GLOBAIS
CarCounter car_route_blue(5);  // Instancia um CarCounter que faz a média dos últimos 5 registros
CarCounter car_route_red(5);  // Instancia um CarCounter que faz a média dos últimos 5 registros


// DECLARAÇÃO DE FUNÇÕES / PROTOTIPAGEM
void iniciarContador();
float relacaoCarros();
void desenharRegiao(cv::Mat& frame, const std::vector<cv::Point>& pontos, const cv::Scalar& cor);
void loadClasses(const string& classesFile);
std::vector<BoxDetectado> detectarVeiculos(const cv::Mat& frame, float confianca, bool zoom = false, const std::vector<cv::Point>* roi = nullptr);


/**
 * @brief Conta o número de veículos dentro da região de interesse (ROI) e desenha retângulos ao redor deles.
 * @param frame Imagem onde vai desenhar os retângulos.
 * @param roi Vetor que define a ROI.
 * @param corBox Cor dos box que envolvem os carros.
 * @return Número de veículos dentro da ROI.
 */
int contarNaArea(const std::vector<BoxDetectado>& boxes, const std::vector<cv::Point>* roi, cv::Mat& frame, const cv::Scalar& corBox) {
    int contador = 0;
    for (const auto& box : boxes) {
        if (cv::pointPolygonTest(*roi, box.centro, false) >= 0) {
            cv::rectangle(frame, box.box, corBox, 2);
            contador++;
        }
    }
    return contador;
}

void loopContagem() {
    loadClasses(classesFile);

    net.setPreferableBackend(DNN_BACKEND_CUDA);
    net.setPreferableTarget(DNN_TARGET_CUDA);

    // net.setPreferableBackend(DNN_BACKEND_OPENCV);
    // net.setPreferableTarget(DNN_TARGET_CPU);

    VideoCapture cap(inputVideo);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir o vídeo.\n";
        return;
    }

    Mat frame;
    while (cap.read(frame)) {

        auto boxesFull = detectarVeiculos(frame, CONFIDENCE_THRESHOLD);  // detecta todos os veículos do frame
        auto boxesZoom = detectarVeiculos(frame, CONFIDENCE_THRESHOLD, true, &regiao_vermelha_E); // Pode ser qualquer ROI mais distante

        int nAzulD = contarNaArea(boxesFull, &regiao_azul_D, frame, corAzul);
        int nAzulE = contarNaArea(boxesFull, &regiao_azul_E, frame, corAzul);
        int nVermelhaD = contarNaArea(boxesFull, &regiao_vermelha_D, frame, corVermelha);
        int nVermelhaE = contarNaArea(boxesZoom, &regiao_vermelha_E, frame, corVermelha);

        car_route_blue.add(nAzulD + nAzulE);
        car_route_red.add(nVermelhaD + nVermelhaE);

        // MODO VISUAL 

        float rel = relacaoCarros();
        if (rel < 0) {
            rel *= -1;
            putText(frame, format("RELACAO: %0.2f", rel), Point(400, 40),
                    FONT_HERSHEY_SIMPLEX, 1, corAzul, 6);

        } else if (rel > 0) {
            putText(frame, format("RELACAO: %0.2f", rel), Point(400, 40),
                    FONT_HERSHEY_SIMPLEX, 1, corVermelha, 6);

        } else {
            putText(frame, format("RELACAO: %0.2f", rel), Point(400, 40),
                    FONT_HERSHEY_SIMPLEX, 1, corBranca, 6);

        }

        desenharRegiao(frame, regiao_azul_D, corAzul);
        desenharRegiao(frame, regiao_azul_E, corAzul);
        desenharRegiao(frame, regiao_vermelha_D, corVermelha);
        desenharRegiao(frame, regiao_vermelha_E, corVermelha);

        imshow("Deteccao", frame);
        if (waitKey(1) == 27) break; // ESC para sair
    }

    cap.release();
    destroyAllWindows();
}

void desenharRegiao(cv::Mat& frame, const std::vector<cv::Point>& pontos, const cv::Scalar& cor) {
    std::vector<std::vector<cv::Point>> contornos = { pontos };
    cv::polylines(frame, contornos, true, cor, 2);  // true = fecha o polígono
    for (const auto& p : pontos) {
        cv::circle(frame, p, 3, cor, -1);  // desenha cada ponto
    }
}

void loadClasses(const string& classesFile) {
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) {
        CLASSES.push_back(line);
    }
}

/**
 * @brief Detecta a posição dos veículos presentes no 'frame' e retorna as coordenadas de cada veículo detectado.
 * @param frame Imagem de entrada.
 * @param confianca Limite de confiança para a detecção.
 * @param zoom Se verdadeiro, aplica zoom na area definida no parâmetro roi (opcional).
 * @param roi Região de interesse (opcional).
 * @return Vetor de BoxDetectado contendo as coordenadas dos veículos detectados.
 */
std::vector<BoxDetectado> detectarVeiculos(const cv::Mat& frame, float confianca, bool zoom, const std::vector<cv::Point>* roi) {
    std::vector<BoxDetectado> resultados;
    cv::Mat frameProcessado = frame;
    cv::Rect boundingBox;

    if (zoom && roi) {
        boundingBox = cv::boundingRect(*roi);
        frameProcessado = frame(boundingBox);
    }

    // Pré-processamento
    cv::Mat blob;
    cv::dnn::blobFromImage(frameProcessado, blob, 1 / 255.0, cv::Size(416, 416), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outs;
    net.forward(outs, net.getUnconnectedOutLayersNames());

    for (const auto& output : outs) {
        for (int i = 0; i < output.rows; ++i) {
            cv::Mat scores = output.row(i).colRange(5, output.cols);
            cv::Point classIdPoint;
            double confidence;

            cv::minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);

            if (confidence > confianca) {
                int centerX = static_cast<int>(output.at<float>(i, 0) * frameProcessado.cols);
                int centerY = static_cast<int>(output.at<float>(i, 1) * frameProcessado.rows);
                int width = static_cast<int>(output.at<float>(i, 2) * frameProcessado.cols);
                int height = static_cast<int>(output.at<float>(i, 3) * frameProcessado.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                // Coordenadas absolutas
                int absLeft = left;
                int absTop = top;
                if (zoom && roi) {
                    absLeft += boundingBox.x;
                    absTop += boundingBox.y;
                    centerX += boundingBox.x;
                    centerY += boundingBox.y;
                }

                BoxDetectado box;
                box.box = cv::Rect(absLeft, absTop, width, height);
                box.confidence = confidence;
                box.classId = classIdPoint.x;
                box.centro = cv::Point(centerX, centerY);
                resultados.push_back(box);
            }
        }
    }

    return resultados;
}

float relacaoCarros() {
    uint8_t totalCarros = car_route_blue.get() + car_route_red.get();
    if (totalCarros == 0) return (float)0; // Evita divisão por zero
    
    float cvv = (float)car_route_red.get()/(float)totalCarros;
    float cva = (float)car_route_blue.get()/(float)totalCarros;
    return cvv - cva; // Retorna a relação entre os carros das vias, de +1 a -1
}

void iniciarContador() {
    std::thread t(loopContagem);
    t.detach(); // Permite que o loop de contagem rode em segundo plano
}