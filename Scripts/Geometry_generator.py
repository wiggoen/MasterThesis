
zoffset = 0.0

if zoffset > 0:
	print("# Miniball geometry with zoffset = +{}".format(zoffset))
elif zoffset == 0.0:
	print("# Miniball geometry with zoffset = {}".format(zoffset))
else:
	print("# Miniball geometry with zoffset = -{}".format(zoffset))

geometry = ["Theta", "Phi", "Alpha", "R"]
values = 0.0

for cluster in range(0, 8):
	for parameter in range(len(geometry)):
		if parameter == 0 or parameter == 2:
			print("Cluster_{}.{}: {}".format(cluster, geometry[parameter], values))
		if parameter == 1:
			print("Cluster_{}.{}:   {}".format(cluster, geometry[parameter], values))
		if parameter == 3:
			print("Cluster_{}.{}:     {}".format(cluster, geometry[parameter], values))

print()
print("# zoffset of target with respect to the centre of the array (mm) (+ = downstream)")
print("zoffset: {}".format(zoffset))