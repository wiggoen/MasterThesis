import matplotlib.pyplot as plt
import numpy as np

skip = [15, 31, 47, 63]  # Not able to use ring 16 in any quadrants
#front_special_case = [30, 62]  # First value in array for Ni is zero, can't fit with zero
back_special_case = [36]  # Quadrant 4, b1 (weird plot)

def front():
    print("Front calibration started...")
    # Front channels
    #front_ch_Ni = np.loadtxt("../fitting/output/Ni_front_ch.txt", usecols=1)
    front_ch_Pb_Sm = np.loadtxt("../fitting/output/Pb_Sm_front_ch.txt", skiprows=1, usecols=(1, 2))

    # Front energy (theoretical), needs to be flipped (wrong ordering, high -> low)
    #front_E_Ni = np.loadtxt("../fitting/output/Ni_energy_sim.txt", usecols=1)
    front_E_Pb_Sm = np.loadtxt("../fitting/output/Pb_Sm_energy_sim.txt", skiprows=1, usecols=(1, 2))

    # Front energy and channels
    #E_Ni = np.flip(front_E_Ni)  # assuming all quadrants give the same energy
    #E_Ni = np.concatenate((E_Ni, E_Ni, E_Ni, E_Ni))  # to get equal number of energies and channels
    E_Pb = np.flip(front_E_Pb_Sm[:, 0])
    E_Pb = np.concatenate((E_Pb, E_Pb, E_Pb, E_Pb))
    E_Sm = np.flip(front_E_Pb_Sm[:, 1])
    E_Sm = np.concatenate((E_Sm, E_Sm, E_Sm, E_Sm))

    #ch_Ni = front_ch_Ni    
    ch_Pb = front_ch_Pb_Sm[:, 0]
    ch_Sm = front_ch_Pb_Sm[:, 1]

    # Gathering the energies and channels in two multi dimensional arrays (matrices)
    factor = 1000  # from MeV to keV
    #E = np.array((E_Ni, E_Pb, E_Sm))*factor
    #ch = np.array((ch_Ni, ch_Pb, ch_Sm))
    E = np.array((E_Pb, E_Sm))*factor
    ch = np.array((ch_Pb, ch_Sm))

    # Arrays for gain and offset
    gain = np.zeros(len(ch_Sm))
    offset = np.zeros(len(ch_Sm))


    degree = 1  # polynomial fit degree
    for i in range(len(ch_Sm)):
        #print("i: {}, ch: {}, E: {}".format(i, ch[:, i], E[:, i]))
        if i not in skip:
            #if i not in front_special_case:
            gain[i], offset[i] = np.polyfit(ch[:, i], E[:, i], degree)
            #else:
            #    gain[i], offset[i] = np.polyfit(ch[1:, i], E[1:, i], degree)
        else:
            gain[i] = -1
            offset[i] = 0
    
    """
    print("Gain:")
    for i in gain:
        print("{:.2f}".format(i))

    print()
    print("Offset:")
    for i in offset:
        print("{:.2f}".format(i))
    """
    print("Front calibration ended")
    return gain, offset, E, ch


