Dynamic memory allocator that interfaces with the Linux kernel to manage page-level memory allocation.
Provided a user-space library that serves dynamic memory requests with efficient bookkeeping, allocation, and deallocation based on requested size.

• Implemented kernel interaction to request and manage memory pages allocated by the Linux kernel.
• Designed a user-space allocator library that maps kernel-allocated pages and serves dynamic memory requests to applications.
• Implemented memory bookkeeping mechanisms to track allocation sizes, free lists, and reuse of memory blocks.
• Handled memory allocation and freeing efficiently to minimize fragmentation and ensure correct lifecycle management.
• Focused on correctness, performance, and safe interaction between kernel and user space.
