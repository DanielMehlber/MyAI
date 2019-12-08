from ctypes import *
import numpy as np

dll = None

try:
    dll = cdll.LoadLibrary("MyAI.dll")
except OSError:
    print("Cannot load DLL")

#functions from dll
newCNN = dll.newCNN
newCNN.restype = c_void_p
newCNN.argtypes = [POINTER(c_uint), c_uint]

delCNN = dll.delCNN
delCNN.restype = None
delCNN.argtype = c_void_p

class CNN:
    def __init__(self, layers):
        self.layer_count = len(layers)
        self.layer_data = layers
        data_array = np.asarray(layers)
        self.cnn = c_void_p(newCNN(data_array, self.layer_count))
    def __del__(self):
        delCNN(self.cnn)