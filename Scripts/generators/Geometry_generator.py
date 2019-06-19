"""
    Detector geometry:
    The generator is used to quickly create the parameters to the
    calibration file.
"""

# Geometry
def print_geometry():
    print("# Miniball geometry (mixed betas)")
    geometry = ["R", "Theta", "Phi", "Alpha"]

    # Values
    R = [107.08, 100.59, 105.76, 105.40, 106.48, 105.18, 127.04, 110.18]
    Theta = [129.79, 51.83, 51.23, 130.31, 128.74, 46.67, 131.04, 46.46]
    Phi = [126.67, 62.74, 126.87, 57.44, 235.53, 239.09, 308.67, 309.09]
    Alpha = [311.16, 51.08, 309.02, 251.90, 296.93, 233.45, 59.42, 130.56]

    values = [R, Theta, Phi, Alpha]

    for cluster in range(0, 8):
        for parameter in range(len(geometry)):
            if parameter == 1 or parameter == 3:
                print("Cluster_{}.{}:  {:.2f}".format(cluster, geometry[parameter], values[parameter][cluster]))
            if parameter == 2:
                print("Cluster_{}.{}:    {:.2f}".format(cluster, geometry[parameter], values[parameter][cluster]))
            if parameter == 0:
                print("Cluster_{}.{}:      {:.2f}".format(cluster, geometry[parameter], values[parameter][cluster]))


# zoffset
def print_zoffset():
    zoffset = 0.0

    print()
    print("# zoffset of target with respect to the centre of the array (mm) (+ = downstream)")
    print("zoffset:  {}".format(zoffset))


# Choose what to print to terminal
print_geometry()
print_zoffset()
