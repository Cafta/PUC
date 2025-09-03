
import dlib

if dlib.DLIB_USE_CUDA:
    print("dlib está utilizando a GPU.")
else:
    print("CUDA NOT available. dlib está utilizando a CPU.")