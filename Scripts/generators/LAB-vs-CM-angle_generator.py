"""
    LAB vs CM angle generator
    The generator is used to quickly create the angles between
    the two reference frames.
"""

import matplotlib.pyplot as plt
import numpy as np

# Initialization
alpha = 140/208  # ratio of the projectile (beam) mass to the target mass

#lab_angles = np.radians(np.asarray([68.825, 65.336, 62.154, 44.47]))  # test angles
#lab_angles2 = np.radians(np.asarray([25.463, 29.745, 33.690, 56.725]))  # test angles

# Input angles
lab_angles = np.radians(np.asarray([22, 26, 29.1, 32.2, 35.2, 37.9, 40.4, 42.8, 
                                    45.0, 47.1, 49.0, 50.7, 52.4, 53.9, 55.3, 
                                    56.7]))  # target angles in LAB frame

def theta_B(th_T, alpha):
    # Two-particle collision in the LAB frame
    th_B = np.degrees(np.arctan(np.sin(2*th_T)/(alpha-np.cos(2*th_T))))
    # Two-particle collision in the center of mass (CM) frame
    th_B_CM = np.degrees(np.pi - 2*th_T)
    # Print results
    print("B (LAB)  &  T (LAB)  &  B (CM)")
    for num in range(th_T.size):
        if (th_B[num] < 0):
            th_B[num] = 180 + th_B[num]
        print("{:.1f}     &  {:.1f}     &  {:.1f}".format(th_B[num], np.degrees(th_T[num]), th_B_CM[num]))


def plot_kinematics(alpha):
    size = 14
    fig = plt.figure()
    CM_angle = np.radians(np.linspace(0, 180, 1001))
    theta_T = 0.5*(np.pi-CM_angle)
    theta_B = np.remainder(np.arctan(np.sin(CM_angle)/(alpha+np.cos(CM_angle))), np.pi)
    # Plot results
    plt.plot(np.degrees(CM_angle), np.degrees(theta_B), label=r"$^{140}$Sm", color="blue")
    plt.plot(np.degrees(CM_angle), np.degrees(theta_T), label=r"$^{208}$Pb", color="red")
    plt.xlabel(r"$\theta_{x, LAB}$ [degrees]", fontsize=size)
    plt.ylabel(r"$\theta_{x, CM}$ [degrees]", fontsize=size)
    plt.legend(loc=0, fontsize=size)
    plt.tick_params(labelsize=size)
    fig.set_tight_layout(True)
    plt.show()

# Choose what to print to terminal
theta_B(lab_angles, alpha)
plot_kinematics(alpha)
