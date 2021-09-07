# Parameters
Parameters num_nodes, franction of slow nodes, TxnGen time and BlockIntervals are specified in NetSim constructor  
TxnGen : specified per node  
BlockInterval : specified for whole chain  

# Logs
Each x_TREE.log file contains :  
mining_speed(ms) 0/1(1 = fast and 0 = slow)   
time_recv id parent_id nTxns minerID  
.  
.  
.  
Each x_TXN.log files contains the complete blockchain with all transactions (verbose)  

Each x_CHAIN.log file contains the longest blockchain in order genesis -> latest  

# Build
To compile input the following commands in the shell
```console
mkdir build
cd build 
cmake ..
make
```
Please create a `logs/` directory in your build directory!!

# Run
Run the following command from `build/` directory 
```console
./sim num_nodes fraction_slow txn_gen_time blk_intervals end_time
```
