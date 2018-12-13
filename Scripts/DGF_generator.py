

# Cores
print("# DGF calibration - Ge cores")
ch_core = 0
for dgf in range(0, 47, 2):
	if dgf < 10: 
		print("dgf_{}_{}.GainQuadr:  {}".format(dgf, ch_core, 0.0))
		print("dgf_{}_{}.Gain:       {}".format(dgf, ch_core, 0.0616898))
		print("dgf_{}_{}.Offset:     {}".format(dgf, ch_core, 0.0))
	else:
		print("dgf_{}_{}.GainQuadr: {}".format(dgf, ch_core, 0.0))
		print("dgf_{}_{}.Gain:      {}".format(dgf, ch_core, 0.0616898))
		print("dgf_{}_{}.Offset:    {}".format(dgf, ch_core, 0.0))

# Segments
print()
print("# DGF calibration - segments")
for dgf in range(0, 48):
	if dgf < 10:
		if dgf % 2 == 0: # even
			for ch in range(1, 3):
				print("dgf_{}_{}.Gain:    {}".format(dgf, ch, 0.0616898))
				print("dgf_{}_{}.Offset:  {}".format(dgf, ch, 0.0))
		else:
			for ch in range(4):
				print("dgf_{}_{}.Gain:    {}".format(dgf, ch, 0.0616898))
				print("dgf_{}_{}.Offset:  {}".format(dgf, ch, 0.0))
	else:
		if dgf % 2 == 0: # even
			for ch in range(1, 3):
				print("dgf_{}_{}.Gain:   {}".format(dgf, ch, 0.0616898))
				print("dgf_{}_{}.Offset: {}".format(dgf, ch, 0.0))
		else:
			for ch in range(4):
				print("dgf_{}_{}.Gain:   {}".format(dgf, ch, 0.0616898))
				print("dgf_{}_{}.Offset: {}".format(dgf, ch, 0.0))

# Beam dump calibration
print()
print("# Beam dump calibration")
for ch in range(0, 2):
	print("dgf_{}_{}.Gain:   {}".format(53, ch, 0.0616898))
	print("dgf_{}_{}.Offset: {}".format(53, ch, 0.0))
