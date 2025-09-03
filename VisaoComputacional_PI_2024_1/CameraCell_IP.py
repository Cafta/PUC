"""
    1) Instalar o programa "IP Webcam" no celular
    2) Executar o servidor no celular (start server)
    3) Anotar o número IP com /video no final.
"""
import cv2
video = cv2.VideoCapture()
ip = "https://192.168.15.35:8080/video"
video.open(ip)
while True:
    ret, frame = video.read()
    small_frame = cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)
    imgCanny = cv2.Canny(small_frame, 10, 100)
    cv2.imshow('video ao vivo2', imgCanny)
    cv2.imshow('video ao vivo', small_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

"""
Filma
processa as imagens identifcando os instrumentos musicias
Identifica o rosto da pessoa
    if (violino e Bruno) = "tocando bem"
    if (trompete e Carlos) = "tocando muito mal"
    if (saxsofone e Murilo) = "tocando muito"
"""
