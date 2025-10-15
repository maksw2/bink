.code

; void enable_avx2()
enable_avx2 PROC
    ; Set CR4.OSXSAVE (bit 18)
    mov     rax, cr4
    or      eax, 40000h        ; set bit 18
    mov     cr4, rax

    ; Read XCR0 (xgetbv, ecx=0)
    xor     ecx, ecx
    xgetbv

    ; Enable XMM (bit 0) and YMM (bit 1)
    or      eax, 6

    ; Write XCR0 (xsetbv, ecx=0)
    xor     ecx, ecx
    xsetbv

    ret
enable_avx2 ENDP

END
