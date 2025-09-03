import cv2

file = 'imagens/piramide.jpg'
img = cv2.imread(file)
img = cv2.resize(img, None, fx=0.5, fy=0.5)
grayImg = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
imgBlur = cv2.GaussianBlur(img, (5, 5), 0)
imgBWBlur = cv2.GaussianBlur(grayImg, (5, 5), 0)
imgCanny = cv2.Canny(img, 10, 100)
imgCanny2 = cv2.Canny(img, 50, 100)

imgBWBlurCanny = cv2.Canny(imgBWBlur, 50, 100)
imgDilate = cv2.dilate(imgCanny, (5, 5), iterations=5)
imgErode = cv2.erode(imgCanny, (5, 5), iterations=2)

# Obs: Opening = Erosao, seguida de dilatação - ajuda a tirar ruidos da imagem
#      Closing = o inverso, ajuda a preencher um objeto.
imgOpening = cv2.morphologyEx(imgCanny, cv2.MORPH_OPEN, (5, 5))
imgClosing = cv2.morphologyEx(imgCanny, cv2.MORPH_CLOSE, (5, 5))

cv2.imshow('image', img)
#cv2.imshow('gray', grayImg)
#cv2.imshow('blur', imgBlur)
#cv2.imshow('canny', imgCanny)
#cv2.imshow('canny2', imgCanny2)
#cv2.imshow('cannyBlur', imgBWBlurCanny)
cv2.imshow('dilate', imgDilate)
cv2.imshow('erode', imgErode)
cv2.imshow('opening', imgOpening)
cv2.imshow('closing', imgClosing)

cv2.waitKey(0)