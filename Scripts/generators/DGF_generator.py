"""
    DGF calibration:
    The generator is used to quickly create the parameters to the
    calibration file.
"""

# Cores
def print_cores():
    print("# DGF calibration - Ge cores")
    ch_core = 0
    gain_quadrant = 0.0
    gain = 0.0616898
    offset = 0.0
    for dgf in range(0, 47, 2):
        if dgf < 10: 
            print("dgf_{}_{}.GainQuadr:   {}".format(dgf, ch_core, gain_quadrant))
            print("dgf_{}_{}.Gain:        {}".format(dgf, ch_core, gain))
            print("dgf_{}_{}.Offset:      {}".format(dgf, ch_core, offset))
        else:
            print("dgf_{}_{}.GainQuadr:  {}".format(dgf, ch_core, gain_quadrant))
            print("dgf_{}_{}.Gain:       {}".format(dgf, ch_core, gain))
            print("dgf_{}_{}.Offset:     {}".format(dgf, ch_core, offset))

# Segments
def print_segments():
    print()
    print("# DGF calibration - segments")
    gain = 0.0616898
    offset = 0.0
    for dgf in range(0, 48):
        if dgf < 10:
            if dgf % 2 == 0: # even
                for ch in range(1, 3):
                    print("dgf_{}_{}.Gain:     {}".format(dgf, ch, gain))
                    print("dgf_{}_{}.Offset:   {}".format(dgf, ch, offset))
            else:
                for ch in range(4):
                    print("dgf_{}_{}.Gain:     {}".format(dgf, ch, gain))
                    print("dgf_{}_{}.Offset:   {}".format(dgf, ch, offset))
        else:
            if dgf % 2 == 0: # even
                for ch in range(1, 3):
                    print("dgf_{}_{}.Gain:    {}".format(dgf, ch, gain))
                    print("dgf_{}_{}.Offset:  {}".format(dgf, ch, offset))
            else:
                for ch in range(4):
                    print("dgf_{}_{}.Gain:    {}".format(dgf, ch, gain))
                    print("dgf_{}_{}.Offset:  {}".format(dgf, ch, offset))

# Beam dump calibration
def print_beam_dump_cal():
    print()
    print("# Beam dump calibration")
    dgf = 53
    gain = 0.0616898
    offset = 0.0
    for ch in range(0, 2):
        print("dgf_{}_{}.Gain:    {}".format(dgf, ch, gain))
        print("dgf_{}_{}.Offset:  {}".format(dgf, ch, offset))


# Choose what to print to terminal
print_cores()
print_segments()
print_beam_dump_cal()
