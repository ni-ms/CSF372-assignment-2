import numpy as np

def fileopen():
   # fr = open("in2.txt", 'r')
   #fw = open("inp2.txt", 'w')
   # array = [[int(number) for number in col.split()] for col in fr]
   # fw.writelines(array)
   array = np.loadtxt("in2.txt", dtype='i', delimiter = ' ')
   array = array.transpose()
   np.savetxt('in2.txt', array, fmt= '%i')
   # print(array, sep = '\n')
fileopen()
