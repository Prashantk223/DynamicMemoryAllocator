#pragma once
#include <stdint.h> /*uint32_t*/

typedef enum{
	MM_FALSE,
	MM_TRUE
}vm_bool_t;

/*Forward declaration*/
struct vm_page_family_;
struct block_meta_data_t;
struct vm_page_ *next;
{
	vm_page_ *next;
	vm_page_ *prev;
	struct vm_page_family_ *pg_family; //pointer to family
	block_meta_data_t block_meta_data;//first meta block in VM page
	char page_memory[0]; //first data block in VM page
}vm_page_t;
#define MM_MAX_STRUCT_SIZE 32
typedef struct vm_page_family_ {
	char struct_name[MM_MAX_STRUCT_SIZE];
	uint32_t struct_size;
	vm_page_t *first_page;
}vm_page_family_t;

typedef struct vm_page_for_families_ {
	struct vm_page_for_families_* next;
	vm_page_family_t vm_page_family[0];
}vm_page_for_families_t;

#define MAX_FAMILIES_PER_VM_PAGE \
	(SYSTEM_PAGE_SIZE - sizeof(vm_page_for_families_t *)/ \
	sizeof(vm_page_family_t))
typedef struct block_meta_data_{
	vm_bool_t is_free;
	uint32_t block_size;
	uint32_t offset; // distance of meta block from start of virtual memory
	struct block_meta_data_ *prev_block;
	struct block_meta_data_ *next_block;
}block_meta_data_t;
//Mandatory assignment
#define offset_of(container_name, field_name) \
	((size_t)&(((container_name *)0)->field_name)) 

#define MM_GET_PAGE_FROM_META_BLOCK(block_meta_data_ptr)    \
	(void *)((char *)block_meta_data_ptr - block_meta_data_ptr->offset)

#define NEXT_META_BLOCK(block_meta_data_ptr)    \
	(block_meta_data_ptr->next_block)
	
#define NEXT_META_BLOCK_BY_SIZE(block_meta_data_ptr) \
	(block_meta_data_t *)((char *)(block_meta_data_ptr +1 ) \
	+ block_meta_data_ptr->prev_block)
	
#define PREV_META_BLOCK(block_meta_data_ptr) \
	(block_meta_data_ptr->prev_block) 

//Data block was split to create an allocated data block and a free data block.
#define mm_bind_blocks_for_allocation(allocated_meta_block, free_meta_block)  \
	free_meta_block->prev_block = allocated_meta_block				\
	free_meta_block->next_block = allocated_meta_block->next_block 	\
	allocated_meta_block->next_block = free_meta_block				\
	if(free_meta_block->next_block)									\
		free_meta_block->next_block->prev_block = free_meta_block

#define MARK_VM_PAGE_EMPTY(vm_page_t_ptr)	\
	vm_page_t_ptr->block_meta_data.next_block = NULL; \
	vm_page_t_ptr->block_meta_data.prev_block = NULL; \
	vm_page_t_ptr->block_meta_data.is_free = MM_TRUE

//Macro to iterate over all VM data pages for a gu=iven page family
#define ITERATE_VM_PAGE_BEGIN(vm_page_family_ptr, cur)											\
{																								\
	for(cur = vm_page_family_ptr->first_page; cur; cur = cur->next)								\
	{																							\
			
#define ITERATE_VM_PAGE_BEGIN(vm_page_family_ptr, cur)}}

//iterate over all meta blocks of a given vm data page
#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(vm_page_ptr, cur)										\
	for(cur = vm_page_ptr->block_meta_data; cur; cur = cur->next_block)					\
	{
		
#define ITERATE_VM_PAGE_ALL_BLOCKS_END(vm_page_ptr, cur)}}									\
	
#define ITERATE_PAGE_FAMILIES_BEGIN(vm_page_for_families_ptr, curr) 							\
{																								\
		uint32_t count = 0;																		\
		for(curr = (vm_page_for_families_ptr *)&vm_page_for_families_ptr->vm_page_family[0];	\
			curr->struct_size && count < MAX_FAMILIES_PER_VM_PAGE;								\
			cur++, count++){																	\
			
#define ITERATE_PAGE_FAMILIES_END(vm_page_for_families_ptr, curr)		}}


//Global function definition
vm_page_t* allocate_vm_page(vm_page_family_t *vm_page_family);
void mm_vm_page_delete_and_free(vm_page_t *vm_page);
vm_page_t* allocate_vm_page(vm_page_family_t *vm_page_family)