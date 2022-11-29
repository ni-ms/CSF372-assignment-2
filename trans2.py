import numpy as np


def fileopen():
    # fr = open("in2.txt", 'r')
    # fw = open("inp2.txt", 'w')
    # array = [[int(number) for number in col.split()] for col in fr]
    # fw.writelines(array)
    array = np.loadtxt("in2.txt", dtype='i', delimiter=' ')
    array = array.transpose()
    r = array.shape[0]
    # print(l)
    rt = np.arange(1, r + 1, 1)
    c = array.shape[1]
    ct = np.arange(1, c + 1, 1)
    # print(ct)
    # ar2 = array.tolist()
    # ar2.insert(0,0)
    # np.savetxt('inp2.txt', array, fmt = '%i')
    # np.insert(array, '\n', 0)
    # array = np.insert(array, 0, rt, axis=1)
    array = np.insert(array, 0, ct, axis=0)
    # print(array)
    np.savetxt('inp2.txt', array, fmt='%i')
    # print(array, sep = '\n')


fileopen()
