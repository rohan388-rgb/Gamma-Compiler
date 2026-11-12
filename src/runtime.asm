global print_int
global print_string
global print_decimal
global read_int
global read_decimal
global read_string
global string_contains
global string_starts_with
global string_ends_with
global string_trim
global string_uppercase
global string_lowercase
global string_replace
global string_substring
global string_split
global string_join

section .bss
    print_buffer resb 32
    input_buffer resb 32
    split_array_buffer resb 4096
    split_data_buffer resb 4096
    join_buffer resb 4096
    replace_buffer resb 4096
    substring_buffer resb 4096
    split_buffer resb 4096


section .text

; ------------------------------------------------------------
; print_int
; Input : RAX = unsigned integer
; Prints integer followed by newline
; ------------------------------------------------------------
print_int:
    push rbx
    push r12

    mov rax, rdi
    xor r12, r12
    mov rbx, 10

    lea rsi, [print_buffer + 31]
    mov byte [rsi], 10

    test rax, rax
    jns .positive

    mov r12, 1
    neg rax

.positive:
    test rax, rax
    jnz .convert

    dec rsi
    mov byte [rsi], '0'
    jmp .add_sign

.convert:
    dec rsi
    xor rdx, rdx
    div rbx

    add dl, '0'
    mov [rsi], dl

    test rax, rax
    jnz .convert

.add_sign:
    cmp r12, 0
    je .print

    dec rsi
    mov byte [rsi], '-'

.print:
    mov rax, 1
    mov rdi, 1
    mov rdx, print_buffer + 32
    sub rdx, rsi
    syscall

    pop r12
    pop rbx
    ret


print_decimal:
    push rbx
    push r12
    push r13

    xor r12, r12
    pxor xmm1, xmm1

    ucomisd xmm0, xmm1
    jae .positive

    mov r12, 1
    xorpd xmm1, xmm1
    subsd xmm1, xmm0
    movapd xmm0, xmm1

.positive:
    cvttsd2si rax, xmm0
    mov r13, rax

    cvtsi2sd xmm1, rax
    subsd xmm0, xmm1

    mov rbx, 1000000
    cvtsi2sd xmm1, rbx
    mulsd xmm0, xmm1

    cvtsd2si rdx, xmm0

    cmp rdx, 1000000
    jne .fraction_ready

    inc r13
    xor rdx, rdx

.fraction_ready:
    mov r9, rdx

    lea rsi, [print_buffer + 31]
    mov byte [rsi], 10

    dec rsi
    mov rcx, 6

.fraction_loop:
    mov rax, r9
    xor rdx, rdx
    mov rbx, 10
    div rbx

    add dl, '0'
    mov [rsi], dl
    dec rsi

    mov r9, rax
    loop .fraction_loop

    mov byte [rsi], '.'
    dec rsi

    mov rax, r13
    mov rbx, 10

    cmp rax, 0
    jne .integer_loop

    mov byte [rsi], '0'
    dec rsi
    jmp .integer_done

.integer_loop:
    xor rdx, rdx
    div rbx

    add dl, '0'
    mov [rsi], dl
    dec rsi

    test rax, rax
    jnz .integer_loop

.integer_done:
    cmp r12, 0
    je .sign_done

    mov byte [rsi], '-'
    dec rsi

.sign_done:
    inc rsi

    lea rdi, [print_buffer + 32]
    mov rdx, rdi
    sub rdx, rsi

    mov rax, 1
    mov rdi, 1
    syscall

    pop r13
    pop r12
    pop rbx
    ret


; ------------------------------------------------------------
; print_string
; Input : RAX = pointer to null-terminated string
; (Placeholder)
; ------------------------------------------------------------
print_string:
    mov rsi, rax
    xor rdx, rdx

.find_length:
    cmp byte [rsi + rdx], 0
    je .print
    inc rdx
    jmp .find_length

.print:
    mov rax, 1
    mov rdi, 1
    syscall

    mov byte [print_buffer + 31], 10

    mov rax, 1
    mov rdi, 1
    lea rsi, [print_buffer + 31]
    mov rdx, 1
    syscall

    ret

; ------------------------------------------------------------
; read_int
; Returns integer in RAX
; (Placeholder)
; ------------------------------------------------------------
read_int:
    xor r9, r9          ; result
    xor r8, r8          ; negative flag

