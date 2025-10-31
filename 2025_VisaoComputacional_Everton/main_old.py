import cv2
import mediapipe as mp

ip = "http:192.168.15.34:8080/video"

# Inicializa o módulo de pose do MediaPipe
mp_pose = mp.solutions.pose
pose = mp_pose.Pose()

# Inicializa o módulo de desenho do MediaPipe
mp_drawing = mp.solutions.drawing_utils

# Abre a câmera (0 para a câmera padrão)
cap = cv2.VideoCapture(0)
# cap = cv2.VideoCapture(ip)

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)


while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Converte a imagem para RGB
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Processa a imagem para encontrar a pose
    results = pose.process(rgb_frame)

    # Desenha os pontos e conexões da pose na imagem
    if results.pose_landmarks:
        # Exemplo: Desenhar apenas os landmarks do nariz, ombros e cotovelos
        landmarks = results.pose_landmarks.landmark
        desired_landmarks = [
            # landmarks[mp_pose.PoseLandmark.NOSE.value],
            landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER.value],
            landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER.value],
            landmarks[mp_pose.PoseLandmark.LEFT_ELBOW.value],
            landmarks[mp_pose.PoseLandmark.RIGHT_ELBOW.value],
            landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value],
            landmarks[mp_pose.PoseLandmark.RIGHT_WRIST.value],
        ]

        # Desenha apenas os landmarks desejados
        for landmark in desired_landmarks:
            x = int(landmark.x * frame.shape[1])
            y = int(landmark.y * frame.shape[0])
            cv2.circle(frame, (x, y), 5, (0, 255, 0), -1)

            # Você também pode desenhar conexões específicas se necessário
            # Por exemplo, a conexão entre os ombros
            LEFT_SHOULDER = landmarks[mp_pose.PoseLandmark.LEFT_SHOULDER.value]
            RIGHT_SHOULDER = landmarks[mp_pose.PoseLandmark.RIGHT_SHOULDER.value]
            LEFT_ELBOW = landmarks[mp_pose.PoseLandmark.LEFT_ELBOW.value]
            RIGHT_ELBOW = landmarks[mp_pose.PoseLandmark.RIGHT_ELBOW.value]
            LEFT_WRIST = landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value]
            RIGHT_WRIST = landmarks[mp_pose.PoseLandmark.RIGHT_WRIST.value]
            x1, y1 = int(LEFT_SHOULDER.x * frame.shape[1]), int(LEFT_SHOULDER.y * frame.shape[0])
            x2, y2 = int(RIGHT_SHOULDER.x * frame.shape[1]), int(RIGHT_SHOULDER.y * frame.shape[0])
            x3, y3 = int(LEFT_ELBOW.x * frame.shape[1]), int(LEFT_ELBOW.y * frame.shape[0])
            x4, y4 = int(RIGHT_ELBOW.x * frame.shape[1]), int(RIGHT_ELBOW.y * frame.shape[0])
            x5, y5 = int(LEFT_WRIST.x * frame.shape[1]), int(LEFT_WRIST.y * frame.shape[0])
            x6, y6 = int(RIGHT_WRIST.x * frame.shape[1]), int(RIGHT_WRIST.y * frame.shape[0])
            cv2.line(frame, (x1, y1), (x2, y2), (255, 0, 0), 2)  # Quadril Superior
            cv2.line(frame, (x1, y1), (x3, y3), (255, 0, 0), 2)  # Braço E
            cv2.line(frame, (x3, y3), (x5, y5), (255, 0, 0), 2)  # Antebraço E
            cv2.line(frame, (x2, y2), (x4, y4), (255, 0, 0), 2)  # Braço D
            cv2.line(frame, (x4, y4), (x6, y6), (255, 0, 0), 2)  # Antebraço D

    # Exibe o frame
    cv2.imshow('MediaPipe Pose', frame)

    # Sai do loop se a tecla 'q' for pressionada
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libera a câmera e fecha as janelas
cap.release()
cv2.destroyAllWindows()
