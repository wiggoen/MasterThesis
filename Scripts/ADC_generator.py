import numpy as np

# Si detector
#gain = np.loadtxt()
#offset = np.loadtxt()

gain = 1.0
offset = 0.0

print("# ADC calibration - CD front and back")
for adc in range(0, 4):
    for ch in range(0, 28):
        if ch < 10:
            print("adc_{}_{}.Gain:    {}".format(adc, ch, gain))#gain[adc, ch]*factor))
            print("adc_{}_{}.Offset:  {}".format(adc, ch, offset))#offset[adc, ch]*factor))
        else:
            print("adc_{}_{}.Gain:   {}".format(adc, ch, gain))#gain[adc, ch]*factor))
            print("adc_{}_{}.Offset: {}".format(adc, ch, offset))#offset[adc, ch]*factor))
 

# PAD
print()
print("# PAD")
ch = 31
for adc in range(0, 4):
    print("adc_{}_{}.Gain:   -1.0".format(adc, ch))
    print("adc_{}_{}.Offset: -1.0".format(adc, ch))


# Ionization Chamber
print()
print("# Ionization Chamber")
adc = 4
for ch in range(0, 2):
    print("adc_{}_{}.Gain:    1.0".format(adc, ch))
    print("adc_{}_{}.Offset:  0.0".format(adc, ch))


# ADC time offsets
print()
print("# ADC time offsets")
time_offsets = [-13, -16, -17, -10]
for adc in range(0, 4):
    print("adc_{}.TimeOffset: {}".format(adc, time_offsets[adc]))


# ADC thresholds
print()
print("# ADC thresholds")
treshold = 60
for ch in range(0, 16):
    if ch < 10: 
        print("adc_0_{}.Treshold:  {}".format(ch, treshold))
    else: 
        print("adc_0_{}.Treshold: {}".format(ch, treshold))
