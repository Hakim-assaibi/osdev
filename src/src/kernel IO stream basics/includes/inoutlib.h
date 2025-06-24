/*
	Bibliothèque qui gère l'entrée et la sortie (que la sortie pour l'instant) grâce au mode texte VGA
*/

#ifndef SCREEN_H
#define SCREEN_H

static inline void outb(unsigned short port, unsigned char val);
static inline char inb(unsigned short port);
char* scan();
void setCursor(unsigned short pos);
void print(char* msg, unsigned short type);
void scroll();
void eraseChr();
void emptyStr();
void bprint(char** msg, unsigned short type);

#endif
