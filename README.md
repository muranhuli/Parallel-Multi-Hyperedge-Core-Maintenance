# Efficient Traversal for Core Maintenance in Large-Scale Dynamic Hypergraphs

the source code of Efficient Traversal for Core Maintenance in Large-Scale Dynamic Hypergraphs

## About the dataset

We preprocess all datasets by removing isolated vertices.

## About how to use

```bash
g++ xxx.cpp -fopenmp -std=c++11 -o xxx
./xxx
```

## About the function of the code

**coreDistributed.py :** Calculate the distribution of core number for each dataset based on the calculated core number, which is used in Experiment 6.1.

**stability-insert.cpp and stability-delete.cpp :** Stability of the computational algorithm, computed under a single thread, which is used in Experiment 6.2.

**parallelism-insert.cpp and parallelism-delete.cpp :** Calculate the parallelism of the algorithm, calculate the average edge processing time under different multi-threaded threads, which is used in Experiment 6.4.

**unprun-insert.cpp and unprun-delete.cpp :** Counting the average number of traversal vertices under pruning strategy. The unpruned version can be obtained by commenting out the pruned part, which is used in Experiment 6.5.

**state-of-the-art-insert.cpp and state-of-the-art-delete.cpp :** The state-of-the-art algorithm[1] for insertion and deletion, which is used in Experiment 6.5.

**breakpoint-insert.cpp and breakpoint-delete :** Calculate the break point, which is used in Experiment 6.6.

**Note :** For Experiment 6.3, after obtaining the generated graph data, apply stability-insert.cpp and stability-delete.cpp.

## A simple running demo

Our operations are all implemented on stability-insert.

1. We need to modify the corresponding path in the file.

2. `g++ stability-insert.cpp -fopenmp -std=c++11 -o stability-insert`

3. `./stability-insert`

4. We can view the results in the folder corresponding to the generated file.
