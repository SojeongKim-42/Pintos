#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "pagedir.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

static void syscall_handler (struct intr_frame *f UNUSED);
struct lock big_lock;

void halt(void);
void exit (int status);
tid_t exec (const char *cmd_line);
int wait(tid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read(int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd); 

bool is_valid_ptr(const void *vaddr);
void kill_process(void);
struct file* get_file (int fd);

void syscall_init (void) 
{
  lock_init (&big_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/*Checks if the user address is valid*/
bool is_valid_ptr (const void * vaddr) {
      return vaddr != NULL && is_user_vaddr(vaddr) && pagedir_get_page(thread_current()->pagedir, vaddr) != NULL;
}

/*Calls exit with -1 status*/
void kill_process(void) {
  exit(-1);
}

/*Return file * equivalent to file descriptor */
struct file* get_file (int fd) {
    struct thread *cur = thread_current();
    struct list_elem *e;

    for (e = list_begin(&cur->file_list); e != list_end(&cur->file_list); e = list_next(e)) {
        struct file_desc *fd_elem = list_entry(e, struct file_desc, elem);
        if (fd_elem->fd == fd)
            return fd_elem;
    }
    return NULL;
}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  if (!is_valid_ptr(f -> esp))
    kill_process();

  int *p = f -> esp;
  int syscall_number = *p;

  switch (syscall_number) {
    case SYS_HALT: {
      halt();
      break;
    }

    case SYS_EXIT: {
      int *arg = esp + 1;
      if (!is_valid_ptr(arg))
        kill_process();
      int status = *arg;
      exit (status);
      break;
    }

    case SYS_EXEC: {
      char **cmd_line = *((char **) (f -> esp +1));
      if (!is_valid_ptr(arg) || !is_valid_ptr(*arg))
        kill_process();
      f -> eax = exec (*arg);
      break;
    }

    case SYS_WAIT: {
      int *arg = esp + 1;
      if (!is_valid_ptr(arg))
        kill_process();
      tid_t pid = *arg;
      f -> eax = wait (pid);
      break;
    }

    case SYS_CREATE: {
      char **arg = (char **)(esp + 1);
      unsigned *arg2 = (unsigned *)(esp + 2);
      if (!is_valid_ptr(arg) || !is_valid_ptr(*arg) || !is_valid_ptr(arg2))
        kill_process();
      f->eax = create(*arg, *arg2);
      break;
    }

    case SYS_REMOVE: {
      char **arg = (char **)(esp + 1);
      if (!is_valid_ptr(arg) || !is_valid_ptr(*arg))
        kill_process();
      f->eax = remove(*arg);
      break;
    }

    case SYS_OPEN: {
      char **arg = (char **)(esp + 1);
      if (!is_valid_ptr(arg) || !is_valid_ptr(*arg))
        kill_process();
      f->eax = open(*arg);
      break;
    }

    case SYS_FILESIZE: {
      int *arg = esp + 1;
      if (!is_valid_ptr(arg))
        kill_process();
      int fd = *arg;
      f->eax = filesize(fd);
      break;
    }

    case SYS_READ: {
      int *fd_arg = esp + 1;
      void **buf_arg = (void **)(esp + 2);
      unsigned *size_arg = (unsigned *)(esp + 3);
      if (!is_valid_ptr(fd_arg) || !is_valid_ptr(buf_arg) || !is_valid_ptr(*buf_arg) || !is_valid_ptr(size_arg))
        kill_process();
      f->eax = read(*fd_arg, *buf_arg, *size_arg);
      break;
    }

    case SYS_WRITE: P{
      int *fd_arg = esp + 1;
      void **buf_arg = (void **)(esp + 2);
      unsigned *size_arg = (unsigned *)(esp + 3);
      if (!is_valid_ptr(fd_arg) || !is_valid_ptr(buf_arg) || !is_valid_ptr(*buf_arg) || !is_valid_ptr(size_arg))
        kill_process();
      f->eax = write(*fd_arg, *buf_arg, *size_arg);
      break;
    }

    case SYS_SEEK: {
      int *fd_arg = esp + 1;
      unsigned *pos_arg = (unsigned *)(esp + 2);
      if (!is_valid_ptr(fd_arg) || !is_valid_ptr(pos_arg))
        kill_process();
      seek(*fd_arg, *pos_arg);
      break;
    }

    case SYS_TELL: {
      int *fd_arg = esp + 1;
      if (!is_valid_ptr(fd_arg))
        kill_process();
      f->eax = tell(*fd_arg);
      break;
    }

    case SYS_CLOSE: {
      int *fd_arg = esp + 1;
      if (!is_valid_ptr(fd_arg))
        kill_process();
      close(*fd_arg);
      break;
    }

    default:
      hex_dump(esp, esp, 64, true);
      kill_process();
      break;
  }
}

/*System Calls*/
void halt(void) 
{
  shutdown_power_off();
}

void exit (int status) {
  struct thread *curr = thread_current();
  struct thread *parent = curr->parent;

  /* Updating the exit code of the current thread */
  curr->exit_code = status;

  if (!list_empty(&parent->children)) {
    struct child *child = get_child(curr->tid, parent);

    if (child != NULL) {
      child->ret_val = status;
      child->used = true;
        
        if (parent->waiton_child == curr->tid)
          sema_up(&parent->child_sem);
        }
    }
    
    thread_exit();
}

tid_t exec (const char *cmd_line) {
  lock_acquire(&big_lock);
  tid_t pid = process_execute (cmd_line);
  lock_release(&big_lock);
  return pid;
}

int wait(tid_t pid) {
  return process_wait(pid);
}

