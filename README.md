# memtest
malloc test tool

This tool allocates a bunch of memory and wastes some cpu cycles.  It is
intended to run as root and ideally one allocates more memory than physically
available.  That will dig into swap and potentially create interesting
effects.

Do not run this on a production machine.  Chances of crashing and killing
running processes are high.

# Usage
-a alloc only, does not memset memory
-b <number> number of bytes allocated per malloc
-c waste some cpu cycles after allocating memory
-f <number> number of forked processes
-m <number> number of bytes allocated across all child processes
