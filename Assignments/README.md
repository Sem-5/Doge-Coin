# Parameters
Parameters num_nodes, franction of slow nodes, TxnGen time and BlockIntervals are specified in NetSim constructor  
TxnGen : specified per node  
BlockInterval : specified for whole chain  

# Logs
Each x_TREE.log file contains :  
mining_speed(ms) 0/1(1 = fast and 0 = slow) number_mined   
time_recv id parent_id nTxns minerID  
.  
.  
.  
Each x_TXN.log files contains the complete blockchain with all transactions (verbose)  

Each x_CHAIN.log file contains the longest blockchain in order genesis -> latest and each block's miner  

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
