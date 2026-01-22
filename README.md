# Clone
Clone with submodules: `git clone --recursive https://github.com/StephanieRct/UE5PNCDemo.git UE5PNCDemo`

# UE5PNCDemo
Demo of UE5PNC from https://github.com/StephanieRct/UE5PNC

It will create multiple "centipedes" as sequences of soft body nodes.

[![](https://img.youtube.com/vi/2_2SYdp8bsg/0.jpg)](https://www.youtube.com/watch?v=2_2SYdp8bsg)

# Results

Captured on Intel Core i9-7940X CPU:

<img width="450" alt="Intel Core i9-7940X" src="https://github.com/user-attachments/assets/3ee10e9b-edfe-4e34-84d9-d259f22330a6" />

## Single-thread results
Insight trace shows 2 sets of 512 centipedes processed in a single thread:
* first one at 4.6ms processes a total of 20,480 nodes (cubes);
* and second one at 10ms processes a total of 40,960 nodes (cubes).

<img width="1137" height="321" src="https://github.com/user-attachments/assets/7023acfd-a14e-45bc-a0c9-d418928a636c" />

## Multi-thread results
ParallelFor over 2 sets of 64 chunks of 8 centepedes each, totalling the same amount of nodes as the single-thread test case.
* first one at 665.4 us processing a total of 20,480 nodes; (6.9 time faster than single-thread)
* and second one at 1ms processes a total of 40,960 nodes. (10 time faster than single-thread)

<img width="1420" height="270" alt="image" src="https://github.com/user-attachments/assets/6b00f20b-c0c3-4710-8fe9-423919f01a3a" />

Note that the `BatchUpdateInstanceData` is part of the overhead of using Unreal's out-of-the box graphic pipeline via the component `InstancedMeshComponent`.



# Sample Code
Centipedes options:

<img width="555" height="409" alt="image" src="https://github.com/user-attachments/assets/3582040a-55b9-4c7e-afaf-3fddd78cbd5c" />

Example of a user algorithm:

<img width="700" alt="image" src="https://github.com/user-attachments/assets/fb62a5ea-6bb1-4bd2-9ce9-59db99b87ed2" />

How to create component types, chunk types and chunks:

<img width="1274" height="914" alt="image" src="https://github.com/user-attachments/assets/17850aa7-f82f-4a94-abb6-29cd60fc009c" />