.read:
    ; Read exactly one byte
    mov rax, 0
    mov rdi, 0
    mov rsi, input_buffer
    mov rdx, 1
    syscall

    cmp rax, 0
    jle .done

    movzx rcx, byte [input_buffer]

    ; Newline
    cmp rcx, 10
    je .done

    ; Carriage return
    cmp rcx, 13
    je .done

    ; Negative sign
    cmp rcx, '-'
    jne .digit

    cmp r9, 0
    jne .read

    mov r8, 1
    jmp .read

.digit:
    cmp rcx, '0'
    jb .read

    cmp rcx, '9'
    ja .read

    sub rcx, '0'

    imul r9, r9, 10
    add r9, rcx

    jmp .read

.done:
    mov rax, r9

    cmp r8, 0
    je .return

    neg rax

.return:
    ret

read_decimal:
    mov rax, 0
    mov rdi, 0
    mov rsi, input_buffer
    mov rdx, 32
    syscall

    mov rsi, input_buffer
    xor rax, rax
    xor r8, r8
    xor r9, r9
    mov r10, 1

    movzx rcx, byte [rsi]

    cmp rcx, '-'
    jne .check_plus

    mov r10, -1
    inc rsi
    jmp .integer

.check_plus:
    cmp rcx, '+'
    jne .integer

    inc rsi

.integer:
    movzx rcx, byte [rsi]

    cmp rcx, '.'
    je .fraction_start

    cmp rcx, 10
    je .convert

    cmp rcx, 13
    je .convert

    cmp rcx, 0
    je .convert

    sub rcx, '0'
    imul rax, rax, 10
    add rax, rcx

    inc rsi
    jmp .integer

.fraction_start:
    inc rsi
    mov r8, 0
    mov r9, 1

.fraction:
    movzx rcx, byte [rsi]

    cmp rcx, 10
    je .fraction_done

    cmp rcx, 13
    je .fraction_done

    cmp rcx, 0
    je .fraction_done

    sub rcx, '0'

    imul r8, r8, 10
    add r8, rcx

    imul r9, r9, 10

    inc rsi
    jmp .fraction

.fraction_done:
    cvtsi2sd xmm0, rax

    cvtsi2sd xmm1, r8
    cvtsi2sd xmm2, r9

    divsd xmm1, xmm2
    addsd xmm0, xmm1

    jmp .apply_sign

.convert:
    cvtsi2sd xmm0, rax

.apply_sign:
    cmp r10, 1
    je .done

    xorpd xmm1, xmm1
    subsd xmm1, xmm0
    movapd xmm0, xmm1

.done:
    movq rax, xmm0
    ret
; ------------------------------------------------------------
; read_string
; Returns pointer in RAX
; (Placeholder)
; ------------------------------------------------------------
read_string:
    push rbx

    mov rax, 9
    xor rdi, rdi
    mov rsi, 4096
    mov rdx, 3
    mov r10, 0x22
    mov r8, -1
    xor r9, r9
    syscall

    mov rbx, rax

    mov rax, 0
    mov rdi, 0
    mov rsi, rbx
    mov rdx, 4095
    syscall

    mov rcx, rax
    mov rsi, rbx

.find_end:
    cmp rcx, 0
    je .terminate

    mov al, byte [rsi]

    cmp al, 10
    je .terminate

    cmp al, 13
    je .terminate

    inc rsi
    dec rcx
    jmp .find_end

.terminate:
    mov byte [rsi], 0

    mov rax, rbx

    pop rbx
    ret

string_contains:
    push rbx

    mov rdx, rdi

.outer:
    cmp byte [rdx], 0
    je .not_found

    mov rcx, rdx
    mov r8, rsi

.inner:
    mov al, byte [r8]

    cmp al, 0
    je .found

    cmp al, byte [rcx]
    jne .next_position

    inc r8
    inc rcx

    jmp .inner

.next_position:
    inc rdx
    jmp .outer

.found:
    mov rax, 1
    pop rbx
    ret

.not_found:
    xor rax, rax
    pop rbx
    ret

string_starts_with:
    push rbx

    mov rdx, rdi
    mov rbx, rsi

.check:
    mov al, byte [rbx]

    cmp al, 0
    je .found

    cmp al, byte [rdx]
    jne .not_found

    inc rbx
    inc rdx
    jmp .check

