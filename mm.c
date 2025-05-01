#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For getpagesize()
#include <sys/mman.h> // For mmap()
#include "mm.h"

static vm_page_for_families_t* first_vm_page_for_families = NULL;
static size_t SYSTEM_PAGE_SIZE = 0;

void mm_init() {
    SYSTEM_PAGE_SIZE = getpagesize();
}

// Request virtual memory page from the kernel
void* mm_get_new_vm_page_from_kernel(int units) {
    char* vm_page = mmap(
        NULL,
        units * SYSTEM_PAGE_SIZE,
        PROT_READ | PROT_WRITE | PROT_EXEC,
        MAP_ANONYMOUS | MAP_PRIVATE,
        -1, 0  // -1 because no file descriptor is used
    );

    if (vm_page == MAP_FAILED) {
        perror("Error: vm page allocation failed");
        return NULL;
    }

    memset(vm_page, 0, units * SYSTEM_PAGE_SIZE);
    return (void*)vm_page;
}

// Return pages to the kernel
void mm_return_vm_page_to_kernel(void* vm_page, int units) {
    if (munmap(vm_page, units * SYSTEM_PAGE_SIZE) != 0) {
        perror("Error: vm page deallocation failed");
    }
}
//request a new data VM page from kernel and add it to front of linked list of VM pages of given family
vm_page_t* allocate_vm_page(vm_page_family_t *vm_page_family){
	vm_page_t *vm_page = mm_get_new_vm_page_from_kernel(1);
	
	/*Initialize lower most Meta block of the new VM page*/
	MARK_VM_PAGE_EMPTY(vm_page);
	vm_page->block_meta_data.block_size = mm_max_page_allocatable_memory(1);
	vm_page->block_meta_data.offset = offset_off(vm_page_t, block_meta_data);
	vm_page->next = NULL;
	vm_page->prev = NULL;
	
	/*Set the back pointer to page family*/
	vm_page->pg_family = vm_page_family;
	
	/*Inster page into doubly linked list*/
	/*If it is a first VM data page for a given page family*/
	if(!vm_page_family->first_page){
		vm_page_family->first_page = vm_page;
		return vm_page;
	}
	
	/*Insert new VM page to the head of  the linked list*/
	vm_page->next = vm_page_family->first_page;
	vm_page_family->first_page->prev = vm_page
	vm_page_family->first_page = vm_page;
	
	return vm_page;
}


//delete and free(return to kernel) data VM page
void mm_vm_page_delete_and_free(vm_page_t *vm_page){
	vm_page_family_t *vm_page_family = vm_page->pg_family;
	
	/*If hte page being deleted is the head of the linked list*/
	if(vm_page_family->first_page == vm_page){
		vm_page_family->first_page = vm_page->next;
		if(vm_page->next)
			vm_page->next->prev = NULL;
		vm_page->next = NULL;
		vm_page->prev = NULL;
		mm_return_vm_page_to_kernel((void *)vm_page, 1);
		return;
	}
	/*If we are deleting the page from middle or end of linked list*/
	if(vm_page->next)
		vm_page->next->prev = vm_page->prev;
	vm_page->prev->next = vm_page->next;
	mm_return_vm_page_to_kernel((void *)vm_page, 1);	
}

