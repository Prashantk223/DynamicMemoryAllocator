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
int main() {
    mm_init();
    printf("VM page size = %zu\n", SYSTEM_PAGE_SIZE);

    void* addr1 = mm_get_new_vm_page_from_kernel(1);
    void* addr2 = mm_get_new_vm_page_from_kernel(1);

    if (addr1 && addr2) {
        printf("Page 1 = %p, Page 2 = %p\n", addr1, addr2);
    }

    mm_return_vm_page_to_kernel(addr1, 1);
    mm_return_vm_page_to_kernel(addr2, 1);

    return 0;
}
