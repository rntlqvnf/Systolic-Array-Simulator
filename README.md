# Systolic-Array-Simulator

## Instruction

- RHM src dst N (N cycle)
  - Read N vectors from Host Memory to Unified Buffer

- WHM src dst N (N cycle)
  - Write N vectors from Unified Buffer to Host Memory

- RW.{U} addr N (N * N / 64 cycle)
  - Read N weight vectors from DRAM to Weight FIFO
  - U option means Unfold (convolution lowering)

- MMC.{SOU} src dst N (If MM, 2 * N -1 cycle. If conv, 2 * (weight_size + N – 1)^2 - 1)
  - Matrix Multiply / Convolution with N input vectors
  - S option means Switch (Switch MMU weight buffer)
  - O option means Overwrite (Overwrite result on accumulator)
  - U option means Unfold (convolution lowering)

- MMC.{SOU}{:CZ} src dst N start end 
  - Crop with Zeroing
  - Crop image leaving pixel in the range of start to end

- MMC.{SOU}{:CDI}{:CDD} src dst N start end value
  - Color distort Increase / Decrease
  - Incease or decrease constant value on each pixel value from start to end

- MMC.{SOU}{:CDR} src dst N start end range
  - Color distort Random value
  - Randomly pick value between +range to -range and add

- MMC.{SOU}{:RC} src dst N
  - Random cutout
  - Randomly choose cutout area and make area 0

- MMC.{SOU}{:FLR}{:FUD} src dst N line
  - Flip left right / up down
  - Flip image based on line

- ACT.{FP} src dst N {M} (N cycle)
  - Activate N vectors (ReLU and something) and write to Unified Buffer (M is kernel matrix size)
  - F option means Fold (Fold unfolded result matrix)
  - P option means Pooling

- NOP

- HLT
