#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>


#define u8 uint8_t
#define u16 uint16_t
#define STACK_SIZE 32
#define HEAP_SIZE 256
#define HEADER 8

static u16 IN_PROCESS;

typedef struct virtualMemory
{

    u8 stack[STACK_SIZE];
    char **unmapped;
    u8 heap[HEAP_SIZE];

    struct
    {
        
        char **data;
        char **bss;
        char *text;
        
    } data_t;

}virtualMemory_t;

typedef struct entity
{
    
    u8 *ptr;
    u16 *size;
    
} entity_t;

entity_t LIST[40];

void TEST()
{
    
    printf("OUR LIST\n");
    for (unsigned i = 0; i < IN_PROCESS; i++)
    {
        printf("Data + HEADER. [%p]. Memory of our heap free: [%u]\n", LIST[i].ptr, LIST[i].size);
    }

    printf("Entities in use: [%d]\n", (sizeof(LIST) / sizeof(LIST[0]) - IN_PROCESS));
    
}

entity_t* newEntity(size_t size)
{
    
    if (LIST[0].ptr == NULL && LIST[0].size == 0)
    {
        
        static virtualMemory_t vm;
        LIST[0].ptr = vm.heap;
        LIST[0].size = (uint16_t *) HEAP_SIZE;
        IN_PROCESS++;
        TEST();
        
    }

    entity_t* best = LIST;

    for (unsigned i = 0; i < IN_PROCESS; i++)
    {
        
        if (LIST[i].size >= size && LIST[i].size < best->size)
        {
            best = &LIST[i];
        }
        
    }

    return best;
}

void* a_malloc(size_t size)
{
    
    assert(size <= HEAP_SIZE);

    size += HEADER;

    entity_t* e = newEntity(size);

    u8* start = e->ptr;
    u8* userPointer = start + HEADER;
    *start = size;

    e->ptr += size;
    e->size -= size;

    assert(e->size >= 0);

    TEST();
    return userPointer;
    
}

void a_free(void* ptr)
{
    
    u8* start = (u8*)ptr - HEADER;

    LIST[IN_PROCESS].ptr = &(*start);
    LIST[IN_PROCESS].size = (uint8_t *)((u8 *) ptr - HEADER);
    IN_PROCESS++;
    TEST();
    
}


void USED()
{
    
    typedef struct foo
    {
        
        int dataTypeFirst;
        int dataTypeSecond;
        
    }foo_t;

    foo_t* foo;
    char* bar;
    int* standart;

    foo = a_malloc(sizeof(foo_t));
    bar = a_malloc(5);
    standart = a_malloc(sizeof(int));

    foo->dataTypeFirst = 5;
    foo->dataTypeSecond = 10;

    strcpy(bar, "bar");
    memcpy(standart, &foo->dataTypeFirst, sizeof(int));

    printf("Address: [%p], data: [%d] [%d]\n", foo, foo->dataTypeFirst, foo->dataTypeSecond);
    printf("Address: [%p], data: [%s] \n", bar, bar);
    printf("Address: [%p], data: [%d] \n", standart, *standart);

    a_free(foo);
    a_free(bar);

    char *other = a_malloc(4);
    strcpy(other, "other");
    printf("Address: [%p], data: [%s] \n", other, other);

};


int main(int argc, char** argv)
{
    
    USED();
    return 0;
    
}
