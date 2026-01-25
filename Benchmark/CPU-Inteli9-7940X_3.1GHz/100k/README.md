# Benchmark: 100,000 nodes divided by 1000 centipedes.
## Setup
<img width="300" height="600" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/CPU-Z-CPU.png" /><img width="300" height="600" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/CPU-Z-RAM.png" /><img width="300" height="600" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/CPU-Z-GPU.png" />
<img width="600" height="600" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/CPU-Inteli9-7940X_3.1GHz.png" />

# Centipedes Update Time
## 1 chunks of 1000 centipedes: 17.45ms median (similar to single-thread)
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly1x1000x100.png" />

## 10 chunks of 100 centipedes: 5.63ms median
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly10x100x100.png" />

## 100 chunks of 10 centipedes: 2.78ms median
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly100x10x100.png" />

## 1000 chunks of 1 centipedes: 2.54ms median
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly1000x1x100.png" />

## Chunk Count Versus Chunk Size
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/ChunkCountVsSize.png" />

# Frame
## 1 chunks of 1000 centipedes
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly1x1000x100-Frame.png" />

## 10 chunks of 100 centipedes
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly10x100x100-Frame.png" />

## 100 chunks of 10 centipedes
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly100x10x100-Frame.png" />

## 1000 chunks of 1 centipedes
<img width="1200" height="300" src="https://github.com/StephanieRct/UE5PNCDemo/blob/main/Benchmark/CPU-Inteli9-7940X_3.1GHz/100k/InsightFly1000x1x100-Frame.png" />