void mm_instantiate_new_page_family(
    char *struct_name,
    uint32_t struct_size)
{
    vm_page_family_t* vm_page_family_curr = NULL;
    vm_page_for_families_t* new_vm_page_for_families = NULL;
    if (struct_size > SYSTEM_PAGE_SIZE) {
        printf("Error : %s() structure %s size excceds system page size\n",__FUNCTION__, struct_name);
        return;
    }

    //first time init
    if (!first_vm_page_for_families) {
        first_vm_page_for_families = (vm_page_for_families_t*)mm_get_new_vm_page_from_kernel(1);
        first_vm_page_for_families->next = NULL;
        strcnpy(first_vm_page_for_families->vm_page_family[0].struct_name,
            struct_name, MM_MAX_STRUCT_SIZE);
        first_vm_page_for_families->vm_page_family[0].struct_size = struct_size;
    }

    uint32_t count = 0;
    ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_for_families, vm_page_family_curr) {
        //check if application is trying to register same family twice
        if (strncmp(vm_page_family_curr->struct_name,
            struct_name, MM_MAX_STRUCT_SIZE) != 0) {
            count++;
            continue;
        }
        assert(0);
    }ITERATE_PAGE_FAMILIES_END(first_vm_page_for_families, vm_page_family_curr)
    //check if current page is full
    if (count == MAX_FAMILIES_PER_VM_PAGE)
    {
        new_vm_page_for_families = (vm_page_for_families_t*)mm_get_new_vm_page_from_kernel(1);
        new_vm_page_for_families->next = first_vm_page_for_families;
        first_vm_page_for_families = new_vm_page_for_families;
        vm_page_family_curr = &first_vm_page_for_families->vm_page_family[0];
    }

    strncpy(vm_page_family_curr->struct_name, struct_name, MM_MAX_STRUCT_SIZE);
    vm_page_family_curr->struct_size = struct_size;
    vm_page_family_curr->first_page = NULL;
}   

//merge second block into first block
static mm_union_free_blocks(block_meta_data_t *first, block_meta_data_t *second)
{
	assert(first->is_free == MM_TRUE &&
			second->is_free == MM_TRUE );
			
	first->next_block = second->next_block;
	first->block_size += sizeof(block_meta_data_t) + second->block_size
	
	if(second->next_block)
		second->next_block->prev_block = first;	
}

// Return TRUE if VM_PAGE has no data block assigned to it
vm_bool_t mm_is_vm_page_empty(vm_page_t *vm_page->block_meta_data.next_block == NULL &&)
{
	if(vm_page->block_meta_data.next_block == NULL &&
	   vm_page->block_meta_data.prev_block == NULL &&
	   vm_page->block_meta_data.is_free == MM_TRUE)
	{
		   return MM_TRUE;
	}
	   return MM_FALSE;
}

//return size of free data block of an Empty vm page
static inline uint32_t mm_max_page_allocatable_memory(int units)
{
	return (uint32_t)((SYSTEM_PAGE_SIZE * units) - offset_of(vm_page_t, page_memory))
}



void mm_print_registered_page_families(){
    vm_page_family_t* vm_page_family_curr = NULL;
    vm_page_for_families_t* vm_page_for_families_cur = NULL;
	
	for(vm_page_for_families_cur = first_vm_page_for_families;
		vm_page_for_families_cur; vm_page_for_families_cur->next)
	{
		ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_families_cur, vm_page_family_curr)
		{
				printf("Page family : %s, size : %u\n",
				vm_page_family_curr->struct_name, vm_page_family_curr->struct_size);
		}
		ITERATE_PAGE_FAMILIES_END(vm_page_for_families_cur, vm_page_family_curr);
	}
}


//Penultimate: Data block which is immediately below data which is going under going split is called Penultimate
//Ante - Penultimate: Data block immediately after the free data block after split is called  Ante - Penultimate
//GLthreads provide advantage over traditional doubly linked list - traditional linked list has data tightly coupled with linked list.
//Glue based linked list sticks the data to linked list. handeling the data becomes easy. We can easily remove the data and add, remove, move to other place. Which is difficult in traditional linked list
//given pointer to GL node we can get start address of structure by using offset_of Macro

/*traditional LL:
struct emp_{
	char name[100];
	int ID;
}emp_t;

struct doubly_LL_{
	struct emp_t *left;
	struct emp_t *right;
}doubly_LL_t;

Glue based LL:
typedef struct _glthread{

    struct _glthread *left;
    struct _glthread *right;
} glthread_t;

struct emp_{
	char name[100];
	int ID;
	glthread_t *node;
}emp_t;
*/
// problem with tradiitonal structures(LL, trees, queues) - Inserting an emp strcutre in three different LL would mean 3 mallocs to add emp structure to 3 LLs. 
// if an employee leaves the office then we need to remove emp from all 3 LLs. Time complexity O(n) * 3
// GL threas needs to find the node and remove it becoems one time operations - TC: O(n)
// GL is used when an object needs to be referenced in various data structures then GL based LL is used.