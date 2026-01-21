#include "campus.h"
#include "sample_data.h"

void init_system() {
    addSampleRooms(room_names, graph);
    addSampleWorkers(&worker_count);
}
