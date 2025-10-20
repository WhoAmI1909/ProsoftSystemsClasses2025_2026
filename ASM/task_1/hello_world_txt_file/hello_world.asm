section .data
    filename db "output.txt", 0     ; variable name, type, string, bytes end
    message db "Hello World", 10
    msg_len equ $ - message         ;   message length

    ; open flags
    O_WRONLY equ 1  ; write only flag
    O_TRUNC equ 512 ; erase flag

section .bss
    fd resq 1          ; memory for descriptor

section .text
    global _start

_start:
    mov rax, 2      ; open
    lea rdi, [rel filename] ; adress of file name
    mov rsi, O_WRONLY | O_TRUNC
    syscall

    cmp rax, 0
    jl error_handler    ; error handler
    mov [fd], rax       ; save descriptor file    

    mov rax, 1          ; write to file
    mov rdi, [fd]       ; first arg - descriptor
    mov rsi, message
    mov rdx, msg_len
    syscall

    cmp rax, 0
    jl error_handler

    mov rax, 3          ; close file
    mov rdi, [fd]
    syscall

    mov rax, 60         ; programm ending
    mov rdi, 0
    syscall

error_handler:
    neg rax         ;   reverse error value
    mov rdi, rax    
    mov rax, 60
    syscall