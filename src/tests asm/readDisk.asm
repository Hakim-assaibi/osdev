org 0x7c00
start:
	xor ax,ax
	mov es, ax
	mov ah, 2 ;code int 13h 2 pour lire 3 pour ecrire par exemple
	mov al, 1 ;nombre de secteurs à lire
	mov ch, 0 ;numero du cylindre a lire
	mov cl, 2 ;numero du secteur a lire (commence par 1 !!!)
	mov dh, 0 ;numero de la tete
	mov dl, [diskNum] ;numero du disque
	mov bx, 0x7e00 
	;on est sur 8086 en mode reel d'ou les 16 bits maximum
	;es:bx est un pointeur qui pointe sur l'adresse où l'on veut charger le secteur (es * 16 + bx) 
	;es est en rapport a la segmentation ce qui peut nous faire acceder à + de memoire (avec un registre de 16 bits c'est 64 ko normalement mais avec la segmentation on arrive a 1 Mo !!)
	;les segments sont decales de 16 octets d'où le *16
	;l'adresse max que peut retenir un registre est 0xFFFF (2 octets donc 16 bits)
	;avec la segmentation on peut aller jusqu'a 0xFFFF * 0xFFFF = 0x10FFEF
	;Néanmoins les bus d'adresses du 8086 vont jusqu'a 20 lignes don con est limité a 20 bits donc le max est 0xFFFFF environ 1 Mo
	int 0x13 ;interruption lecture
	mov ah, 0x0e
	mov al, [0x7e00]
	int 0x10
	jc erreur

erreur:
	mov ah, 0x0E
    mov al, 'X'
    int 0x10
    jmp $	
	
diskNum:
	db 0x80
end:
	jmp $
	
times 510-($-$$) db 0
db 0x55, 0xaa
