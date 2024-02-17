    opt	m+,l.,c+

    section data            ; Store the array in the data section

    global tim_my_image     ; Define label as global
tim_my_image:
    incbin 'assets/TEXTURE64.tim'   ; Include file data (your TIM)