import subprocess
import matplotlib.pyplot as plt
import numpy as np
import os

buildDir = "../build/"
logDir = "logs/"

for f in os.listdir(logDir):
    os.remove(logDir+f)
subprocess.run([buildDir+"sim", "30", "0.5", "7.5e3", "30e3", "6000e3"])

speed = []
isFast = []
frac = []

for node in range(30):
    with open( logDir + str(node) + "_TREE.log", 'r' ) as file:
        [t, fast, nMine] = file.readline().split()
    speed.append(float(t)/1e6)
    isFast.append(int(fast))
    nMine = int(nMine)
    with open ( logDir + str(node) + "_CHAIN.log", 'r' ) as file:
        miners = [ int(f.split()[1]) for f in file.readlines() ]
    nSuccess = miners.count(node)
    frac.append(nSuccess/nMine)

plt.scatter(speed, frac)
plt.xlabel("Processing Time Parameter (in 1000s)")
plt.ylabel("Fraction of Blockchain entries")
plt.title("Variation of Chain Dominance with CPU power")
plt.plot(np.unique(speed), np.poly1d(np.polyfit(speed, frac, 1))(np.unique(speed)), color="red")
plt.legend(['Best Fit Line','Points'])
plt.savefig("fracVCPU.png")
plt.show()

slow = np.array([ frac[i] for i in range(30) if isFast[i] == 0 ])
fast = np.array([ frac[i] for i in range(30) if isFast[i] != 0 ])

smean = np.mean(slow)
serr = np.std(slow)
fmean = np.mean(fast)
ferr = np.std(fast)

plt.errorbar([0, 1], [smean,fmean], [serr, ferr], color="red", ecolor="black", capsize=10)
plt.xlabel("0 : Slow and 1 : Fast")
plt.ylabel("Fraction of Blockchain entries")
plt.title("Variation of Chain Dominance with Network Speed")
plt.savefig("fracVnet.png")
plt.show()