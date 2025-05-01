#include "uapi_mm.h"
typedef struct emp_ {
	uint32_t emp_id;
	char name[32];
}emp_t;

/* student_ is the tag name of the struct. It's used internally within the struct 
 * definition—particularly to define self-referential pointers (like struct student_* next;)*/
typedef struct student_ {
	char name[32];
	uint32_t rollno;
	uint32_t marks_phys;
	uint32_t marks_chem;
	uint32_t maths;
	struct student_* next;
}student_t;

int main() {
	mm_init();
	MM_REG_STRUCT(emp_t);
	MM_REG_STRUCT(student_t);
	return 0;
}