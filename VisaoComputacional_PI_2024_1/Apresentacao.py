import os
import cv2
import dlib
import face_recognition
import random
from ultralytics import YOLO
from PIL import Image, ImageDraw

if dlib.DLIB_USE_CUDA:
    print("dlib está utilizando a GPU.")
else:
    print("CUDA NOT available. dlib está utilizando a CPU.")

# Parâmetros:
showDetects = True
classesSearch = [197, 559, 569]  # para lista de classes vá em # lendo modelo pré treinado
                # 96,gato | 197,flauta | 240,gaita | 435,sax | 559,trumpet | 569,violino
confianca = 0.3  # para detecção dos objetos
modelo = 'yolov8l-oiv7.pt'
escala = 0.6 # escala da imagem para mostrar na tela
reduzObj = 0.2 # para ficar mais rápido o processamento
reduzFace = 0.2  # não dá para reduzir tanto a face se não não detecta
nSalvos = len(os.listdir('imagens/salvos/'))
ip = "https://192.168.15.35:8080/video"
pasta = 'imagens/' # Nesta pasta tem que ter 2 diretórios: conhecidos e salvos. o diretório conhecidos tem que ter as fotos das pessoas
frame_interval = 20
frame_count = 0
comentario = ''
cmt_expira = 120
cmt_count = 0

# ALGUMAS FUNÇÕES:
def index(dic, item):
    for key, value in dic.items():
        if value == item:
            return key
    return ''

def comenta(boxes_detectados, frame):
    global cmt_count
    global cmt_expira
    global comentario
    if (cmt_count % cmt_expira == 0 or comentario == ''):
        comentario = ''
        cmt_count = 0
        detectados = []
        for box in boxes_detectados:
            detectados.append(box.rotulo)
        if 'Carlos' in detectados:
            comentario = 'Lindo!'
        if 'Tatiana' in detectados:
            comentario = 'Linda!'
        if 'Carlos' in detectados and 'Violin' in detectados:
            comentario = 'Toca muito!!!'
        elif 'Carlos' in detectados and 'Trumpet' in detectados:
            comentario = 'Ta aprendendo!'
    cmt_count += 1

# ALGUMAS CLASSES:
class Conhecido:
    def __init__(self, nome: str, imagem_path: str):
        self.nome = nome
        self.encoding = self._gerar_encoding(imagem_path)

    def _gerar_encoding(self, imagem_path: str):
        # Carregar a imagem
        imagem = face_recognition.load_image_file(imagem_path)

        # Obter a codificação facial (assumindo que há apenas uma face na imagem)
        encodings = face_recognition.face_encodings(imagem)

        if encodings:
            return encodings
        else:
            raise ValueError("Nenhuma face encontrada na imagem.")

    def __repr__(self):
        return f"Conhecido(nome={self.nome})"

class Boxe:
    def __init__(self, rotulo: str, conf: float, x1, y1, x2, y2):
        self.rotulo = rotulo
        self.conf = conf
        self.x1 = round(x1)
        self.y1 = round(y1)
        self.x2 = round(x2)
        self.y2 = round(y2)

boxes_detectados = []
objetos_detectados = []
faces_detectadas = []
# # Definindo a cor do frame para cada classe
# corBorda = []
# for i in range(len(class_list)):
#     r = random.randint(0, 255)
#     g = random.randint(0, 255)
#     b = random.randint(0, 255)
#     corBorda.append([r, g, b])

# FACE_RECOGNITION - RECONHECIMENTO FACIAL
# Parte 1: CRIANDO BD COM PESSOAS CONHECIDAS:
conhecidos = []
for img in os.listdir(f'{pasta}conhecidos/'):
    if img.split('.')[-1] == 'jpg':
        conheco = Conhecido(img.split('.')[0], f'{pasta}/conhecidos/{img}')
        conhecidos.append(conheco)

# YOLO - RECONHEIMENTO DE OBJETOS
model = YOLO(modelo, 'v8')
nomes = model.names     # lista dos nomes das classes de classificação
# print(nomes.items())
print('Procurando apenas por:', end=' ')
for classe in classesSearch:
    print(nomes[classe], end='; ')