.found:
    mov rax, 1
    pop rbx
    ret

.not_found:
    xor rax, rax
    pop rbx
    ret
string_ends_with:
    push rbx
    push rcx
    push rdx

    mov rdx, rdi
    xor rcx, rcx

.find_text_end:
    cmp byte [rdx + rcx], 0
    je .text_end_found
    inc rcx
    jmp .find_text_end

.text_end_found:
    mov rdx, rsi
    xor r8, r8

.find_value_end:
    cmp byte [rdx + r8], 0
    je .value_end_found
    inc r8
    jmp .find_value_end

.value_end_found:
    cmp r8, rcx
    ja .not_found

    sub rcx, r8
    add rdi, rcx

.check:
    cmp r8, 0
    je .found

    mov al, byte [rdi]
    cmp al, byte [rsi]
    jne .not_found

    inc rdi
    inc rsi
    dec r8
    jmp .check

.found:
    mov rax, 1
    pop rdx
    pop rcx
    pop rbx
    ret

.not_found:
    xor rax, rax
    pop rdx
    pop rcx
    pop rbx
    ret

string_trim:
    push rbx
    push rcx
    push rdx

    mov rbx, rdi

.skip_left:
    mov al, byte [rbx]
    cmp al, ' '
    je .left_next
    cmp al, 9
    je .left_next
    jmp .left_done

.left_next:
    inc rbx
    jmp .skip_left

.left_done:
    mov rdx, rbx
    xor rcx, rcx

.find_end:
    cmp byte [rdx + rcx], 0
    je .end_found
    inc rcx
    jmp .find_end

.end_found:
    test rcx, rcx
    jz .empty

    dec rcx

.skip_right:
    cmp rcx, 0
    jl .all_trimmed

    mov al, byte [rdx + rcx]
    cmp al, ' '
    je .right_next
    cmp al, 9
    je .right_next
    jmp .right_done

.right_next:
    dec rcx
    jmp .skip_right

.right_done:
    mov byte [rdx + rcx + 1], 0
    mov rax, rbx

    pop rdx
    pop rcx
    pop rbx
    ret

.all_trimmed:
    mov byte [rdx], 0
    mov rax, rdx

    pop rdx
    pop rcx
    pop rbx
    ret

.empty:
    mov rax, rdx

    pop rdx
    pop rcx
    pop rbx
    ret

string_uppercase:
    push rbx

    mov rbx, rdi

.loop:
    mov al, byte [rbx]

    cmp al, 0
    je .done

    cmp al, 'a'
    jb .next

    cmp al, 'z'
    ja .next

    sub byte [rbx], 32

.next:
    inc rbx
    jmp .loop

.done:
    mov rax, rdi

    pop rbx
    ret

string_lowercase:
    push rbx

    mov rbx, rdi

.loop:
    mov al, byte [rbx]

    cmp al, 0
    je .done

    cmp al, 'A'
    jb .next

    cmp al, 'Z'
    ja .next

    add byte [rbx], 32

.next:
    inc rbx
    jmp .loop

.done:
    mov rax, rdi

    pop rbx
    ret

string_replace:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi
    mov r13, rsi
    mov r14, rdx

    xor r8, r8
    mov rbx, r13

.find_old_length:
    cmp byte [rbx], 0
    je .old_length_done

    inc rbx
    inc r8
    jmp .find_old_length

.old_length_done:
    test r8, r8
    jz .return_original

    xor r9, r9
    mov rbx, r14

.find_new_length:
    cmp byte [rbx], 0
    je .new_length_done

    inc rbx
    inc r9
    jmp .find_new_length

.new_length_done:
    lea r15, [rel replace_buffer]

    mov r10, r12
    xor r11, r11

.next:
    mov al, byte [r10]

    test al, al
    jz .finish

    xor rcx, rcx

.check:
    cmp rcx, r8
    je .match

    mov al, byte [r10 + rcx]

    test al, al
    jz .copy_one

    cmp al, byte [r13 + rcx]
    jne .copy_one

    inc rcx
    jmp .check

.match:
    xor rcx, rcx

.copy_new:
    cmp rcx, r9
    je .skip_old

    mov al, byte [r14 + rcx]
    mov byte [r15 + r11], al

    inc rcx
    inc r11

    jmp .copy_new

