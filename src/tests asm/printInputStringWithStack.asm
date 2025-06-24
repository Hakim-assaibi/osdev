[org 0x7c00] ;on commence a l'adresse 0x7c00 (important)

start:
	xor cx, cx ;xor -> ou exclusif, vaut 1 si un seul des deux arguments vaut 1, ici c'est impossible puisque les deux arguments sont la meme variable donc = 0, (ca prend moins d'octets que mov cx, 0)
	mov di, buffer ;on cree un pointeur sur la chaine de caractere

inputString: ;lit ce que l'utilisateur ecrit et le met dans la stack mais a l'envers
	xor ah, ah
	int 0x16 ;interruption pour lire l'input utilisateur (enregistré sur al)
	mov ah, 0x0e ;adresse affichage video
	int 0x10 ;interruption pour afficher le contenu de al
	xor ah, ah
	push ax ;on met le caractere dans la stack
	inc cx ;on incremente le compteur
	cmp al, 13 ;si le caractere est un retour a la ligne on quitte la fonction
	jne inputString ;sinon on jump sur cette meme fonction (une boucle while en C)

mov al, 10
mov ah, 0x0e
int 0x10 ;sauter une ligne

fillBuffer: ;vide la stack dans la chaine de caractere
	pop ax ;on recupere l'element en haut de la stack
	mov [di], al ;on met l'element dans la chaine de caractere
	inc di ;on avance dans la chaine de caractere
	dec cx ;decremente le compteur
	cmp cx, 0
	jne fillBuffer ;ici on a l'equivalent d'une boucle for en gros

buffer:
	times 256 db 0 ;on initialise 256 zero pour avoir de la marge

mov cx, 0
mov byte [di], 10
inc di
mov byte [di], 0 ;on ajoute une fin de chaine (pas vraiment utile puisqu'il ya 256 zero mais on est jamais trop sur j'ai de la marge niveau octets)
mov di, buffer ;on remet le pointeur a zero

pushBuffer: ;on met la chaine de caractere qui est l'input inversé dans la stack comme ca quand on va lire la stack on aura la chaine dans l'ordre
	mov al, [di]
	cmp al, 0 ;on ecrit quand on arrive en fin de chaine
	je finalPrint
	xor ah, ah
	push ax
	inc di
	inc cx
	jmp pushBuffer

finalPrint:
	pop ax
	mov ah, 0x0e
	int 0x10
	dec cx
	cmp cx, 0
	jne finalPrint
	
end:
	jmp $
	
times 510-($-$$) db 0 ;on ajoute des 0 jusqu a avoir 512 octets (la taille obligatoire du bootloader)
db 0x55, 0xaa ;signature du bootloader (sinon je peux pas booter sur mon code)