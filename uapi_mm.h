
#include <stdint.h> //for uint
void mm_init();
void mm_instantiate_new_page_family(char* struct_name, uint32_t struct_size);
//here # is ised to convert the argument to string
#define MM_REG_STRUCT(struct_name) \
	(mm_instantiate_new_page_family(#struct_name, sizeof(struct_name)))