print('\n                      confiança > ', confianca)

cap = cv2.VideoCapture()
cap.open(ip)
if not cap.isOpened():
    print('Error opening video stream or file')
    exit(0)
while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print('Stream ended.')
        break
    tinyframe = cv2.resize(frame, None, fx=reduzObj, fy=reduzObj, interpolation=cv2.INTER_CUBIC)
    tinyFace = cv2.resize(frame, None, fx=reduzFace, fy=reduzFace, interpolation=cv2.INTER_CUBIC)

    # Trabalhando os resultados:  Processando apenas no intervalo... pulando alguns frames
    if frame_count % frame_interval == 0:
        objetos_detectados = []
        # RECONHECIMENTO DE INSTRUMENTOS
        results = model.predict(tinyframe, conf=confianca, classes=classesSearch, save=False, verbose=False)
        objetos = []
        if len(nomes) == 0:
            nomes = results[0].names  # array das classes
        if (showDetects):
            for box in results[0].boxes:
                #boxe = results[0].boxes[1].data[0].cpu().numpy()  # [x1, y1, x2, y2, confiança, classe]
                objeto = box.data[0].cpu().numpy()  # [x1, y1, x2, y2, confiança, classe]
                objetos.append(objeto)
                x1 = round(objeto[0])
                y1 = round(objeto[1])
                x2 = round(objeto[2])
                y2 = round(objeto[3])
                confi = objeto[4]
                classe = nomes[objeto[5]]
                boxe = Boxe(classe, confi, x1/reduzObj, y1/reduzObj, x2/reduzObj, y2/reduzObj)
                objetos_detectados.append(boxe)
    if (frame_count+6) % frame_interval == 0:
        faces_detectadas = []
        # RECONHECIMENTO FACIAL
        rgb_frame = cv2.cvtColor(tinyFace, cv2.COLOR_BGR2RGB)
        unknown_face_locations = face_recognition.face_locations(rgb_frame)
        unknown_face_encodings = face_recognition.face_encodings(rgb_frame, unknown_face_locations)
        for i in range(len(unknown_face_locations)):
            top, right, bottom, left = unknown_face_locations[i]
            # cv2.rectangle(frame, (left, top), (right, bottom), (0, 255, 255), 2)
            label = 'Desconhecido'
            unknown_face_encoding = unknown_face_encodings[i]
            for conhecido in conhecidos:
                results = face_recognition.compare_faces(conhecido.encoding, unknown_face_encoding)
                if any(results):
                    # print(f"A face desconhecida corresponde a(o) {conhecido.nome}!")
                    label = conhecido.nome
                # else:
                #     print("A face é desconhecida.")
            # print(label)
            boxe = Boxe(label, 0, left/reduzFace, top/reduzFace, right/reduzFace, bottom/reduzFace)
            faces_detectadas.append(boxe)

    frame_count += 1
    # print(faces_detectadas, objetos_detectados)
    boxes_detectados = objetos_detectados + faces_detectadas
    # if (len(boxes_detectados)>0):
    #     print(boxes_detectados)
    comenta(boxes_detectados, frame)
    for box in boxes_detectados:
        cv2.rectangle(frame, (box.x1, box.y1), (box.x2, box.y2), (0, 255, 0), 3)
        if box.conf>0:
            label = f"{box.rotulo}: {box.conf:.2f}"
        else:
            label = f"{box.rotulo}"
        cv2.putText(frame, label, (box.x1, box.y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), 6)

    cv2.putText(frame, comentario, (300, 100), cv2.FONT_HERSHEY_SIMPLEX, 3, (0, 255, 255), 12)

    mostra = cv2.resize(frame, None, fx=escala, fy=escala, interpolation=cv2.INTER_CUBIC)
    cv2.imshow("Reconhecimento", mostra)
    key = cv2.waitKey(1) & 0xFF
    if key == ord('p'):
        cv2.imwrite(f'{pasta}salvos/salvo{nSalvos}.jpg', frame)
        nSalvos += 1
        print('Capturado')
    elif key == ord('q'):
        break





