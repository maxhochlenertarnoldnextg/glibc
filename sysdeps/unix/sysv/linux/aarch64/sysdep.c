/* Copyright (C) 2011-2024 Free Software Foundation, Inc.

   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <errno.h>

long __syscall_error (long err);
hidden_proto (__syscall_error)

/* This routine is jumped to by all the syscall handlers, to stash
   an error number into errno.  */
long
__syscall_error (long err)
{
  __set_errno (- err);
  return -1;
}
hidden_def (__syscall_error)


long syscall(long number, ...);

 char msg[256] ;
 void strcpy_simple(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

 void strcat_simple(char *dest, const char *src) {
    while (*dest) dest++;
    while ((*dest++ = *src++));
}
 void long_to_str(long num, char *str) {
    char buffer[32];
    int i = 30;
    buffer[31] = '\0';
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    do {
        buffer[i--] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    if (is_negative) {
        buffer[i--] = '-';
    }

    strcpy_simple(str, &buffer[i + 1]);
}

typedef struct {
    long syscall_number;
    long args[6];
} syscall_args_t;

void do_syscall(syscall_args_t *args){
    // Load syscall number and arguments into registers
    register long x8 asm("x8") = args->syscall_number;
    register long x0 asm("x0") = args->args[0];
    register long x1 asm("x1") = args->args[1];
    register long x2 asm("x2") = args->args[2];
    register long x3 asm("x3") = args->args[3];
    register long x4 asm("x4") = args->args[4];
    register long x5 asm("x5") = args->args[5];

    // Execute the syscall
    asm volatile ("svc 0"
                  : "=r"(x0), "=r"(x1), "=r"(x2), "=r"(x3), "=r"(x4), "=r"(x5) // x0-x5 will contain return values
                  : "0"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5), "r"(x8)
                  : "memory");

    // After syscall, save the return values back into the struct
    args->args[0] = x0;  // x0 is the return value of the syscall
    args->args[1] = x1;
    args->args[2] = x2;
    args->args[3] = x3;
    args->args[4] = x4;
    args->args[5] = x5;
}

#include <stdarg.h>

#define WRITE_SYSCALL 64
#define STDOUT_FD 1
#define STDERR_FD 2
#define MAX_BUFFER 1024

void simple_write(const char *buf, size_t len) {
    syscall_args_t args = {WRITE_SYSCALL, {STDERR_FD, (long)buf, len, 0, 0, 0}};
    do_syscall(&args);
}

void simple_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[MAX_BUFFER];
    int buf_index = 0;
    const char *fmt_ptr = format;

    while (*fmt_ptr && buf_index < MAX_BUFFER - 1) {
        if (*fmt_ptr != '%') {
            buffer[buf_index++] = *fmt_ptr++;
        } else {
            fmt_ptr++;
            switch (*fmt_ptr) {
                case 'd': {
                    long num = va_arg(args, long);
                    char num_buf[32];
                    long_to_str(num, num_buf);
                    for (int i = 0; num_buf[i] && buf_index < MAX_BUFFER - 1; i++) {
                        buffer[buf_index++] = num_buf[i];
                    }
                    break;
                }
                case 's': {
                    char *str = va_arg(args, char*);
                    while (*str && buf_index < MAX_BUFFER - 1) {
                        buffer[buf_index++] = *str++;
                    }
                    break;
                }
                default:
                    if (buf_index < MAX_BUFFER - 2) {
                        buffer[buf_index++] = '%';
                        buffer[buf_index++] = *fmt_ptr;
                    }
            }
            fmt_ptr++;
        }
    }
    buffer[buf_index] = '\0';

    simple_write(buffer, buf_index);

    va_end(args);
}

#include "/home/hochmax/llvm-project/pass/path_mappings.h" /* Include the path mappings header */

void convert_args(syscall_args_t *args) {
    //print syscall number
    // simple_printf("syscall_number:%d,%d\n", args->syscall_number, args->args[0]);

    const char* filename = (const char *)args->args[1];
    if (args->syscall_number == 56) { // openat syscall
        simple_printf("opening file: %s\n", args->args[1]);
        for (size_t i = 0; i < path_mappings_count; i++) {
            if (!strcmp(filename, path_mappings[i].original)) {
                /* Replace the original path with the new path */
                args->args[1] = (long) path_mappings[i].replacement;
                simple_printf("Filename changed to: %s\n", (const char *)args->args[1]);
                break; /* Stop after the first match */
            }
        }
    }
    if (args->syscall_number == 64) { // openat syscall
        if(args->args[0] == 1 || args->args[0] == 2) // then to stdout/err
        {
            simple_printf("PROGRAM STDOUT: %s\n", args->args[1]); 
        }
        
    }
            
    do_syscall(args);
}