"""
    ADC calibration:
    The generator is used to quickly create the parameters to the
    calibration file.
"""

import numpy as np

# Si (ADC) detector
def print_ADC():
    gain = np.loadtxt("../calibration/output/c_gain.txt")
    offset = np.loadtxt("../calibration/output/c_offset.txt")
    
    print("# ADC calibration - CD front and back")
    ch_max = 28
    for adc in range(0, 4):
        for ch in range(0, 28):
            if ch < 10:
                print("adc_{}_{}.Gain:     {}".format(adc, ch, gain[adc*ch_max + ch]))#gain[adc, ch]*factor))
                print("adc_{}_{}.Offset:   {}".format(adc, ch, offset[adc*ch_max + ch]))#offset[adc, ch]*factor))
            else:
                print("adc_{}_{}.Gain:    {}".format(adc, ch, gain[adc*ch_max + ch]))#gain[adc, ch]*factor))
                print("adc_{}_{}.Offset:  {}".format(adc, ch, offset[adc*ch_max + ch]))#offset[adc, ch]*factor))
        

# PAD
def print_PAD():
    # values from online calibration
    values = [2.32738, 0.0, 2.20913, 0.0, 2.32738, 0.0, -1.0, -1.0]

    print()
    print("# PAD")
    ch = 31
    for adc in range(0, 4):
        print("adc_{}_{}.Gain:    {}".format(adc, ch, values[adc*2]))
        print("adc_{}_{}.Offset:  {}".format(adc, ch, values[adc*2+1]))


# Ionization Chamber
def print_IC():
    # values from online calibration
    values = [6.155992859, -132.4543943529, 0.2527, 25.4667]

    print()
    print("# Ionization Chamber")
    adc = 4
    for ch in range(0, 2):
        print("adc_{}_{}.Gain:    {}".format(adc, ch, values[ch*2]))
        print("adc_{}_{}.Offset:  {}".format(adc, ch, values[ch*2+1]))


# ADC time offsets
def print_time_offsets():
    print()
    print("# ADC time offsets")
    time_offsets = [0, -2, -3, 5]
    for adc in range(0, 4):
        print("adc_{}.TimeOffset:  {}".format(adc, time_offsets[adc]))


# ADC thresholds
def print_thresholds():
    print()
    print("# ADC thresholds")
    threshold = 100  # default value
    for adc in range(0, 4):
        for ch in range(0, 28):
            if ch < 10: 
                print("adc_{}_{}.Threshold:   {}".format(adc, ch, threshold))
            else: 
                print("adc_{}_{}.Threshold:  {}".format(adc, ch, threshold))


# Choose what to print to terminal
print_ADC()
#print_PAD()
#print_IC()
#print_time_offsets()
#print_thresholds()
