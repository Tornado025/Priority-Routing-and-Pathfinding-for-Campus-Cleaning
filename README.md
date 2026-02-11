# Campus Maintenance Management System

An intelligent job scheduling and pathfinding system for campus maintenance operations. Uses priority queues and Dijkstra's algorithm to optimize worker routes and manage cleaning tasks across campus facilities.


## Key Features

- **Priority-based Job Scheduling**: Routine jobs are queued with automatic priority updates; emergency jobs take precedence
- **Optimal Pathfinding**: Dijkstra's algorithm calculates shortest routes between campus locations
- **Worker Management**: Assign jobs to available workers and track their status and task completion
- **Dynamic Job Assignment**: System intelligently routes workers based on location and skill requirements
- **Interactive GUI**: Modern interface (PySide6/Qt) for job and worker management
- **CLI Support**: Command-line interface for system initialization and testing



## Technologies

- **Backend**: C (Dijkstra's algorithm, priority queues, job scheduling)
- **Frontend**: Python with PySide6 (Qt) for GUI
- **Data Structures**: Priority queues, graphs, linked lists for efficient scheduling

## Project Structure

```
├── src/              # C source files for core algorithms
│   ├── main.c          # CLI interface
│   ├── dijkstra.c      # Shortest path algorithm
│   ├── priority_queue.c # Job queue management
│   ├── campus.c        # Campus graph initialization
│   ├── worker.c        # Worker management
│   └── job.c           # Job handling and priority updates
├── include/          # C header files with type definitions
└── gui/              # Python GUI application
    ├── main.py         # Qt GUI interface
    ├── bindings.py     # C library bindings
    └── requirements.txt # Python dependencies
```

## Installation 

To run this project, please follow the below commands:

1. Compile the C program into a shared library:

    ```bash
    make all
    ```

2. Install dependencies for the Python GUI:
    
    ```bash
    cd gui && pip install -r requirements.txt
    ```

3. Run the GUI:

    ```bash
    cd gui && python run gui.py
    ```
