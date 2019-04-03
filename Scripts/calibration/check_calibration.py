import matplotlib.pyplot as plt
import numpy as np 


# Initialization
quadrants = 4
rings = 16
strips = 12

# Choice of x-axis range
x_values = np.arange(1, 600)

# Read data
cal_online = np.loadtxt("input/cal_online.txt", usecols=1, skiprows=1)
cal_user = np.loadtxt("input/cal_user.txt", usecols=1, skiprows=1)
cal_user_wNi = np.loadtxt("input/cal_user_wNi.txt", usecols=1, skiprows=1)

gain_online = cal_online[::2]
offset_online = cal_online[1::2]
gain_user = cal_user[::2]
offset_user = cal_user[1::2]
gain_user_wNi = cal_user_wNi[::2]
offset_user_wNi = cal_user_wNi[1::2]


def plot_detector_side(detector_side):
    """
        Shows the different attempts of calibration vs the online calibration.
        The online calibration is very good, so comparing to this is kind of a 
        goal of the calibration.
    """
    if detector_side == "f":
        start = 0
        end = rings
    if detector_side == "b":
        start = rings
        end = rings+strips
    
    plot_number = [221, 222, 223, 224]
    
    for i in range(start, end):
        fig = plt.figure(i+1)
        for Q in range(quadrants):
            if i < 16:
                detector_side = ["Front", "f"]
                detector_num = i+1
            else:
                detector_side = ["Back", "b"]
                detector_num = i+1-rings
            title_name = "{} detector, Q{}, {}{}".format(detector_side[0], Q+1, detector_side[1], detector_num)
            fig_name = "{}{:02d}".format(detector_side[1], detector_num)
            pos = Q*(rings+strips) + i
            #print(pos)
            ax = plt.subplot(plot_number[Q])
            plt.plot(x_values, x_values*gain_online[pos]+offset_online[pos], color="red", label="online", linestyle="-")
            plt.plot(x_values, x_values*gain_user[pos]+offset_user[pos], color="blue", label="user wo/Ni", linestyle="--")
            plt.plot(x_values, x_values*gain_user_wNi[pos]+offset_user_wNi[pos], color="green", label="user w/Ni", linestyle="-.")
            plt.xlabel("x-values")
            plt.ylabel("y-values")
            plt.title(title_name)
            plt.legend(loc="lower right")
            plt.tick_params(labelsize=14)
            plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
            ax.yaxis.get_offset_text().set_x(-0.1)
            fig.set_tight_layout(True)
            plt.savefig("../../Plots/calibration/{}.png".format(fig_name))
        plt.clf()  # Clear figure
    #plt.show()


# Choose (f)ront side or (b)ack side of detector.
plot_detector_side("f")
plot_detector_side("b")


def plot_linear_calibration(detector_side):
    """
        Shows the lines used to get the gain (slope) and offset (y-intercept) for 
        each quadrant.
        Can't tell anything about the calibration since the individual differences
        in the detectors will give different gain and offset for each detector.
    """
    if detector_side == "f":
        start = 0
        end = rings
    if detector_side == "b":
        start = rings
        end = rings+strips
    
    plot_number = [221, 222, 223, 224]
    
    for Q in range(quadrants):
        fig = plt.figure(Q+1)
        for i in range(start, end):
            if i < 16:
                title_name = "Front detector, Q{}".format(Q+1)
                fig_name = "Q{}_front".format(Q+1)
            else:
                title_name = "Back detector, Q{}".format(Q+1)
                fig_name = "Q{}_back".format(Q+1)
            pos = Q*(rings+strips) + i
            #print(pos)
            plt.plot(x_values, x_values*gain_online[pos]+offset_online[pos], label=i, linestyle="-")
            plt.xlabel("x-values")
            plt.ylabel("y-values")
            plt.title(title_name)
            plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
            plt.tick_params(labelsize=14)
            plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
            fig.set_tight_layout(True)
            plt.savefig("../../Plots/calibration/{}.png".format(fig_name))
        plt.clf()  # Clear figure
    #plt.show()


# Choose (f)ront calibration or (b)ack calibration.
#plot_linear_calibration("f")
#plot_linear_calibration("b")


def plot_all_calibration(detector_side):
    """
        Shows all the lines used to get the gain (slope) and offset (y-intercept).
        Can't tell anything about the calibration since the individual differences
        in the detectors will give different gain and offset for each detector.
    """
    if detector_side == "f":
        start = 0
        end = rings
    if detector_side == "b":
        start = rings
        end = rings+strips
    
    linestyles = ["-", "--", "-.", ":"]

    fig = plt.figure()
    for Q in range(quadrants):
        for i in range(start, end):
            if i < 16:
                title_name = "Front detector"
                fig_name = "front"
            else:
                title_name = "Back detector"
                fig_name = "back"
            pos = Q*(rings+strips) + i
            #print(pos)
            if (i == start):
                plt.plot(x_values, x_values*gain_online[pos]+offset_online[pos], label="Q{}".format(Q+1), linestyle=linestyles[Q])
            else:
                plt.plot(x_values, x_values*gain_online[pos]+offset_online[pos], linestyle=linestyles[Q])
            plt.xlabel("x-values")
            plt.ylabel("y-values")
            plt.title(title_name)
            plt.tick_params(labelsize=14)
            plt.ticklabel_format(axis='y', style='sci', scilimits=(0,0))
    plt.legend(loc="best")
    fig.set_tight_layout(True)
    plt.savefig("../../Plots/calibration/all_{}.png".format(fig_name))
    #plt.show()

# Choose (f)ront calibration or (b)ack calibration.
#plot_all_calibration("f")
#plot_all_calibration("b")
