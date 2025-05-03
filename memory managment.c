#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MEMORY_SIZE 1000000
#define MAX_PROCESSES 100

typedef struct {
    char name[10];
    int start;
    int size;
} Block;

Block memory[MAX_PROCESSES];
int block_count = 0;

void to_lower_case(char* str) {
    int i;
    for (i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void sort_blocks() {
    int i, j;
    for (i = 0; i < block_count - 1; i++) {
        for (j = i + 1; j < block_count; j++) {
            if (memory[i].start > memory[j].start) {
                Block temp = memory[i];
                memory[i] = memory[j];
                memory[j] = temp;
            }
        }
    }
}


void print_memory() {
    int i, j, current = 0, total_used = 0;
    sort_blocks();

    printf("\n--- Memory Map ---\n");
    for (i = 0; i < block_count; i++) {
        if (memory[i].start > current) {
            printf("Hole: [%d -> %d]\n", current, memory[i].start);
        }
        printf("Process %s: [%d -> %d]\n", memory[i].name, memory[i].start, memory[i].start + memory[i].size);
        total_used += memory[i].size;
        current = memory[i].start + memory[i].size;
    }
    if (current < MEMORY_SIZE) {
        printf("Hole: [%d -> %d]\n", current, MEMORY_SIZE);
    }


    printf("\n[ Memory Diagram ]\n");
    int scale = MEMORY_SIZE / 50;
    for (i = 0; i < 50; i++) {
        int start = i * scale;
        int end = start + scale;
        char label = '.';

        for (j = 0; j < block_count; j++) {
            int bstart = memory[j].start;
            int bend = memory[j].start + memory[j].size;
            if (!(end <= bstart || start >= bend)) {
                label = memory[j].name[0];
                break;
            }
        }
        printf("[%c]", label);
    }
    printf("\n");

    float usage = (float)total_used / MEMORY_SIZE * 100;
    printf("Memory Usage: %.2f%%\n", usage);
}

void compact_memory() {
    int i, current = 0;
    sort_blocks();
    for (i = 0; i < block_count; i++) {
        memory[i].start = current;
        current += memory[i].size;
    }
    printf("Memory compacted.\n");
}


int find_fit(int size, const char* strategy) {
    int best_start = -1;
    int best_size = MEMORY_SIZE + 1;
    int worst_size = -1;
    int current = 0;
    int i;

    sort_blocks();

    for (i = 0; i <= block_count; i++) {
        int hole_start = current;
        int hole_end = (i == block_count) ? MEMORY_SIZE : memory[i].start;
        int hole_size = hole_end - hole_start;

        if (hole_size >= size) {
            if (strcmp(strategy, "first") == 0) {
                return hole_start;
            } else if (strcmp(strategy, "best") == 0 && hole_size < best_size) {
                best_size = hole_size;
                best_start = hole_start;
            } else if (strcmp(strategy, "worst") == 0 && hole_size > worst_size) {
                worst_size = hole_size;
                best_start = hole_start;
            }
        }

        if (i < block_count) {
            current = memory[i].start + memory[i].size;
        }
    }

    return best_start;
}

void request_memory(const char* name, int size, const char* strategy) {
    if (block_count >= MAX_PROCESSES) {
        printf("Maximum process limit reached.\n");
        return;
    }

    if (find_fit(size, strategy) == -1) {
        printf("No suitable space found for process %s.\n", name);
        return;
    }

    Block new_block;
    strcpy(new_block.name, name);
    new_block.size = size;
    new_block.start = find_fit(size, strategy);

    memory[block_count++] = new_block;
    printf("Process %s allocated at [%d -> %d].\n", name, new_block.start, new_block.start + size);
}

void release_memory(const char* name) {
    int i, j;
    for (i = 0; i < block_count; i++) {
        if (strcmp(memory[i].name, name) == 0) {
            for (j = i; j < block_count - 1; j++) {
                memory[j] = memory[j + 1];
            }
            block_count--;
            printf("Process %s released.\n", name);
            return;
        }
    }
    printf("Process %s not found.\n", name);
}

int main() {
    char command[100], pname[10], strategy[10];
    int size;

    printf("Simple Memory Manager (RQ nameOfProcess size strategy  / RL nameOfProcess / C[compaction] / STAT / EXIT)\n");

    while (1) {
        printf("\nEnter command: ");
        fgets(command, sizeof(command), stdin);


        to_lower_case(command);

        if (strncmp(command, "rq", 2) == 0) {
            sscanf(command, "rq %s %d %s", pname, &size, strategy);
            to_lower_case(pname);
            to_lower_case(strategy);
            request_memory(pname, size, strategy);
        } else if (strncmp(command, "rl", 2) == 0) {
            sscanf(command, "rl %s", pname);
            to_lower_case(pname);
            release_memory(pname);
        } else if (strncmp(command, "c", 1) == 0) {
            compact_memory();
        } else if (strncmp(command, "stat", 4) == 0) {
            print_memory();
        } else if (strncmp(command, "exit", 4) == 0) {
            break;
        } else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}
