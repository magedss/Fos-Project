# Description

This project is an Operating System (OS) implementation focusing on memory management and CPU scheduling. It includes features such as dynamic allocation using the first-fit algorithm, free allocation, reallocate block functionality, kernel heap management including allocation, freeing, and reallocation of pages, and implementing First-In-First-Out (FIFO) and Least Recently Used (LRU) replacement policies. Additionally, it incorporates user heap management, marking pages that must be allocated, freed, and reallocated to the kernel heap upon page fault occurrences. Furthermore, it includes CPU scheduling mechanisms with FIFO and BSD (Berkeley Software Distribution) scheduling algorithms.

# Features

# 1-Dynamic Allocation by First Fit:

Utilizes the first-fit algorithm to allocate memory dynamically, ensuring efficient memory usage.
# 2-Free Allocation:

Allows for the deallocation of memory blocks, freeing up allocated memory for reuse.
# 3-Reallocate Block:

Provides functionality to reallocate memory blocks, allowing for resizing and relocation as needed.
# 4-Kernel Heap Management:

Manages memory allocation within the kernel space, including allocation, freeing, and reallocation of pages.

# 5-Page Allocation, Freeing, and Reallocation in Kernel Heap:

Facilitates the allocation, freeing, and reallocation of pages within the kernel heap, ensuring optimal memory management.
# 6-FIFO and LRU Replacement Policies:

Implements both FIFO and LRU replacement policies for efficient memory usage and management.
# 7-User Heap Management:

Handles memory allocation in the user space, marking pages that must be allocated, freed, and reallocated to the kernel heap during page fault occurrences.
# 8-CPU Scheduling with FIFO Algorithm:

Implements CPU scheduling using the First-In-First-Out (FIFO) algorithm, where processes are executed in the order they arrive.
# 9-CPU Scheduling with BSD Algorithm:

Incorporates CPU scheduling with the Berkeley Software Distribution (BSD) algorithm, which balances system responsiveness and fairness by dynamically adjusting process priorities.
