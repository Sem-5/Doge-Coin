import subprocess
import os
import matplotlib.pyplot as plt

buildDir = "../build/"
logDir = "logs/"


def nForks(parent):
    n = len(parent)
    nparents = len(set(parent))
    return n - nparents


def run(n, t):
    for f in os.listdir(logDir):
        os.remove(logDir+f)
    subprocess.run([buildDir+"sim", str(n), "0.5", str(t/20), str(t), str(t*30)])

nvalues = [5, 10, 15, 20]
tvalues = [1e3, 10e3, 50e3, 100e3]

forks = {}

for n in nvalues:
    for t in tvalues:
        print("Running on n = {} and t = {}".format(n,t), end='\r')
        run(n, t)
        avgForks = 0.0
        for node in range(n):
            with open(logDir + str(node) + "_TREE.log", 'r') as file:
                lines = file.readlines()[1:]
                parent = [ int(x.split()[2]) for x in lines ]
                avgForks += nForks(parent)/n
        forks[(n,t)] = avgForks

for n in nvalues:
    y = [ forks[(n,t)] for t in tvalues ]
    plt.scatter(tvalues, y)
    plt.xlabel("Time interval between blocks")
    plt.ylabel("Number of Forks")
    plt.title("Forks vs Time for N = {}".format(n))
    plt.savefig("expN_{}.png".format(n))
    plt.show()

for t in tvalues:
    y = [ forks[(n,t)] for n in nvalues ]
    plt.scatter(nvalues, y)
    plt.xlabel("Number of Nodes")
    plt.ylabel("Number of Forks")
    plt.title("Forks vs Nodes for T = {}".format(t))
    plt.savefig("expT_{}.png".format(int(t/1e3)))
    plt.show()