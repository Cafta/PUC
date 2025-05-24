
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace cv;
using namespace dnn;
using namespace std;

// Parâmetros
float CONFIDENCE_THRESHOLD = 0.3;
float NMS_THRESHOLD = 0.4;
int INPUT_WIDTH = 416;
int INPUT_HEIGHT = 416;
vector<string> CLASSES;

// Define as cores (BGR) para as areas de detecção
cv::Scalar corAzul(255, 0, 0);
cv::Scalar corLaranja(0, 140, 255);

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
std::vector<cv::Point> regiao_laranja_D = {
    cv::Point(1060, 590),
    cv::Point(750, 590),
    cv::Point(605, 460),
    cv::Point(819, 440)
};
std::vector<cv::Point> regiao_laranja_E = {
    cv::Point(504, 322),
    cv::Point(256, 347),
    cv::Point(150, 170),
    cv::Point(275, 160)
};

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

void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame) {
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(0, 255, 0), 2);
    string label = format("%.0f%% %s", conf * 100, CLASSES[classId].c_str());
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - labelSize.height),
              Point(left + labelSize.width, top + baseLine),
              Scalar(255, 255, 255), FILLED);
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,0));
}

int main() {
    string modelConfiguration = "cfg/yolov4-tiny.cfg";
    string modelWeights = "yolov4-tiny.weights";
    string classesFile = "cfg/obj.names";
    string inputVideo = "data/video.mp4";  // Ou pode usar frame.jpg se for imagem única

    loadClasses(classesFile);

    Net net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);

    VideoCapture cap(inputVideo);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir o vídeo.\n";
        return -1;
    }

    Mat frame;
    while (cap.read(frame)) {
        Mat blob;
        blobFromImage(frame, blob, 1/255.0, Size(INPUT_WIDTH, INPUT_HEIGHT), Scalar(0,0,0), true, false);
        net.setInput(blob);

        vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        vector<int> classIds;
        vector<float> confidences;
        vector<Rect> boxes;

        for (auto& output : outputs) {
            for (int i = 0; i < output.rows; ++i) {
                float* data = (float*)output.ptr(i);
                float confidence = data[4];
                if (confidence >= CONFIDENCE_THRESHOLD) {
                    Mat scores = output.row(i).colRange(5, output.cols);
                    Point classIdPoint;
                    double maxClassScore;
                    minMaxLoc(scores, 0, &maxClassScore, 0, &classIdPoint);
                    if (maxClassScore >= CONFIDENCE_THRESHOLD) {
                        int centerX = (int)(data[0] * frame.cols);
                        int centerY = (int)(data[1] * frame.rows);
                        int width = (int)(data[2] * frame.cols);
                        int height = (int)(data[3] * frame.rows);
                        int left = centerX - width / 2;
                        int top = centerY - height / 2;

                        classIds.push_back(classIdPoint.x);
                        confidences.push_back((float)maxClassScore);
                        boxes.push_back(Rect(left, top, width, height));
                    }
                }
            }
        }

        vector<int> indices;
        NMSBoxes(boxes, confidences, CONFIDENCE_THRESHOLD, NMS_THRESHOLD, indices);

        int count = 0;
        for (int i : indices) {
            if (CLASSES[classIds[i]] == "car" || CLASSES[classIds[i]] == "truck" || CLASSES[classIds[i]] == "bus") {
                drawPred(classIds[i], confidences[i], boxes[i].x, boxes[i].y,
                         boxes[i].x + boxes[i].width, boxes[i].y + boxes[i].height, frame);
                count++;
            }
        }

        putText(frame, format("Veiculos: %d", count), Point(20, 40),
                FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

        desenharRegiao(frame, regiao_azul_D, corAzul);
        desenharRegiao(frame, regiao_azul_E, corAzul);
        desenharRegiao(frame, regiao_laranja_D, corLaranja);
        desenharRegiao(frame, regiao_laranja_E, corLaranja);

        imshow("Deteccao", frame);
        if (waitKey(1) == 27) break; // ESC para sair
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
