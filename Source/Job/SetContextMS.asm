mov r8, [rcx + 8 * 0] ; Load RIP into r8

mov rsp, [rcx + 8 * 1] ; Change RSP back

mov rbx, [rcx + 8 * 2] ; Other GPRs
mov rbp, [rcx + 8 * 3]
mov r12, [rcx + 8 * 4]
mov r13, [rcx + 8 * 5]
mov r14, [rcx + 8 * 6]
mov r15, [rcx + 8 * 7]
mov rdi, [rcx + 8 * 8]
mov rsi, [rcx + 8 * 9]

movups xmm6, [rcx + 8 * 10 + 16 * 0] ; XMM registers
movups xmm7, [rcx + 8 * 10 + 16 * 1]
movups xmm8, [rcx + 8 * 10 + 16 * 2]
movups xmm9, [rcx + 8 * 10 + 16 * 3]
movups xmm10, [rcx + 8 * 10 + 16 * 4]
movups xmm11, [rcx + 8 * 10 + 16 * 5]
movups xmm12, [rcx + 8 * 10 + 16 * 6]
movups xmm13, [rcx + 8 * 10 + 16 * 7]
movups xmm14, [rcx + 8 * 10 + 16 * 8]
movups xmm15, [rcx + 8 * 10 + 16 * 9]

push r8 ; Set the return address to the loaded address

xor eax, eax
ret
