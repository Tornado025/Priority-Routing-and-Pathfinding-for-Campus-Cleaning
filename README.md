# RVCE Campus Cleaning Scheduler

Priority-based job scheduling system for campus cleaning operations with intelligent worker assignment and pathfinding.


## Technical Details

**Language:** C

**Data Structures:**
- Max-heap priority queue
- Adjacency matrix graph
- Worker hashmap (chaining for collision resolution)

**Algorithms:**
- Dijkstra's shortest path
- Priority-based scheduling with ageing
- FIFO within same priority level

## Files

- `main.c` - CLI interface
- `campus.c` - Core algorithms
- `campus.h` - Data structures
- `launcher.bat` - Windows launcher
- `launcher.sh` - Mac/Linux launcher

## Installation

### Windows

1. Install MinGW: [MinGW-w64](https://www.mingw-w64.org/)
2. Compile and run:
   ```powershell
   gcc -o campus main.c campus.c
   .\campus.exe
   ```
   Or use: `launcher.bat`

### Mac

1. Install Xcode Command Line Tools:
   ```bash
   xcode-select --install
   ```
2. Make launcher executable and run:
   ```bash
   chmod +x launcher.sh
   ./launcher.sh
   ```
   Or compile manually:
   ```bash
   gcc -o campus main.c campus.c
   ./campus
   ```