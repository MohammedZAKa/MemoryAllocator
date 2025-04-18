#include <stdio.h>
#include <stddef.h>

#define MEMORY_SIZE 100

char memory[MEMORY_SIZE];

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
} Block;

#define BLOCK_SIZE sizeof(Block)

Block* begin = (void*)memory;

void init_memory() {
    begin->size = MEMORY_SIZE - BLOCK_SIZE;
    begin->free = 1;
    begin->next = NULL;
}

void* mmaloc(size_t size) {
    Block* curr = begin;
    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size >= size + BLOCK_SIZE + 1) {
                Block* new_block = (void*)((char*)curr + BLOCK_SIZE + size);
                new_block->size = curr->size - size - BLOCK_SIZE;
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void*)(curr + 1);
        }
        curr = curr->next;
    }
    return NULL;
}

void compaction() {
    Block* curr = begin;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += BLOCK_SIZE + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void ffree(void* ptr) {
    if (!ptr) return;
    Block* block_ptr = (Block*)ptr - 1;
    block_ptr->free = 1;
    compaction();
}

void print_memory_state() {
    Block* curr = begin;
    int i = 0;
    while (curr) {
        printf("Block %d: Size = %zu, Free = %d\n", i++, curr->size, curr->free);
        curr = curr->next;
    }
    printf("----------\n");
}

int main() {
    init_memory();
    printf("Initial memory:\n");
    print_memory_state();

    void* p1 = mmaloc(20);
    void* p2 = mmaloc(20);
    void* p3 = mmaloc(20);

    printf("After allocating  processes:\n");
    print_memory_state();

    ffree(p2);
    printf("After freeing the second process:\n");
    print_memory_state();


    void* p4 = mmaloc(25);
    if (!p4) {
        printf("Failed to allocate new process due to EXTERNAL FRAGMENTATION\n");
    } else {
        printf("Successfully allocated new process of 25 bytes\n");
    }

    print_memory_state();


    ffree(p1);
    ffree(p3);

    printf("After compacting\n");
    print_memory_state();

    void* p5 = mmaloc(50);
    if (p5) {
        printf("Successfully allocated after compacting\n");
    } else {
        printf("failed to allocate\n");
    }

    print_memory_state();

    return 0;
}
