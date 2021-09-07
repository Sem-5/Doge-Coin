# Parameters
Parameters num_nodes, franction of slow nodes, TxnGen time and BlockIntervals are specified in NetSim constructor
TxnGen : specified per node
BlockInterval : specified for whole chain

# Logs
Each x_TREE.log file contains :
0/1 - 1 = fast and 0 = slow 
time_recv id parent_id nTxns minerID
.
.
.
Each x_TXN.log files contains the complete blockchain with all transactions (verbose)

Each x_CHAIN.log file contains the longest blockchain in order genesis -> latest

# Directions
Please create a logs/ directory in your build directory!!