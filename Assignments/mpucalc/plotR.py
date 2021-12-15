import matplotlib.pyplot as plt

def R(a,g):
	return (a*(1-a)*(1-a)*(4*a+g*(1-2*a))-a*a*a)/(1-a*(1+(2-a)*a))

alp = [i/10 for i in range(1,6)]
gam = [0.25,0.5,0.75,1]
for g in gam:
	y = [R(a,g) for a in alp]
	plt.plot(alp,y,label="gamma={}".format(g))

plt.xlabel("aplha")
plt.ylabel("R_pool")
plt.legend()
plt.savefig("R_wala_plot.jpg")