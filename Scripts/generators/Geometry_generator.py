"""
    Detector geometry:
    The generator is used to quickly create the parameters to the
    calibration file.
"""

# Geometry
def print_geometry():
    print("# Miniball geometry")
    geometry = ["R", "Theta", "Phi", "Alpha"]

    # Values
    R = [96.38, 95.17, 98.47, 92.64, 96.85, 98.06, 108.86, 104.33]
    Theta = [137.57, 46.25, 44.22, 138.70, 132.79, 40.00, 137.86, 32.26]
    Phi = [129.37, 56.49, 132.43, 52.95, 236.60, 235.05, 310.34, 318.96]
    Alpha = [322.40, 59.10, 298.73, 242.25, 294.55, 241.33, 65.21, 111.63]

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
