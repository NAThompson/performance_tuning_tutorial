global dot

section .text
dot: ; Signature: dot(double* a, double* b, int64_t n), so a = rdi, b = rsi, n = rdx
    ; d = 0
    ; vxorpd xmm0, xmm0, xmm0
    vzeroall
    ; If n <= 0:
    test rdx,rdx
    jle dot.bye
    ; Now n > 0, so the sum is non-empty:
    ; If there are < 4 elements, we cannot use AVX-256 instructions:
    xor rcx, rcx
    xor rax, rax
    cmp rdx, 4
    jnae dot.small_vec
    ; rax will store n/4:
    mov rax, rdx
    and rax, -4

dot.large_vec:
    vmovupd ymm1,yword [rdi + 8*rcx] ; move a[i], a[i+1], a[i+2], a[i+3] into ymm1
    vmovupd ymm2,yword [rsi + 8*rcx] ; move b[i], b[i+1], b[i+2], b[i+3] into ymm2
    vfmadd231pd ymm0, ymm1, ymm2 ;
    add rcx, 4
    cmp rax, rcx
    jne dot.large_vec
    ; now ymm0 has 4 parts of the dot product. Add each one into xmm0:
    ; First, take [c0, c1, c2, c3] -> [c1 + c0, c0 + c1, c2 + c3, c2 + c3]
    vhaddpd ymm0, ymm0, ymm0
    ; Now extract the upper 128 bits out of the ymm0 register and place them in xmm1:
    vextractf128 xmm1, ymm0, 1
    ; And add:
    addpd xmm0, xmm1
    ; If the length of the vector is a multiple of 4, we don't need to do a small_vec cleanup:
    cmp rax, rdx
    je dot.bye

    mov rax, rcx

dot.small_vec:
    vmovsd xmm1, qword [rdi + 8*rax] ; mov a[i] into xmm1
    vfmadd231sd xmm0, xmm1, qword [rsi + 8*rax] ; d = fma(a[i], b[i], d)
    inc rax
    cmp rdx, rax
    jae dot.small_vec

dot.bye:
    vzeroupper ; clear 128 upper bits of ymm registers while preserving xmm portions of register
    ret
