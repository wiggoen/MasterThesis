import numpy as np


# Known LAB angles for beam
angle_B = np.asarray([[22.0, 71.669, 36.636],
                      [26.0, 68.386, 43.198],
                      [29.1, 65.860, 48.247],
                      [32.2, 63.352, 53.261],
                      [35.2, 60.944, 58.073],
                      [37.9, 58.796, 62.368],
                      [40.4, 56.824, 66.311],
                      [42.8, 54.948, 70.062],
                      [45.0, 53.244, 73.469],
                      [47.1, 51.633, 76.690],
                      [49.0, 50.189, 79.578],
                      [50.7, 48.909, 82.138],
                      [52.4, 47.640, 84.675],
                      [53.9, 46.531, 86.893],
                      [55.3, 45.505, 88.946],
                      [56.7, 44.488, 90.980]])


# Known LAB angles for target
angle_T = np.asarray([[93.756, 22.0, 135.969],
                      [85.775, 26.0, 127.965],
                      [80.185, 29.1, 121.762],
                      [74.977, 32.2, 115.560],
                      [70.220, 35.2, 109.558],
                      [66.127, 37.9, 104.157],
                      [62.467, 40.4, 99.156],
                      [59.052, 42.8, 94.356],
                      [55.993, 45.0, 89.955],
                      [53.129, 47.1, 85.756],
                      [50.579, 49.0, 81.956],
                      [48.326, 50.7, 78.556],
                      [46.098, 52.4, 75.157],
                      [44.151, 53.9, 72.158],
                      [42.348, 55.3, 69.358],
                      [40.558, 56.7, 66.559]])


def print_angles(angle):
    print("th_B  &  th_T  &  th_B_CM")
    for i in range(len(angle)):
        print("{:.1f}  &  {:.1f}  &  {:.1f}".format(angle[i][0], angle[i][1], angle[i][2]))
    print()


# Choose what to print to terminal
print_angles(angle_B)
print_angles(np.flip(angle_T, 0))

