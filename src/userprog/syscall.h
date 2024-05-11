#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/synch.h"
#include "threads/thread.h"
void syscall_init (void);

#define ERROR -1
#define UNLOADED 0
#define LOADED 1
#define LOADED_FAIL 2
#define CLOSE_FILE -1
#define USER_VADDR_BOTTOM ((void *) 0x08048000)


struct lock lock_file_sys;

struct file_desc{
    struct file *fp;
    int fd;
    struct list_elem elem;
};

struct child_process {
  int pid;
  int load_status;
  int wait;
  int exit;
  int status;
  struct semaphore load_sema;
  struct semaphore exit_sema;
  struct list_elem elem;
};

int pointer_page (const void *vaddr);
struct child_process* find_child_process (int pid);
void remove_child_process (struct child_process *child);
void remove_all_child_processes (void);
struct file* get_file(int fd);
void process_close_file (int file_descriptor);
void exit (int status);



#endif /* userprog/syscall.h */
