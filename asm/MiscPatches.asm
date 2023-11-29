.code


; Fixes Ginsu engine audio above 30 FPS
GinsuPatchCave proc
 
	mov      eax, [rbx+8Ch]          ; eax = mOutputSamplesRequested
    movd     xmm2, eax               
    movaps   xmm1, xmm3              ; slewSamplesRemaining
    cvtdq2ps xmm2, xmm2              ; (float)mOutputSamplesRequested
    subss    xmm1, xmm2              ; slewSamplesRemaining -= outputSamplesRequested
    comiss   xmm1, xmm10             
    jnb      DontSetZero             ; if (slewSamplesRemaining >= 0.f)
    movaps   xmm1, xmm10             ; slewSamplesRemaining = 0.f
    DontSetZero:                     
    mov      eax, [rbx+224h]         ; eax = mfCurrentFrequency
 
GinsuPatchCave endp
 
end