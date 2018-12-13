import subprocess
import sys
import os


settingfile = "../../Miniball/MiniballCoulexSort/config/MBSettings2017_CLX_IS558.dat"


def Samarium_run():
	return [25]#, 26, 31]


def Nickel_run():
	return [47]#, 50, 52, 53, 54, 55, 56, 57, 58, 59]


element_dictionary = { "Sm": ["samarium", Samarium_run()], 
					   "Ni": ["nickel", Nickel_run()] }


def convert_data(element):
	print("Converting {} data...".format(element_dictionary[element][0]))
	run_number = element_dictionary[element][1]
	#MedToRoot = "/Users/trondwj/GitHub/MiniballCoulexSort/MedToRoot/./MedToRoot"


	for number in run_number:
		if element == 'Sm':
			medfile = "../Raw_data/Sm/140Sm_208Pb_pos6_0{}.med".format(number)

		elif element == 'Ni':
			medfile = "../Raw_data/Ni/144Ba_58Ni_pos5_0{}.med".format(number)

		#command = "MedToRoot -mf {} -sf {}".format(medfile, settingfile)
		command = 'MedToRoot -mf ../Raw_data/Sm/140Sm_208Pb_pos6_025.med -sf ../../Miniball/MiniballCoulexSort/config/MBSettings2017_CLX_IS558.dat'

		# Run program
		#os.system(command)
		#libCore = "../../Miniball/MiniballCoulexSort/lib/libCore.so"
		#subprocess.call("echo $DYLD_LIBRARY_PATH")
		subprocess.call(command, shell=True)#, shell=True)
		
		#environment = os.environ.copy()
		#environment["PATH"] = "/Users/trondwj/GitHub/ROOT-famework/build/lib:/Users/trondwj/GitHub/ROOT-famework/build/bin:/Users/trondwj/GitHub/Miniball/MiniballCoulexSort/lib:/Users/trondwj/GitHub/Miniball/MiniballCoulexSort/bin:" + environment["PATH"]
		##environment = os.getenv("/Users/trondwj/GitHub/ROOT-framework/build/lib:/Users/trondwj/GitHub/ROOT-framework/build/bin:/Users/trondwj/GitHub/Miniball/MiniballCoulexSort/lib:/Users/trondwj/GitHub/Miniball/MiniballCoulexSort/bin")
		#subprocess.Popen(command, shell=True, env=environment)


def usage_message():
	print("Usage of {}:".format(str(sys.argv[0])))
	print("Provide one command line argument 'Sm' or 'Ni' to sort the respective data.")
	print('Number of arguments:', len(sys.argv), 'arguments.')
	print('Argument List:', str(sys.argv))


# Check the number of parameters (command line arguments)
if len(sys.argv) < 2 or len(sys.argv) > 2:
	usage_message()
else:
	if str(sys.argv[1]) == 'Sm' or str(sys.argv[1]) == 'Ni':
		convert_data(sys.argv[1])
	else:
		usage_message()