def back(ring_list):
    print("Back calibration started...")
    # Back channels
    #Ni_datafiles = []
    #for i in range(1, 15):
    #    filename = "../fitting/output/Ni_back_f{}_ch.txt".format(i)
    #    Ni_datafiles.append(filename)

    #ch_Ni = np.array([np.loadtxt(file, usecols=1) for file in Ni_datafiles])

    using_rings = ring_list

    Pb_Sm_datafiles = []
    #for i in range(1, 16):  # would use this if I had all rings
    for i in ring_list:
        filename = "../fitting/output/Pb_Sm_back_f{}_ch.txt".format(i)
        Pb_Sm_datafiles.append(filename)

    ch_Pb = np.array([np.loadtxt(file, skiprows=1, usecols=1) for file in Pb_Sm_datafiles]) 
    ch_Sm = np.array([np.loadtxt(file, skiprows=1, usecols=2) for file in Pb_Sm_datafiles]) 

    #f15_back_ch_Pb_Sm = np.loadtxt("../fitting/output/Pb_Sm_back_f15_ch.txt", usecols=(1, 2))

    # Front energy (theoretical), needs to be flipped (wrong ordering, high -> low)
    #front_E_Ni = np.loadtxt("../fitting/output/Ni_energy_sim.txt", usecols=1)
    front_E_Pb_Sm = np.loadtxt("../fitting/output/Pb_Sm_energy_sim.txt", skiprows=1, usecols=(1, 2))

    # Front energy and channels (energies for f1 - f15)
    #E_Ni = np.flip(front_E_Ni[2:])  # assuming all quadrants give the same energy
    E_Pb = np.flip(front_E_Pb_Sm[1:, 0])
    E_Sm = np.flip(front_E_Pb_Sm[1:, 1])

    filter_E_Pb = [E_Pb[i-1] for i in ring_list]
    filter_E_Sm = [E_Sm[i-1] for i in ring_list]

    E_Pb = np.array(filter_E_Pb)
    E_Sm = np.array(filter_E_Sm)

    # Gathering the energies in a multi dimensional array (matrix)
    factor = 1000  # from MeV to keV
    #E = np.array((E_Ni, E_Pb, E_Sm))*factor  
    E = np.array((E_Pb, E_Sm))*factor  

    # Arrays for gain and offset
    gain = np.zeros(len(ch_Sm[0]))
    offset = np.zeros(len(ch_Sm[0]))


    degree = 1  # polynomial fit degree
    for i in range(len(ch_Sm[0])):
        #ch_b = np.concatenate((ch_Ni[:, i], ch_Pb[:, i], ch_Sm[:, i]))
        #E_b = np.concatenate((E[0, :], E[1, 2:], E[2, :]))
        ch_b = np.concatenate((ch_Pb[:, i], ch_Sm[:, i]))
        E_b = np.concatenate((E[0, :], E[1, :]))
        if i not in back_special_case:
            gain[i], offset[i] = np.polyfit(ch_b, E_b, degree)
        else:
            #ch_b = np.concatenate((ch_Ni[:, i], ch_Pb[2:, i], ch_Sm[1:, i]))
            #E_b = np.concatenate((E[0, :], E[1, 2:], E[2, 1:]))
            ch_b = np.concatenate((ch_Pb[1:, i], ch_Sm[1:, i]))
            E_b = np.concatenate((E[0, 1:], E[1, 1:]))
            gain[i], offset[i] = np.polyfit(ch_b, E_b, degree)
        #print("i: {}, \nch: {},\nE: {}".format(i, ch_b, E_b))

    """
    print("Gain:")
    for i in gain:
        print("{:.2f}".format(i))

    print()
    print("Offset:")
    for i in offset:
        print("{:.2f}".format(i))
    """
    #return gain, offset, E, ch_Ni, ch_Pb, ch_Sm
    print("Back calibration ended")
    return gain, offset, E, ch_Pb, ch_Sm


# Call to functions
f_gain, f_offset, f_E, f_ch = front()
#b_gain, b_offset, b_E, b_ch_Ni, b_ch_Pb, b_ch_Sm = back()
using_rings = [2, 7, 12]
b_gain, b_offset, b_E, b_ch_Pb, b_ch_Sm = back(using_rings)


def plot_front(quadrant, ring):
    """
        Plots the calibration of the front detectors.
        Quadrant 1-4.
        Ring 1-16 (16 will give zero since we can't use it to calibrate).
    """
    max_ring = 16
    pos = (quadrant-1)*max_ring + ring-1
    ch = f_ch[:, pos]
    E = f_E[:, pos]
    #if pos in front_special_case:
    #    ch = f_ch[1:, pos]
    #    E = f_E[1:, pos]
    fig = plt.figure()
    plt.plot(ch, E, color='blue', marker="x", linestyle='None')
    plt.plot(ch, f_gain[pos]*ch+f_offset[pos], color='red')
    plt.title("Quadrant {}, ring {}".format(quadrant, ring))
    plt.xlabel("Channel")
    plt.ylabel("E (keV)")
    #plt.legend(["Ni, Pb, Sm", "Lin. fit"], loc=0)
    plt.legend(["Pb, Sm", "Lin. fit"], loc=0)
    fig.set_tight_layout(True)
    plt.show()


