/*
 * =====================================================================================
 *
 *       Filename:  glthread.h
 *
 *    Description:  This file defines the Data structure and APIs for Glue thread
 * =====================================================================================
 */
 
 typedef struct _glthread{

    struct _glthread *left;
    struct _glthread *right;
} glthread_t;

void
glthread_add_next(glthread_t *base_glthread, glthread_t *new_glthread);

void
remove_glthread(glthread_t *glthread);

void
remove_glthread(glthread_t *glthread);

#define IS_GLTHREAD_LIST_EMPTY(glthreadptr)         \
    ((glthreadptr)->right == 0 && (glthreadptr)->left == 0)