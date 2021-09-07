# Parameters
TxnGen : specified per node in milliseconds  
BlockInterval : specified for whole chain in milliseconds  

# Logs
Each x_TREE.log file contains complete blocktree:  
mining_speed(in ms) 0/1(1 = fast and 0 = slow) number_of_mined_blocks   
time_recv id parent_id nTxns minerID  
.  
.  
.  
for each block id in blocktree  

Each x_TXN.log files contains the complete blockchain with all transactions (verbose)  

Each x_CHAIN.log file contains the blockchain in order genesis -> latest and each block's miner  

# Directions
To compile, follow :

```console
mkdir build
cd build
mkdir logs
cmake ..
make
```

Now you can run the sim executable with parameters
```console
./sim num_nodes fraction_slow txn_gen_time blk_intervals end_time
```

To test visualization scripts, enter the visualize directory and perform
```console
mkdir logs
```
before running the scripts