def plot_back(quadrant, strip):
    """
        Plots the calibration of the back detectors.
        Quadrant 1-4.
        Strip 1-12.
    """
    max_strip = 12
    pos = (quadrant-1)*max_strip + strip-1
    #ch_b = np.concatenate((b_ch_Ni[:, pos], b_ch_Pb[2:, pos], b_ch_Sm[:, pos])) # Excluding f1 & f2 for Pb
    #E_b = np.concatenate((b_E[0, :], b_E[1, 2:], b_E[2, :]))
    ch_b = np.concatenate((b_ch_Pb[:, pos], b_ch_Sm[:, pos]))
    E_b = np.concatenate((b_E[0, :], b_E[1, :]))
    if pos in back_special_case:
        #ch_b = np.concatenate((b_ch_Ni[:, pos], b_ch_Pb[2:, pos], b_ch_Sm[1:, pos]))
        #E_b = np.concatenate((b_E[0, :], b_E[1, 2:], b_E[2, 1:]))
        ch_b = np.concatenate((b_ch_Pb[1:, pos], b_ch_Sm[1:, pos]))
        E_b = np.concatenate((b_E[0, 1:], b_E[1, 1:]))
    fig = plt.figure()
    plt.plot(ch_b, E_b, color='blue', marker='x', linestyle='None')
    plt.plot(ch_b, b_gain[pos]*ch_b+b_offset[pos], color='red')
    plt.title("Quadrant {}, strip {}".format(quadrant, strip))
    plt.xlabel("Channel")
    plt.ylabel("E (keV)")
    #plt.legend(["Ni, (Pb), Sm", "Lin. fit"], loc=0)
    plt.legend(["Pb, Sm", "Lin. fit"], loc=0)
    fig.set_tight_layout(True)
    plt.show()


# Plot
plot_front(1, 7)  # quadrant, ring
plot_back(1, 7)  # quadrant, strip


# Gain (front and back)
gf_Q1 = f_gain[0:16]
gf_Q2 = f_gain[16:32]
gf_Q3 = f_gain[32:48]
gf_Q4 = f_gain[48:64]

gb_Q1 = b_gain[0:12]
gb_Q2 = b_gain[12:24]
gb_Q3 = b_gain[24:36]
gb_Q4 = b_gain[36:48]

gain = np.hstack((gf_Q1, gb_Q1, gf_Q2, gb_Q2, gf_Q3, gb_Q3, gf_Q4, gb_Q4))

# Offset (front and back)
of_Q1 = f_offset[0:16]
of_Q2 = f_offset[16:32]
of_Q3 = f_offset[32:48]
of_Q4 = f_offset[48:64]

ob_Q1 = b_offset[0:12]
ob_Q2 = b_offset[12:24]
ob_Q3 = b_offset[24:36]
ob_Q4 = b_offset[36:48]

offset = np.hstack((of_Q1, ob_Q1, of_Q2, ob_Q2, of_Q3, ob_Q3, of_Q4, ob_Q4))


for i in range(len(gain)):
    print("{}: g = {:.2f}, offs = {:.2f}".format(i, gain[i], offset[i]))


gain_file = "output/c_gain.txt"
offset_file = "output/c_offset.txt"

def write_to_file(file, array):
        with open(file, 'w') as outfile:
            for value in array:
                outfile.write("{:.2f}\n".format(value))


write_to_file(gain_file, gain)
write_to_file(offset_file, offset)
