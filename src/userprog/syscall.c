#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "pagedir.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

#define max_arg 3

static void syscall_handler (struct intr_frame *f UNUSED);
int add_file (struct file * file_name);
void get_arg (struct intr_frame *f, int *arg, int num_of_args);
void halt(void);
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
void is_valid_str (const void *str);
bool is_valid_buffer (const void *buf, unsigned byte_size);

bool file_lock = false;


int add_file (struct file *file_name) {
  struct thread *cur_thread = thread_current();
  struct file_desc *a = malloc (sizeof(struct file_desc));
  if (!a)
  {
    return ERROR;
  }
  a -> fp = file_name;
  a -> fd = cur_thread -> fd++;
  list_push_back (&cur_thread -> file_list, &a -> elem);
  return a ->fd; 
}

void get_arg (struct intr_frame *f, int *args, int num_of_args){
  int i;
  int *ptr;
  for (i = 0; i < num_of_args; i++)
  {
    ptr = (int *) f -> esp + i + 1;
    is_valid_ptr ((const void*) ptr);
    args[i] = *ptr;
  }
}

/*Checks if the user address is valid*/
bool is_valid_ptr (const void * vaddr) {
      return vaddr != NULL && is_user_vaddr(vaddr) && pagedir_get_page(thread_current()->pagedir, vaddr) != NULL;
}

/*Calls exit with -1 status*/
void is_valid_str (const void *str) {
    is_valid_ptr(str);
}

bool is_valid_buffer (const void *buf, unsigned byte_size) {
    if (!is_valid_ptr(buf)) {
        return false;
    }
    return true;
}

/*Return file * equivalent to file descriptor */
struct file* get_file (int fd) {
    struct thread *cur = thread_current();
    struct list_elem *e;

    for (e = list_begin(&cur->file_list); e != list_end(&cur->file_list); e = list_next(e)) {
        struct file_desc *fd_elem = list_entry(e, struct file_desc, elem);
        if (fd_elem->fd == fd)
            return fd_elem -> fp;
    }
    return NULL;
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  if (!file_lock)
  {
    lock_init (&lock_file_sys);
    file_lock = true;
  }

  int arg [max_arg];
  int esp = pointer_page ((const void *) f -> esp);
  if (esp == -1) {
    f -> eax = ERROR;
    return;
  }

  int syscall_number = *(int *)esp;

  switch (syscall_number) {
    case SYS_HALT: {
      halt();
      break;
    }

    case SYS_EXIT: {
      get_arg (f, &arg[0], 1);
      sys_exit(arg[0]);
      break;
    }

    case SYS_EXEC: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      is_valid_str((const void *)arg[0]);
      arg[0] = (int)pointer_page((const void *)arg[0]);
      f->eax = exec((const char *)arg[0]);
      break;
    }

    case SYS_WAIT: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      f->eax = wait(arg[0]);
      break;
    }

    case SYS_CREATE: {
      get_arg(f, &arg[0], 2);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      is_valid_str((const void *)arg[0]);
      arg[0] = (int)pointer_page((const void *)arg[0]);
      f->eax = create((const char *)arg[0], (unsigned)arg[1]);
      break;
    }

    case SYS_REMOVE: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      is_valid_str((const void *)arg[0]);
      arg[0] = (int)pointer_page((const void *)arg[0]);
      f->eax = remove((const char *)arg[0]);
      break;
    }

    case SYS_OPEN: {
     get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      is_valid_str((const void *)arg[0]);
      arg[0] = (int)pointer_page((const void *)arg[0]);
      f->eax = open((const char *)arg[0]);
      break;
    }

    case SYS_FILESIZE: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      f->eax = filesize(arg[0]);
      break;
    }

    case SYS_READ: {
     get_arg(f, &arg[0], 3);
      if (!is_valid_ptr((const void *)arg[1]))
        sys_exit(ERROR);
      is_valid_buffer((const void *)arg[1], (unsigned)arg[2]);
      arg[1] = (int)pointer_page((const void *)arg[1]);
      f->eax = read(arg[0], (void *)arg[1], (unsigned)arg[2]);
      break;
    }

    case SYS_WRITE: {
      get_arg(f, &arg[0], 3);
      if (!is_valid_ptr((const void *)arg[1]))
        sys_exit(ERROR);
      is_valid_buffer((const void *)arg[1], (unsigned)arg[2]);
      arg[1] = (int)pointer_page((const void *)arg[1]);
      f->eax = write(arg[0], (const void *)arg[1], (unsigned)arg[2]);
      break;
    }

    case SYS_SEEK: {
      get_arg(f, &arg[0], 2);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      seek(arg[0], (unsigned)arg[1]);
      break;
    }

    case SYS_TELL: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      f->eax = tell(arg[0]);
      break;
    }

    case SYS_CLOSE: {
      get_arg(f, &arg[0], 1);
      if (!is_valid_ptr((const void *)arg[0]))
        sys_exit(ERROR);
      close(arg[0]);
      break;
    }

    default:
      break;
  }
}

/*System Calls*/
void halt(void) 
{
  shutdown_power_off();
}

void sys_exit (int status) {
  struct thread *cur = thread_current();
  if (check_thread(cur->parent) && cur->cp)
  {
    if (status < 0)
    {
      status = -1;
    }
    cur->cp->status = status;
  }
  printf ("%s: exit(%d)\n", cur -> name, status);
  thread_exit();
}

