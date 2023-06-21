import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import find_peaks
import datetime

# def serch_str(file_path, word):
#     with open(file_path, 'r') as file:
#         content = file.read()
#         if word in content:
#             print('find')
#         else: 
#             print('not find')

def serch_str(file_path, word):
    result = []
    with open(file_path, 'r') as file:
        for l_on, line in enumerate(file):
            if word in line:
                text = line.split()
                result.append(text[1])
    return result

fig = plt.figure()
ax = fig.add_subplot()

if __name__ == '__main__':
    # time =  serch_str('data 12.30.06.txt', 'TIME')
    # turn =  serch_str('data 12.30.06.txt', 'TURN')

    # # xmax1 = time[np.argmax(turn)]
    # # ymax1 = max(turn)

    # # xmax2 = time[np.argmax(turn)]
    # # ymax2 = max(turn)

    # ax.clear()
    # ax.plot(time, turn)

    # plt.xlabel('Time')
    # plt.ylabel('Turn')
    # plt.title('Grafic')

    # # ax.annotate('max 1', xy = (time[peaks[0]], turn[peaks[0]]))
    # # ax.annotate('max 2', xy = (time[peaks[1]], turn[peaks[1]]))

    # plt.show()
    time =  serch_str('data 12.32.20.txt', 'TIME')
    turn =  serch_str('data 12.32.20.txt', 'TURN')
    turn = [int(line) for line in turn]
    #fig = plt.figure()
    ax = fig.add_subplot()
    
    ax.clear()
    plt.xticks(rotation=90)
    plt.xlabel('Time')
    plt.ylabel('Turn')
    plt.title('Grafic')
    ax.plot(time, turn)
    plt.show()



# Fs = 8000
# f = 5
# sample = 8000

# x = np.arange(sample)
# y = np.sin(2*np.pi*f*x/Fs)
# plt.plot(x, y)
# plt.xlabel('sample(n)')
# plt.ylabel('voltage(V)')
# plt.show()