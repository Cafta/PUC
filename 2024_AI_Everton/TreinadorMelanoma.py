import os
import tensorflow as tf
import numpy as np
import glob
from sklearn.utils import shuffle

def extract_label(image_path):
    return os.path.basename(os.path.dirname(image_path))

# Defina o tamanho da imagem corretamente
img_shape_size = (122, 122)

# Defina o modelo
model1 = tf.keras.models.Sequential([
    tf.keras.layers.Input(shape=[img_shape_size[0], img_shape_size[1], 3]),
    tf.keras.layers.Conv2D(filters=32, kernel_size=3, padding="same", activation="relu"),
    tf.keras.layers.Conv2D(filters=32, kernel_size=3, padding="same", activation="relu"),
    tf.keras.layers.MaxPool2D(pool_size=2, strides=2, padding='valid'),
    tf.keras.layers.Conv2D(filters=64, kernel_size=3, padding="same", activation="relu"),
    tf.keras.layers.Conv2D(filters=64, kernel_size=3, padding="same", activation="relu"),
    tf.keras.layers.MaxPool2D(pool_size=2, strides=2, padding='valid'),
    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(units=128, activation='relu'),
    tf.keras.layers.Dense(units=2, activation='softmax')  # Ajuste para o número correto de classes
])

# Compilar o modelo
model1.compile(optimizer='adam', loss='sparse_categorical_crossentropy', metrics=['sparse_categorical_accuracy'])

# PRE PROCESSANDO AS IMAGENS

# Obtém os caminhos de todas as imagens na pasta de treinamento e teste
train_images = glob.glob('train/*/*.jpg')
test_images = glob.glob('test/*/*.jpg')

# Carrega as imagens e os rótulos
X_train = []; y_train = []; X_test = []; y_test = []
# Dicionário para mapear rótulos de string para valores numéricos
label_map = {}
current_label = 0

for image_path in train_images:
    # Carrega a imagem
    img = tf.keras.preprocessing.image.load_img(image_path, target_size=img_shape_size)
    # Converte a imagem para um array NumPy
    x = tf.keras.preprocessing.image.img_to_array(img)
    # Normaliza os valores dos pixels
    x /= 255.0
    # Obtém o rótulo da imagem
    label = extract_label(image_path)
    # Verifica se o rótulo já está no dicionário, caso contrário, adiciona
    if label not in label_map:
        label_map[label] = current_label
        current_label += 1
    # Adiciona a imagem e o rótulo às listas de treinamento
    X_train.append(x)
    y_train.append(label_map[label])

for image_path in test_images:
    # Carrega a imagem
    img = tf.keras.preprocessing.image.load_img(image_path, target_size=img_shape_size)
    # Converte a imagem para um array NumPy
    x = tf.keras.preprocessing.image.img_to_array(img)
    # Normaliza os valores dos pixels
    x /= 255.0
    # Obtém o rótulo da imagem
    label = extract_label(image_path)
    # Adiciona a imagem e o rótulo às listas de teste
    if label not in label_map:
        label_map[label] = current_label
        current_label += 1
    # Adiciona a imagem e o rótulo às listas de teste
    X_test.append(x)
    y_test.append(label_map[label])

# Embaralha os dados de treinamento
X_train, y_train = shuffle(X_train, y_train)
X_test, y_test = shuffle(X_test, y_test)

# Converte as listas em arrays NumPy
X_train = np.array(X_train)
y_train = np.array(y_train)
X_test = np.array(X_test)
y_test = np.array(y_test)

# Verifique as formas para garantir que estão corretas
print(f"Forma de X_train: {X_train.shape}")
print(f"Forma de y_train: {y_train.shape}")
print(f"Forma de X_test: {X_test.shape}")
print(f"Forma de y_test: {y_test.shape}")

# Treinando o modelo
model1.fit(X_train, y_train, epochs=15, batch_size=32, validation_data=(X_test, y_test))

# Salvar o modelo
model1.save('modelMelanomaNew.keras')
