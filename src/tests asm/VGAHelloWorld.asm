org 0x7c00

;on efface l'écran (sinon il y aura Booting for Disk etc...)
mov ax, 0x0003 ; le premier octet de ax -> ah = 0 : changer le mode d'affichage, le 2eme al = 3 : numéro du mode utilisé (ici VGA)
int 0x10

CODE_SEG equ code_descriptor - GDT_Start
DATA_SEG equ data_descriptor - GDT_Start ;constantes pour calculer les adresses des segments

cli ;desactiver les interruptions du processeur

lgdt [GDT_Descriptor];activer la GDT
mov eax, cr0
or eax, 1 ;toujours 1 donc modifie le dernier bit à 1
mov cr0, eax ;modifie le dernier bit de cr0 à 1 pour switcher sur le mode protegé

jmp CODE_SEG:start_protected_mode ;far jump pour recharger CS et eviter que le pipelining execute des instructions en mode réel

jmp $

GDT_Start:;table GDT modele flat
	null_descriptor: ;descripteur vide
		dd 0 ; 4 octets a 0
		dd 0 ; 4 octets a 0
	code_descriptor: ;descripteur du segment Code
		dw 0xffff ;premiers 16 bits de la limite
		dw 0x0000 ;
		db 0x00 ; premiers 24 bits de la base
		db 0b10011010 ; 4 premiers bits : PPT -> Present : 1 (on utilise la segmentation)
					;						   Privilege : 00 (ring -> privileges maximum)
					;						   Type : 1 (code ou data segment)
					; 4 derniers bits : Type Flags -> Code : 1 (parce que on est dans le segment code)
					;								  Conforming : 0 (On est dans le ring 0 donc pas la peine de le mettre a 1)
					;								  Readable : 1 (Pour pouvoir lire)
					;								  Accessed : 0 (C'est le CPU qui gère on peut laisser à 0)
		db 0b11001111 ; 4 premiers bits : Other Flags -> Granularity : 1 (Grace à ça on pourra accéder à 4 Go de mémoire)
					; 									 32 bits : 1 (Pour utiliser une mémoire 32 bits)
					;									 Mode Long (64 bits) : 0
					;									 Reserved
					; 4 derniers bits : Les 4 derniers bits de la limite (20 bits) donc on a 0xFFFFF à la fin
		db 0x00 ; derniers 8 bits de la base
	data_descriptor:
		dw 0xffff ;premiers 16 bits de la limite
		dw 0x0000 ;
		db 0x00 ; premiers 24 bits de la base
		db 0b10010010 ; 4 premiers bits : PPT -> Present : 1 (on utilise la segmentation)
					;						   Privilege : 00 (ring -> privileges maximum)
					;						   Type : 1 (code ou data segment)
					; 4 derniers bits : Type Flags -> Code : 0 (parce que on est dans le segment data)
					;								  Conforming : 0 (On est dans le ring 0 donc pas la peine de le mettre a 1)
					;								  Writable : 1 (Pour pouvoir écrire)
					;								  Accessed : 0 (C'est le CPU qui gère on peut laisser à 0)
		db 0b11001111 ; 4 premiers bits : Other Flags -> Granularity : 1 (Grace à ça on pourra accéder à 4 Go de mémoire)
					; 									 32 bits : 1 (Pour utiliser une mémoire 32 bits)
					;									 Mode Long (64 bits) : 0
					;									 Reserved
					; 4 derniers bits : Les 4 derniers bits de la limite (20 bits) donc on a 0xFFFFF à la fin
		db 0x00 ; derniers 8 bits de la base
GDT_End:

GDT_Descriptor:
	dw GDT_End - GDT_Start - 1 ;calcul le debut de la GDT
	dd GDT_Start ;pointeur vers le debut
	


[bits 32]
start_protected_mode:
	;recharge les segments pour le mode protégé
	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	
	mov edi, 0xB8000 ;0xB8000 adresse du mode video de VGA
	mov esi, string


print:
	lodsb ;on met [esi] dans al et on incremente esi
	cmp al, 0
	je end
	mov ah, 0x0F ;code pour Blanc sur fond noir
	mov word [edi], ax ;on met ax à l'emplacement de notre pointeur, donc en haut a gauche de l'ecran au début
	add edi, 2 ;on ajoute deux et pas un car une 'case' VGA prend deux octets, le caractere et la maniere de l'afficher
	jmp print
	
end:
	jmp $

string db "Hello World !", 0

times 510-($-$$) db 0
db 0x55, 0xaa
