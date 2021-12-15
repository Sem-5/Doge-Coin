import matplotlib.pyplot as plt

x = [i/10 for i in range(1,6)]

with open("op2.txt", 'r') as f:
	lines = [[float(x) for x in y.split()] for y in f.readlines()]
	d = dict()
	d[0.25]=[[],[],[]]
	d[0.5]=[[],[],[]]
	d[0.75]=[[],[],[]]
	d[1]=[[],[],[]]
	i=0
	while(i<len(lines)):
		for j in range(3):
			d[lines[i][1]][j].append(lines[i+1][j])
		i+=2
	yax = ["mpu_adv", "mpu_total", "frac_adv"]
	for i in range(3):
		plt.figure()

		for j in d.keys():
			plt.plot(x,d[j][i][:5], label="gamma={}".format(j))
		plt.xlabel("alpha")
		plt.ylabel(yax[i])
		plt.legend()
		plt.savefig("fig2_{}.jpg".format(i))



