*****README FILE*******
Cache Simulator Readme:

1.Prerequisites:
C++ compiler (e.g., g++)
Make sure you have the necessary permissions to read files and execute the compiled program.

2.To compile the cache simulator, use the following command in the terminal:
g++ cache_simulator.cpp -o cache_simulator

3.Execution:
After compilation, you can run the cache simulator using the following command:
./cache_simulator

INPUT FILES FORMAT:

The cache.config file should be formatted as follows:
Cache Size
Block Size
Associativity
Replacement Policy (FIFO, LRU, RANDOM)
Write Policy (WT, WB)

For example:
1024
64
4
LRU
WT

Memory Access Trace File (cache.access):
Each line should contaion mode: address

example:
R: 0x00401234
W: 0x00A0FEDC

Output:
for each address it prints address,its index ,weather it is hit or miss and tag.

example:
address:0x00401234, Set:0x12, Miss, Tag:0x40
