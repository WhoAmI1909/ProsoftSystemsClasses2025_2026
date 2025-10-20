section .data
    message db  "Hello world!",10  ; строка для вывода на консоль
    length  equ $ - message

section .text
    global _start

_start:
    mov rax, 1                     ; 1 - номер системного вызова функции write
    mov rdi, 1                     ; 1 - дескриптор файла стандартного вызова stdout
    mov rsi, message               ; адрес строки для вывод
    mov rdx, length                ; количество байтов
    syscall                        ; выполняем системный вызов write
    
    cmp rax, 0
    jl error_exit
    
    ; Выход
    mov rax, 60
    mov rdi, 0
    syscall

error_exit:
    ; КРИТИЧЕСКАЯ ОШИБКА - преобразуем и выходим
    neg rax                 ; преобразуем отрицательный код ошибки в положительный
    mov rdi, rax            ; используем код ошибки как код возврата программы
    mov rax, 60             ; sys_exit
    syscall