[org 0x7c00]

printRegs:
	pusha ;met la valeur de tous les registres dans la stack
	mov ah, 0x0e 
	mov bx, labelRegs 
	
printLabels:
	mov al, [bx];on met dans al le contenu du pointeur sur bx
	cmp al, 0
	je zeroESI
	int 0x10;afficher al
	inc bx
	jmp printLabels

labelRegs:
	db 10, 13, "DI: SI: BP: SP: BX: DX: CX: AX:", 10, 13, 0

zeroESI:
	xor esi, esi

popReg:
	inc esi
	cmp esi, 9
	je end
	pop ax ;enlever le haut de la stack pour le mettre dans ax

__printInt:
	mov bx, 10
	xor cx, cx

stackIntDigits:
	xor dx, dx
	div bx
	add dx, 48 ;le code ascii des chiffres sont de 48 à 57 donc en ajoutant 48 on obtient le code ascii du nombre
	push dx
	inc cx
	cmp ax, 0
	jne stackIntDigits

printStack:
	pop ax
	mov ah, 0x0e
	int 0x10
	dec cx
	cmp cx, 0
	jne printStack

printSpaceThenLoop:
	mov al, 32 ;code ascii pour l'espace
	int 0x10
	jmp popReg

end:
	jmp $
	
times 510-($-$$) db 0
db 0x55, 0xaa