tid_t exec (const char *cmd_line) {
    tid_t tid = process_execute(cmd_line);
    struct child_process *cp_pointer = find_cp(tid);
    if (!cp_pointer)
    {
      return ERROR;
    }
    /* check if process if loaded */
    if (cp_pointer->load_status == UNLOADED)
    {
      sema_down(&cp_pointer->load_sema);
    }
    /* check if process failed to load */
    if (cp_pointer->load_status == LOADED_FAIL)
    {
      remove_cp(cp_pointer);
      return ERROR;
    }
    return tid;
}

int wait(tid_t id ) {
  tid_t tid = process_wait(id);
  return tid;
}

bool create (const char *file, unsigned initial_size){
  lock_acquire(&lock_file_sys);
  bool new = filesys_create(file, initial_size); // from filesys.h
  lock_release(&lock_file_sys);
  return new;
}

bool remove (const char *file) {
  lock_acquire(&lock_file_sys);
  bool new = filesys_remove(file); // from filesys.h
  lock_release(&lock_file_sys);
  return new;
}

int open (const char *file){
  lock_acquire(&lock_file_sys);
  struct file *f_pointer = filesys_open(file); // from filesys.h
  if (!f_pointer)
  {
    lock_release(&lock_file_sys);
    return ERROR;
  }
  int filedes = add_file(f_pointer);
  lock_release(&lock_file_sys);
  return filedes;
}

int filesize (int fd) {
  lock_acquire(&lock_file_sys);
  struct file *f_pointer = get_file(fd);
  if (!f_pointer)
  {
    lock_release(&lock_file_sys);
    return ERROR;
  }
  int filesize = file_length(f_pointer); // from file.h
  lock_release(&lock_file_sys);
  return filesize;
}

#define SYSINPUT 0
#define SYSOUTPUT 1

int read(int fd, void *buffer, unsigned size){
  if (size <= 0)
  {
    return size;
  }
  
  if (fd == SYSINPUT)
  {
    unsigned i = 0;
    uint8_t *local_buf = (uint8_t *) buffer;
    for (;i < size; i++)
    {
      local_buf[i] = input_getc(); // from input.h
    }
    return size;
}
  lock_acquire(&lock_file_sys);
  struct file *f_pointer = get_file(fd);
  if (!f_pointer)
  {
    lock_release(&lock_file_sys);
    return ERROR;
  }
  int bytes_read = file_read(f_pointer, buffer, size); // from file.h
  lock_release (&lock_file_sys);
  return bytes_read;
}


int write (int fd, const void *buffer, unsigned size){
    if (size <= 0)
    {
      return size;
    }
    if (fd == SYSOUTPUT)
    {
      putbuf (buffer, size); // from stdio.h
      return size;
    }
    lock_acquire(&lock_file_sys);
    struct file *f_pointer = get_file(fd);
    if (!f_pointer)
    {
      lock_release(&lock_file_sys);
      return ERROR;
    }
    int bytes_written = file_write(f_pointer, buffer, size); // file.h
    lock_release (&lock_file_sys);
    return bytes_written;
}

void seek (int fd, unsigned position){
  lock_acquire(&lock_file_sys);
  struct file *f_pointer = get_file(fd);
  if (!f_pointer)
  {
    lock_release(&lock_file_sys);
    return;
  }
  file_seek(f_pointer, position);
  lock_release(&lock_file_sys);
}

unsigned tell (int fd) {
  lock_acquire(&lock_file_sys);
  struct file *f_pointer = get_file(fd);
  if (!f_pointer)
  {
    lock_release(&lock_file_sys);
    return ERROR;
  }
  off_t offset = file_tell(f_pointer); //from file.h
  lock_release(&lock_file_sys);
  return offset;
}

void close (int fd) {
  lock_acquire(&lock_file_sys);
  close_fd(fd);
  lock_release(&lock_file_sys);
}

int pointer_page (const void *vaddr){
    if (is_valid_ptr(vaddr)) {
        return (int)vaddr;
    } else {
        return -1;
    }
}

struct child_process* find_cp(int pid)
{
  struct thread *t = thread_current();
  struct list_elem *e;
  struct list_elem *next;
  
  for (e = list_begin(&t->child_list); e != list_end(&t->child_list); e = next)
  {
    next = list_next(e);
    struct child_process *cp = list_entry(e, struct child_process, elem);
    if (pid == cp->pid)
    {
      return cp;
    }
  }
  return NULL;
}

void
remove_cp (struct child_process *cp)
{
  list_remove(&cp->elem);
  free(cp);
}

/* remove all child processes for a thread */
void remove_all_cp (void) 
{
  struct thread *t = thread_current();
  struct list_elem *next;
  struct list_elem *e = list_begin(&t->child_list);
  
  for (;e != list_end(&t->child_list); e = next)
  {
    next = list_next(e);
    struct child_process *cp = list_entry(e, struct child_process, elem);
    list_remove(&cp->elem); //remove child process
    free(cp);
  }
}

void
close_fd (int file_descriptor)
{
  struct thread *t = thread_current();
  struct list_elem *next;
  struct list_elem *e = list_begin(&t->file_list);
  
  for (;e != list_end(&t->file_list); e = next)
  {
    next = list_next(e);
    struct file_desc *p = list_entry (e, struct file_desc, elem);
    if (file_descriptor == p->fd || file_descriptor == CLOSE_FILE)
    {
      file_close(p->fp);
      list_remove(&p->elem);
      free(p);
      if (file_descriptor != CLOSE_FILE)
      {
        return;
      }
    }
  }
}
