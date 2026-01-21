#include "campus.h"
#include "priority_queue.h"
#include <stdio.h>
#include <string.h>

void print_header() {
    printf("\n");
    printf("  #############################################\n");
    printf("  #    RVCE CAMPUS CLEANING SCHEDULER CLI     #\n");
    printf("  #      Priority-based Cleaning System      #\n");
    printf("  #############################################\n");
}

void print_menu() {
    printf("\n  ========== MAIN MENU ==========\n");
    printf("  [1] Initialize Routine Jobs\n");
    printf("  [2] ADD EMERGENCY JOB\n");
    printf("  [3] Execute Next Job\n");
    printf("  [4] View Job Queue\n");
    printf("  [5] View Worker Status\n");
    printf("  [6] Exit\n");
    printf("  > ");
}

void view_worker_status() {
    printf("\n  ========== WORKER STATUS ==========\n");
    if (worker_count == 0) {
        printf("  No workers registered yet.\n");
        return;
    }
    
    Worker* workers_array[MAX_WORKERS];
    int count;
    get_all_workers(workers_array, &count);
    
    for (int i = 0; i < count; i++) {
        printf("  Worker %d: %s\n", workers_array[i]->worker_id, workers_array[i]->name);
        printf("    Location: %s\n", room_names[workers_array[i]->current_room_id]);
        printf("    Status: %s\n", workers_array[i]->is_busy ? "BUSY" : "AVAILABLE");
        printf("    Jobs Completed: %d\n", workers_array[i]->jobs_completed);
        printf("\n");
    }
}

int main() {
    PriorityQueue job_queue;
    initPQ(&job_queue);
    
    init_system(); 

    int choice;
    int routine_initialized = 0;
    
    print_header();
    printf("  [INFO] System initialized with Graph Map.\n");
    printf("  [INFO] Please initialize routine jobs to begin.\n");

    while(1) {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            choice = -1;
        }

        switch(choice) {
            case 1: {
                if (routine_initialized) {
                    printf("\n  [!] Routine jobs already initialized.\n");
                    printf("  [INFO] Routine jobs cycle automatically after completion.\n");
                } else {
                    init_routine_jobs(&job_queue);
                    routine_initialized = 1;
                }
                break;
            }
            
            case 2: {
                int r_id, prio;
                char cause[100];
                
                printf("\n  === EMERGENCY JOB ===\n");
                printf("  Rooms: 0:Reception, 1:Lobby, 2:Office, 3:Cafe, 4:Lab, 5:Class\n");
                printf("  Room ID (0-5): "); scanf("%d", &r_id);
                printf("  Priority (9-10 for emergencies): "); scanf("%d", &prio);
                printf("  Cause/Description: ");
                getchar();
                fgets(cause, sizeof(cause), stdin);
                cause[strcspn(cause, "\n")] = 0;
                
                if(r_id >= 0 && r_id < 6 && prio >= 9 && prio <= 10) {
                    add_emergency_job(&job_queue, r_id, prio, cause);
                } else {
                    printf("  [!] Invalid input. Emergency priority must be 9-10.\n");
                }
                break;
            }
            
            case 3: {
                update_job_priorities(&job_queue);
                
                Request req = dequeue(&job_queue);
                
                if (req.room_id == -1) {
                    printf("\n  [INFO] Queue is empty.\n");
                } else {
                    printf("\n  ==========================================\n");
                    if (req.is_emergency) {
                        printf("  [!!!] EMERGENCY: %s\n", room_names[req.room_id]);
                        printf("  [!!!] Cause: %s\n", req.cause);
                    } else {
                        printf("  [>>>] Routine Job: %s\n", room_names[req.room_id]);
                        printf("  [>>>] Task: %s\n", req.cause);
                    }
                    printf("  Priority: Base=%d, Effective=%d (waited %d cycles)\n", 
                           req.priority, req.effective_priority, req.wait_count);
                    
                    int worker_id = find_best_available_worker(req.room_id);
                    
                    if (worker_id == -1) {
                        printf("  [!] No available workers (all busy). Job re-queued.\n");
                        enqueue(&job_queue, req.room_id, req.priority, req.is_emergency, req.cause);
                    } else {
                        Worker *assigned = get_worker(worker_id);
                        printf("  [ASSIGNED] Worker: %s (ID %d)\n", assigned->name, assigned->worker_id);
                        
                        printf("  [MOVING] From %s to %s...\n", 
                               room_names[assigned->current_room_id], 
                               room_names[req.room_id]);
                        dijkstra(assigned->current_room_id, req.room_id);
                        
                        assigned->current_room_id = req.room_id;
                        assigned->jobs_completed++;
                        
                        printf("  [DONE] Worker arrived and completed job at %s.\n", room_names[req.room_id]);
                        printf("  [STATS] %s: Jobs Completed=%d\n", 
                               assigned->name, assigned->jobs_completed);
                        
                        if (req.is_emergency) {
                            printf("  [RESOLVED] Emergency cleared.\n");
                        } else {
                            printf("  [CYCLE] Routine job re-queued for next cycle.\n");
                            enqueue(&job_queue, req.room_id, req.priority, 0, req.cause);
                        }
                    }
                    printf("  ==========================================\n");
                }
                break;
            }
            
            case 4: {
                printf("\n  ========== JOB QUEUE ==========\n");
                if (job_queue.size == 0) {
                    printf("  (Empty)\n");
                } else {
                    for(int i=0; i<job_queue.size; i++) {
                        Request r = job_queue.data[i];
                        printf("  %d. %s%s [Base P:%d, Eff P:%d, Wait:%d | %s]\n", 
                            i+1,
                            r.is_emergency ? "[EMERGENCY] " : "",
                            room_names[r.room_id], 
                            r.priority,
                            r.effective_priority,
                            r.wait_count,
                            r.cause);
                    }
                }
                break;
            }
            
            case 5: {
                view_worker_status();
                break;
            }
            
            case 6:
                printf("\n  [EXIT] Shutting down system. Goodbye!\n");
                return 0;
                
            default: 
                printf("\n  [!] Invalid Choice.\n");
        }
    }
    return 0;
}