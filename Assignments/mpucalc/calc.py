builddir = "../build/"
logdir = builddir + "logs/"

def longestchain(parent):
    depth = dict()
    for val in parent.values():
        depth[val] = 0
        orig = val
        while val != "-1":
            depth[orig] += 1
            val = parent[val]
    dlist = [ (d,node) for (node,d) in depth.items()]
    leaf = max(dlist)[1]
    chain = []
    while leaf != "-1":
        chain += [leaf]
        leaf = parent[leaf]
    return chain


miner = dict()
parent = dict()
with open(logdir+"50_TREE.log",'r') as file:
    mined_by_adv = int(file.readline().strip().split()[-1])
    for line in file.readlines():
        data = line.strip().split()
        id = data[1]
        pid = data[2]
        mid = data[4]
        parent[id] = pid
        miner[id] = mid
with open(logdir+"50_PCHAIN.log",'r') as file:
    pchain = file.readline().strip().split()
for i in range(1,len(pchain)):
    parent[pchain[i]] = pchain[i-1]
    miner[pchain[i]] = "50"
chain = longestchain(parent)
total_chain = len(chain)
total_adv_chain = sum([1 if miner[c] == "50" else 0 for c in chain])
total_mined = mined_by_adv
for i in range(50):
    with open(logdir+"{}_TREE.log".format(i),'r') as file:
        total_mined += int(file.readline().strip().split()[-1])
frac_adv = total_adv_chain/total_chain
mdu_adv = total_adv_chain/mined_by_adv
mdu_tot = total_chain/total_mined

print(mdu_adv, mdu_tot, frac_adv)