.skip_old:
    add r10, r8
    jmp .next

.copy_one:
    mov al, byte [r10]

    mov byte [r15 + r11], al

    inc r10
    inc r11

    jmp .next

.finish:
    mov byte [r15 + r11], 0

    mov rax, r15
    jmp .restore

.return_original:
    mov rax, r12

.restore:
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

string_substring:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10

    mov r8, rdi
    mov r9, rsi
    mov r10, rdx

    xor rcx, rcx
    mov rbx, r8

.find_end:
    cmp byte [rbx], 0
    je .end_found

    inc rbx
    inc rcx

    jmp .find_end

.end_found:
    cmp r9, rcx
    jae .empty

    lea rdi, [rel substring_buffer]

    xor rbx, rbx

.copy:
    cmp rbx, r10
    jae .finish

    mov rax, r9
    add rax, rbx

    cmp rax, rcx
    jae .finish

    mov al, byte [r8 + rax]
    mov byte [rdi + rbx], al

    inc rbx
    jmp .copy

.finish:
    mov byte [rdi + rbx], 0

    mov rax, rdi

    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

.empty:
    lea rax, [rel substring_buffer]
    mov byte [rax], 0

    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret

string_split:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov r12, rdi              ; input text
    mov r13, rsi              ; delimiter

    ; Find delimiter length
    xor r8, r8
    mov rbx, r13

.find_delimiter_length:
    cmp byte [rbx], 0
    je .delimiter_length_done

    inc rbx
    inc r8
    jmp .find_delimiter_length

.delimiter_length_done:

    lea r14, [rel split_array_buffer]
    lea r15, [rel split_data_buffer]

    mov r10, r12              ; input pointer
    mov r11, r15              ; output pointer
    xor rcx, rcx              ; token count

    ; Empty delimiter
    test r8, r8
    jz .single_token

.next_token:

    ; Save pointer to current token
    mov qword [r14 + rcx * 8 + 8], r11

.scan:

    mov al, byte [r10]

    ; End of source string
    test al, al
    jz .finish

    ; Check delimiter
    xor r9, r9

.check_delimiter:

    cmp r9, r8
    je .delimiter_found

    mov al, byte [r10 + r9]

    test al, al
    jz .copy_character

    cmp al, byte [r13 + r9]
    jne .copy_character

    inc r9
    jmp .check_delimiter

.copy_character:

    mov al, byte [r10]
    mov byte [r11], al

    inc r10
    inc r11

    jmp .scan

.delimiter_found:

    ; Null terminate current token
    mov byte [r11], 0

    ; Move input past delimiter
    add r10, r8

    ; Move output past the null terminator
    inc r11

    ; One token completed
    inc rcx

    jmp .next_token


.single_token:

    mov qword [r14 + 8], r15

    mov r10, r12
    mov r11, r15

.copy_single:

    mov al, byte [r10]
    mov byte [r11], al

    test al, al
    jz .single_finish

    inc r10
    inc r11

    jmp .copy_single

.single_finish:

    mov qword [r14], 1
    mov rax, r14

    jmp .restore


.finish:

    ; Terminate final token
    mov byte [r11], 0

    ; Include final token
    inc rcx

    mov qword [r14], rcx

    mov rax, r14


.restore:

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx

    ret

string_join:
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14

    mov r12, rdi
    mov r13, rsi

    mov r8, [r12]

    lea rdi, [rel join_buffer]

    xor r10, r10
    xor r11, r11

    test r8, r8
    jz .finish

.next_element:
    cmp r10, r8
    jae .finish

    cmp r10, 0
    je .get_element

    mov rbx, r13

.copy_delimiter:
    mov al, byte [rbx]

    test al, al
    jz .get_element

    mov byte [rdi + r11], al

    inc rbx
    inc r11

    jmp .copy_delimiter

.get_element:
    mov rbx, [r12 + r10 * 8 + 8]

    test rbx, rbx
    jz .finish

.copy_string:
    mov al, byte [rbx]

    test al, al
    jz .element_done

    mov byte [rdi + r11], al

    inc rbx
    inc r11

    jmp .copy_string

.element_done:
    inc r10

    jmp .next_element

.finish:
    mov byte [rdi + r11], 0

    mov rax, rdi

    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ret