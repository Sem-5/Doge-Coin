## NOTE
It feels like functions are running, but i really have no clue how to test tbh. I just saw blocks and transactions are being generated and received for 2 and 3 nodes simulation to keep track of shit.
Need to think of good parameter times and more debugging may be necessary 

## Input Format

n, z, end_time
T_1, T_2, ..., T_n
B_1, B_2, ..., B_n

T_i - mean time to generate new transaction for user i
B_i - mean time to generate new block for user i (i.e. time for PoW)
0<=z<=1
All times are in seconds