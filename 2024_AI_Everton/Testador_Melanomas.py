import cv2
import os
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

# Abridno o modelo modelMelanoma.h5
model = tf.keras.models.load_model('modelMelanomaNew.keras')
# Recompilar o modelo
model.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['sparse_categorical_accuracy'])
categorias = {1: 'Maligno', 0: 'Benigno'}

def analisa(imagemPath):
    # Carrega a imagem e a converte para um array NumPy
    img = tf.keras.preprocessing.image.load_img(imagemPath, target_size=(122, 122))
    x = tf.keras.preprocessing.image.img_to_array(img)

    # Normaliza os pixels
    x /= 255.0
    # Adiciona a imagem à lista de teste
    X_test_image = np.expand_dims(x, axis=0)
    # Obtém a previsão do modelo
    predicted_label = np.argmax(model.predict(X_test_image))
    # Obtém o nome da classe prevista
    predicted_class_name = categorias[predicted_label]

    # Mostra a imagem e o rótulo previsto
    plt.imshow(x)
    plt.title(f"Predicted Label: {predicted_class_name}")
    plt.axis('off')
    plt.show()

analisa('test/Benign/6304.jpg')
analisa('test/Malignant/5605.jpg')

video = cv2.VideoCapture(0)
nImagens = len(os.listdir('imgs'))
while True:
    ret, frame = video.read()
    cv2.imshow('video ao vivo', frame)
    frame = cv2.resize(frame, (244,244))
    key = cv2.waitKey(1) & 0xFF
    if key == ord('p'):
        cv2.imwrite(f'imgs/image{nImagens+1}.jpg', frame)
        nImagens += 1
        print('Capturado')
    elif key == ord('q'):
        break
    elif key == ord('a'):
        cv2.imwrite(f'imgs/image{nImagens+1}.jpg', frame)
        nImagens += 1
        print('Analizando')
        # Analisa a ultima amostra
        analisa(f'imgs/image{nImagens}.jpg')
video.release()
cv2.destroyAllWindows()
