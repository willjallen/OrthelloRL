from cgi import test
from ctypes import *


lib = WinDLL("C:\\Users\\WilliamAllen\\Desktop\\School\\thesis\\Orthello\\orthelloDLL.dll") # Not fine
print(lib)

testBoard = ((c_int * 8) * 8)()

# print(testBoard[9][0])

testBoard[3][3] = 1;
testBoard[4][3] = 2;
testBoard[3][4] = 2;
testBoard[4][4] = 1;

print('array ptr: ', byref(testBoard))

for i in testBoard:
    for j in i:
        print(j, end='')
    print('')
    

player = c_int(1)

# https://stackoverflow.com/questions/58610333/c-function-called-from-python-via-ctypes-returns-incorrect-value/58611011#58611011
lib.calculateLegalMoves.argtypes = (((c_int * 8) * 8), c_int)
lib.calculateLegalMoves(testBoard, player);

print()

for i in testBoard:
    for j in i:
        print(j, end='')
    print('')