#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "pqueue.h"
#include "event.h"

#define NUM_PARAMS 12
#define LINE_LENGTH 20

unsigned int conf_vals[NUM_PARAMS] = {};
const char *conf_types[NUM_PARAMS] = {
    "SEED",
    "INIT_TIME",
    "FIN_TIME",
    "ARRIVE_MIN",
    "ARRIVE_MAX",
    "CPU_MIN",
    "CPU_MAX",
    "QUIT_PROB",
    "DISK1_MIN",
    "DISK1_MAX",
    "DISK2_MIN",
    "DISK2_MAX"
};

enum config {
    SEED,
    INIT_TIME,
    FIN_TIME,
    ARRIVE_MIN,
    ARRIVE_MAX,
    CPU_MIN,
    CPU_MAX,
    QUIT_PROB,
    DISK1_MIN,
    DISK1_MAX,
    DISK2_MIN,
    DISK2_MAX
};

int main() {

    /*
        BEGIN INIT CONFIG
    */
    FILE* fp;
    fp = fopen("sim.cfg", "r");
    if (fp == NULL) {
        printf("Could not read sim.cfg!\n");
        exit(1);
    }

    char search_str[LINE_LENGTH];
    char line[LINE_LENGTH];
    int i;
    while (fgets(line, LINE_LENGTH, fp) != NULL) {
        for (i = 0; i < NUM_PARAMS; i++) {
            strcpy(search_str, conf_types[i]);
            strcat(search_str, "%d\n");
            sscanf(line, search_str, &conf_vals[i]);
        }
    }
    fclose(fp);

    printf("Values for simulation: \n");
    for (i = 0; i < NUM_PARAMS; i++) {
        printf("%s : %d\n", conf_types[i], conf_vals[i]);
    }
    srand(conf_vals[SEED]);

    pqueue* event_queue = create_p_queue();
    queue* cpu_fifo = create_queue();
    queue* disk1_fifo = create_queue();
    queue* disk2_fifo = create_queue();

    /*
        END INIT CONFIG
    */

    sim_event* e_begin = create_event(SIM_BEGIN, -1, conf_vals[INIT_TIME]);
    sim_event* e_end = create_event(SIM_EXITS, -2, conf_vals[FIN_TIME]);
    push_p_queue(event_queue, e_begin);
    push_p_queue(event_queue, e_end);

    /*
        BEGIN STAT VARS
    */
    unsigned int stat_total_time = 0;
    unsigned int stat_cpu_max = 0;
    unsigned int stat_cpu_avg = 0;
    unsigned int stat_cpu_busy = 0;
    unsigned int stat_cpu_strt = 0;
    unsigned int stat_cpu_res_max = 0;
    unsigned int stat_cpu_res_avg = 0;
    unsigned int stat_cpu_res_cnt = 0;
    unsigned int stat_disk1_max = 0;
    unsigned int stat_disk1_avg = 0;
    unsigned int stat_disk1_busy = 0;
    unsigned int stat_disk1_strt = 0;
    unsigned int stat_disk1_res_max = 0;
    unsigned int stat_disk1_res_avg = 0;
    unsigned int stat_disk1_res_cnt = 0;
    unsigned int stat_disk2_max = 0;
    unsigned int stat_disk2_avg = 0;
    unsigned int stat_disk2_busy = 0;
    unsigned int stat_disk2_strt = 0;
    unsigned int stat_disk2_res_max = 0;
    unsigned int stat_disk2_res_avg = 0;
    unsigned int stat_disk2_res_cnt = 0;
    unsigned int stat_finished_jobs = 0;
    /*
        END STAT VARS
    */

    /*
        BEGIN SIM LOOP
    */
    unsigned int id_counter = 0;
    unsigned int sim_time;
    unsigned int cpu_busy = 0;
    unsigned int disk1_busy = 0;
    unsigned int disk2_busy = 0;
    unsigned int break_sim = 0;
    while (event_queue->size != 0) {
        sim_event* e_popped = pop_p_queue(event_queue);
        // HANDLE EVENTS
        switch(e_popped->type) {
            case SIM_BEGIN: {
                sim_time = e_popped->time;
                printf("\n\n%d -\tSimulation started.\n", sim_time);
                sim_event* e_new = create_event(JOB_ARRIVE, id_counter, conf_vals[ARRIVE_MIN]);
                push_p_queue(event_queue, e_new);
                id_counter++;
                break;
            }
            case SIM_EXITS: {
                sim_time = e_popped->time;
                break_sim = 1;
                printf("%d -\tSimulation ended.\n", sim_time);
                break; 
            }
            case JOB_ARRIVE: {
                sim_time = e_popped->time;
                printf("%d -\tJob #%d arrived in sim.\n", sim_time, e_popped->id);
                // Create New Job
                int new_arrival = rand() % (conf_vals[ARRIVE_MAX] + 1 - conf_vals[ARRIVE_MIN]) + conf_vals[ARRIVE_MIN] + sim_time;
                sim_event* e_new = create_event(JOB_ARRIVE, id_counter, new_arrival);
                push_p_queue(event_queue, e_new);
                id_counter++;
                // Send popped job to CPU fifo
                e_popped->srv_arr = sim_time;
                push_queue(cpu_fifo, e_popped);
                break; 
            }
            case JOB_EXITS: {
                sim_time = e_popped->time;
                stat_finished_jobs++;
                printf("%d -\tJob #%d left sim.\n", sim_time, e_popped->id);
                break; 
            }
            case JOB_STARTS_CPU: {
                sim_time = e_popped->time;
                cpu_busy = 1;
                stat_cpu_strt = sim_time;
                int job_done = rand() % (conf_vals[CPU_MAX] + 1 - conf_vals[CPU_MIN]) + conf_vals[CPU_MIN] + sim_time;
                printf("%d -\tJob #%d started at CPU.\n", sim_time, e_popped->id);
                e_popped->type = JOB_FINISH_CPU;
                e_popped->time = job_done;
                push_p_queue(event_queue, e_popped);
                break; 
            }
            case JOB_FINISH_CPU: {
                sim_time = e_popped->time;
                cpu_busy = 0;
                stat_cpu_busy += sim_time - stat_cpu_strt;
                stat_cpu_res_cnt++;
                int res_time = sim_time - e_popped->srv_arr;
                stat_cpu_res_avg += res_time;
                if (stat_cpu_res_max < res_time)
                    stat_cpu_res_max = res_time;
                printf("%d -\tJob #%d finished at CPU.\n", sim_time, e_popped->id);
                // Determine whether job finishes after CPU
                int rnd_val = rand() % (10 + 1 - 1) + 1;
                if (rnd_val <= conf_vals[QUIT_PROB]) {
                    e_popped->type = JOB_EXITS;
                    push_p_queue(event_queue, e_popped);
                } else {
                    // Send job to Disk
                    e_popped->srv_arr = sim_time;
                    if (disk1_fifo->size + disk1_busy < disk2_fifo->size + disk2_busy) {
                        push_queue(disk1_fifo, e_popped);
                    } else if (disk1_fifo->size + disk1_busy > disk2_fifo->size + disk2_busy) {
                        push_queue(disk2_fifo, e_popped);
                    } else {
                        rnd_val = rand() % 2;
                        if (rnd_val == 0) {
                            push_queue(disk1_fifo, e_popped);
                        } else {
                            push_queue(disk2_fifo, e_popped);
                        }
                    }
                }
                break; 
            }
            case JOB_STARTS_DISK1: {
                sim_time = e_popped->time;
                disk1_busy = 1;
                stat_disk1_strt = sim_time;
                int job_done = rand() % (conf_vals[DISK1_MAX] + 1 - conf_vals[DISK1_MIN]) + conf_vals[DISK1_MIN] + sim_time;
                printf("%d -\tJob #%d started at Disk1.\n", sim_time, e_popped->id);
                e_popped->type = JOB_FINISH_DISK1;
                e_popped->time = job_done;
                push_p_queue(event_queue, e_popped);
                break; 
            }
            case JOB_FINISH_DISK1: {
                sim_time = e_popped->time;
                disk1_busy = 0;
                stat_disk1_busy += sim_time - stat_disk1_strt;
                stat_disk1_res_cnt++;
                int res_time = sim_time - e_popped->srv_arr;
                stat_disk1_res_avg += res_time;
                if (stat_disk1_res_max < res_time)
                    stat_disk1_res_max = res_time;
                printf("%d -\tJob #%d finished at Disk1.\n", sim_time, e_popped->id);
                // Send popped job to CPU fifo
                e_popped->srv_arr = sim_time;
                push_queue(cpu_fifo, e_popped);
                break; 
            }
            case JOB_STARTS_DISK2: {
                sim_time = e_popped->time;
                disk2_busy = 1;
                stat_disk2_strt = sim_time;
                int job_done = rand() % (conf_vals[DISK2_MAX] + 1 - conf_vals[DISK2_MIN]) + conf_vals[DISK2_MIN] + sim_time;
                printf("%d -\tJob #%d started at Disk2.\n", sim_time, e_popped->id);
                e_popped->type = JOB_FINISH_DISK2;
                e_popped->time = job_done;
                push_p_queue(event_queue, e_popped);
                break; 
            }
            case JOB_FINISH_DISK2: {
                sim_time = e_popped->time;
                disk2_busy = 0;
                stat_disk2_busy += sim_time - stat_disk2_strt;
                stat_disk2_res_cnt++;
                int res_time = sim_time - e_popped->srv_arr;
                stat_disk2_res_avg += res_time;
                if (stat_disk2_res_max < res_time)
                    stat_disk2_res_max = res_time;
                printf("%d -\tJob #%d finished at Disk2.\n", sim_time, e_popped->id);
                // Send popped job to CPU fifo
                e_popped->srv_arr = sim_time;
                push_queue(cpu_fifo, e_popped);
                break; 
            }
        }

        if (break_sim) {
            break;
        }

        stat_total_time++;
        stat_cpu_avg += cpu_fifo->size;
        if (cpu_fifo->size > stat_cpu_max)
            stat_cpu_max = cpu_fifo->size;
        stat_disk1_avg += disk1_fifo->size;
        if (disk1_fifo->size > stat_disk1_max)
            stat_disk1_max = disk1_fifo->size;
        stat_disk2_avg += disk2_fifo->size;
        if (disk2_fifo->size > stat_disk2_max)
            stat_disk2_max = disk2_fifo->size;

        // PROCESS FIFOs
        // CPU
        if (cpu_fifo->size > 0 && !cpu_busy) {
            e_popped = pop_queue(cpu_fifo);
            e_popped->type = JOB_STARTS_CPU;
            e_popped->time = sim_time;
            push_p_queue(event_queue, e_popped);
        }
        if (disk1_fifo->size > 0 && !disk1_busy) {
            e_popped = pop_queue(disk1_fifo);
            e_popped->type = JOB_STARTS_DISK1;
            e_popped->time = sim_time;
            push_p_queue(event_queue, e_popped);
        }
        if (disk2_fifo->size > 0 && !disk2_busy) {
            e_popped = pop_queue(disk2_fifo);
            e_popped->type = JOB_STARTS_DISK2;
            e_popped->time = sim_time;
            push_p_queue(event_queue, e_popped);
        }

    }
    /*
        END EVENT LOOP
    */

    /*
        START STATS
    */
    int util_total_time = conf_vals[FIN_TIME] - conf_vals[INIT_TIME];
    printf("\n\t\t=Statistics=\n");
    printf("Max Size Queues:\n");
    printf("\tCPU FIFO:\t%d\n", stat_cpu_max);
    printf("\tDISK1 FIFO:\t%d\n", stat_disk1_max);
    printf("\tDISK2 FIFO:\t%d\n", stat_disk2_max);
    printf("Avg Size of Queues:\n");
    printf("\tCPU FIFO:\t%d\n", stat_cpu_avg / stat_total_time);
    printf("\tDISK1 FIFO:\t%d\n", stat_disk1_avg / stat_total_time);
    printf("\tDISK2 FIFO:\t%d\n", stat_disk2_avg / stat_total_time);
    printf("Utilization of Servers:\n");
    printf("\tCPU FIFO:\t%f%%\n", (float)stat_cpu_busy / util_total_time * 100);
    printf("\tDISK1 FIFO:\t%f%%\n", (float)stat_disk1_busy / util_total_time * 100);
    printf("\tDISK2 FIFO:\t%f%%\n", (float)stat_disk2_busy / util_total_time * 100);
    printf("Response Times:\n");
    printf("\tCPU FIFO Avg:\t%d\n", stat_cpu_res_avg / stat_cpu_res_cnt);
    printf("\tCPU FIFO Max:\t%d\n", stat_cpu_res_max);
    printf("\tDisk1 FIFO Avg:\t%d\n", stat_disk1_res_avg / stat_disk1_res_cnt);
    printf("\tDisk1 FIFO Max:\t%d\n", stat_disk1_res_max);
    printf("\tDisk2 FIFO Avg:\t%d\n", stat_disk2_res_avg / stat_disk2_res_cnt);
    printf("\tDisk2 FIFO Max:\t%d\n", stat_disk2_res_max);

    printf("Throughput:\n");
    printf("\t%f Jobs/time_unit\n", (float)stat_finished_jobs / util_total_time);
    /*
        END STATS
    */

    /*
        CLEANUP
    */

    sim_event* e;
    printf("Jobs left in FIFO's:\n");
    printf("\tCPU:\t%d\n", cpu_fifo->size);
    printf("\tDisk1:\t%d\n", disk1_fifo->size);
    printf("\tDisk2:\t%d\n", disk2_fifo->size);
    while ((e = pop_p_queue(event_queue)) != NULL)
        free(e);
    while ((e = pop_queue(cpu_fifo)) != NULL)
        free(e);
    while ((e = pop_queue(disk1_fifo)) != NULL)
        free(e);
    while ((e = pop_queue(disk2_fifo)) != NULL)
        free(e);

    free(event_queue);
    free(cpu_fifo);
    free(disk1_fifo);
    free(disk2_fifo);
    
    return 0;
}
