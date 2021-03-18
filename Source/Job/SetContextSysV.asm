mov r8, [rdi + 8 * 0]

mov rsp, [rdi + 8 * 1]

mov rbx, [rdi + 8 * 2]
mov rbp, [rdi + 8 * 3]
mov r12, [rdi + 8 * 4]
mov r13, [rdi + 8 * 5]
mov r14, [rdi + 8 * 6]
mov r15, [rdi + 8 * 7]

push r8

xor eax, eax
ret
