import ctypes
from pathlib import Path

lib_path = Path(__file__).parent.parent / "libcampus.dylib"
if not lib_path.exists():
    lib_path = Path(__file__).parent.parent / "libcampus.so"
if not lib_path.exists():
    lib_path = Path(__file__).parent.parent / "libcampus.dll"

lib = ctypes.CDLL(str(lib_path))

MAX_NODES = 30
MAX_WORKERS = 10
HASH_SIZE = 10
MAX_REQUESTS = 50


class Worker(ctypes.Structure):
    _fields_ = [
        ("worker_id", ctypes.c_int),
        ("current_room_id", ctypes.c_int),
        ("name", ctypes.c_char * 20),
        ("is_occupied", ctypes.c_int),
        ("is_busy", ctypes.c_int),
        ("jobs_completed", ctypes.c_int),
        ("next", ctypes.c_void_p),
    ]


class Request(ctypes.Structure):
    _fields_ = [
        ("room_id", ctypes.c_int),
        ("priority", ctypes.c_int),
        ("effective_priority", ctypes.c_int),
        ("arrival_time", ctypes.c_int),
        ("wait_count", ctypes.c_int),
        ("is_emergency", ctypes.c_int),
        ("cause", ctypes.c_char * 100),
    ]


class PriorityQueue(ctypes.Structure):
    _fields_ = [
        ("data", Request * MAX_REQUESTS),
        ("size", ctypes.c_int),
        ("counter", ctypes.c_int),
    ]


lib.init_system.argtypes = []
lib.init_system.restype = None

lib.initPQ.argtypes = [ctypes.POINTER(PriorityQueue)]
lib.initPQ.restype = None

lib.enqueue.argtypes = [
    ctypes.POINTER(PriorityQueue),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_char_p,
]
lib.enqueue.restype = None

lib.dequeue.argtypes = [ctypes.POINTER(PriorityQueue)]
lib.dequeue.restype = Request

lib.add_worker.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_int]
lib.add_worker.restype = None

lib.get_worker.argtypes = [ctypes.c_int]
lib.get_worker.restype = ctypes.POINTER(Worker)

lib.find_best_available_worker.argtypes = [ctypes.c_int]
lib.find_best_available_worker.restype = ctypes.c_int

lib.init_routine_jobs.argtypes = [ctypes.POINTER(PriorityQueue)]
lib.init_routine_jobs.restype = None

lib.add_emergency_job.argtypes = [
    ctypes.POINTER(PriorityQueue),
    ctypes.c_int,
    ctypes.c_int,
    ctypes.c_char_p,
]
lib.add_emergency_job.restype = None

lib.update_job_priorities.argtypes = [ctypes.POINTER(PriorityQueue)]
lib.update_job_priorities.restype = None

lib.get_all_workers.argtypes = [
    ctypes.POINTER(ctypes.POINTER(Worker)),
    ctypes.POINTER(ctypes.c_int),
]
lib.get_all_workers.restype = None

graph = (ctypes.c_int * MAX_NODES * MAX_NODES).in_dll(lib, "graph")
room_names = (ctypes.c_char * 20 * MAX_NODES).in_dll(lib, "room_names")
worker_count = ctypes.c_int.in_dll(lib, "worker_count")
