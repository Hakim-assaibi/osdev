#include <inoutlib.h>
#define MAX_COL 80
#define MAX_ROW 25
#define MAX_STR_SIZE 128

//le mode texte VGA permet d'afficher des caractères selon un tableau de 80 colonnes * 25 lignes

static unsigned short row = 0; //ligne
static unsigned short col = 0; //colonnes
static unsigned short pcol = 0; //colonne à laquelle s'arrête la ligne précédente

static char scancodeTable[128] = { 
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',  // 0x00 - 0x0F
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n',    // 0x10 - 0x1D
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '"', '~',             // 0x1E - 0x2B
    0, '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '?', 0, '*', 0,       // 0x2C - 0x3B
    ' ' // 0x39 (space)
}; //pour convertir les scancode en caractères
static char scancodeTableMaj[128] = { 
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   // 0x00 - 0x0F
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n',    // 0x10 - 0x1D
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',             // 0x1E - 0x2B
    0, '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0,       // 0x2C - 0x3B
    ' ' // 0x39 (space)
};

static char str[MAX_STR_SIZE + 1]; //chaine pour l'entrée utilisateur


//Fonction qui envoie des des données à un port
static inline void outb(unsigned short port, unsigned char val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); //utilise la fonction outb d'Intel, on met la valeur dans le registre al et le port dans dx ou bien en valeur immediate
}

//Fonction qui récupere des données d'un port
static inline char inb(unsigned short port)
{
    char ret;
    asm volatile ("inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory"); //utilise la fonction inb d'Intel, on met recupere la valeur contenu dans un port
    return ret;
}


//Fonction qui met le curseur après le dernier caractère 
void setCursor(unsigned short pos)
{
    //On écrit des commandes dans les port 3D4 et 3D5 deux fois (car la position fait 16 bits et le port ne prend que 8 bits !! donc on envoie en deux fois) 
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

//Fonction qui vide la chaine contenant l'entrée utilisateur
void emptyStr()
{
    for(int i = 0;i < MAX_STR_SIZE;i++)
    {
        str[i] = 0;
    }
}

//Fonction qui lit une entrée utilisateur et la renvoie
char* scan()
{
    emptyStr(); //on vide l'entrée utilisateur précédente
    char continuer = 1;
    unsigned short i = 0; //va servir à pointer la chaine
    char shift = 0;
    char chr;
    char capsLockOn = 0;
    while(continuer) 
    {
        while(!(inb(0x64) & 1)){} //si on ne lit pas d'entrée on ne fait rien 0x64 est a 1 si une touche est pressée
        unsigned char scancode = inb(0x60); //on lit la touche pressée 0x60 est l'adresse qui contient la touche dans un port PS/2 
        if(scancode == 0x2A) { // Shift pressé
            shift = 1;
            continue;
        }
        if(scancode == 0xAA) { // Shift relâché
            shift = 0;
            continue;
        }
        if(scancode == 0x3A) { // Caps Lock
            capsLockOn = !capsLockOn;
            continue;
        }

        if(!(scancode & 0x80)) //on ne lit que si la touche est pressée et pas relachée !
        {       
            if(scancode != 0x0E) //0x0E est le scancode pour un retour arrière donc on écrit que si on n'efface pâs
            {
                if(shift && !capsLockOn)
                    chr = scancodeTableMaj[scancode];
                else if(shift && capsLockOn)
                    chr = scancodeTable[scancode];
                else if(!shift && capsLockOn)
                    chr = scancodeTableMaj[scancode];
                else
                    chr = scancodeTable[scancode]; //on convertit le scancode en lettre
                if(chr)
                {
                    if(chr != '\n')
                    {
                        str[i] = chr;
                        char ltr[2] = {chr, 0}; //ma fonction print ne prend que des char* donc je suis obligé de créer un char* pour la lettre que je veux afficher
                        print(ltr, 0);
                    }
                    else
                        str[i] = 0;
                    i++; 
                    if(chr == '\n'|| i >= MAX_STR_SIZE) //tant qu'on a pas appuyer sur entrée ou que la chaine ne fait pas 128 caractères on lit les scancodes
                    {
                        char ltr[2] = {'\n', 0};
                        print(ltr, 0);
                        str[i] = 0;
                        continuer = 0;
                    }
                }
            }
            else
            {
                if(i > 0) //empèche d'effacer au delà du premier caractère de la chaine (sinon on pourrait effacer ce qui a été ecrit avant cette fonction) 
                {
                    eraseChr();
                    i--;
                    str[i] = 0;
                }
            }
        }
    }
    return str;
}

//Fonction qui permet d'effacer un caractère
void eraseChr()
{
    volatile char* video = (volatile char*) 0xb8000; 
    if(col > 0)
        col--;
    else
    {
        if(row > 0) //on revient à la ligne d'avant si on est au bord de l'ecran
        {
            if (pcol>=MAX_COL)
                col=pcol-1;
            else
                col = pcol;
            row--;
        }
    }
    video[2*(row * MAX_COL + col)] = ' ';
    video[2*(row * MAX_COL + col) + 1] = 0x0F; 
    unsigned short cursorPos = row*MAX_COL+col; 
    setCursor(cursorPos);
}

//Fonction qui écrit une chaine de caractère (msg) à l'écran, il prend également un paramètre type pour choisir la couleur
void print(char* msg, unsigned short type)
{
    volatile char* video = (volatile char*) 0xb8000; //adresse où commence l'écran en VGA mode texte (ça correspond à la colonne 0, ligne 0 )
    char code; //code d'affichage
    switch(type) //le type est associé à la couleur
    {
        case 0: //normal text - white
            code =  0x0F;
            break;
        case 1: //error - red
            code =  0x04;
            break;
        case 2: //warning - yellow
            code =  0x0E;
            break;
        case 3: //other - blue
            code =  0x01;
            break;
        case 4: //other - green
            code =  0x02;
            break;
        case 5: //other - cyan
            code =  0x03;
            break;
        case 6: //other - magenta
            code =  0x05;
            break;
        case 7: //other - brown
            code =  0x06;
            break;
        case 8: //other - light gray
            code =  0x07;
            break;
        case 9: //other - dark gray
            code =  0x08;
            break;
        case 10: //other - light blue
            code =  0x09;
            break;
        case 11: //other - light green
            code =  0x0A;
            break;
        case 12: //other - light cyan
            code =  0x0B;
            break;
        case 13: //other - light red
            code =  0x0C;
            break;
        case 14: //other - light magenta
            code =  0x0D;
            break;
        default: //normal text - white
            code =  0x0F;
            break;
    }
    for (int i = 0; msg[i]; i++) {
        //si il y a un retour à la ligne et qu'on dépasse pas la ligne 25 on remet la colonne à zero et on va à la prochaine ligne 
        if(msg[i] == '\n' && row < MAX_ROW) 
        {
            pcol = col;
            col = 0;
            row++;
            if(row >= MAX_ROW)
            {
                scroll();
                row--;
            }
        }
        else
        {
            //si on dépasse la 80eme colonne on continue d'écrire dans la ligne d'après
            if(col >= MAX_COL && row < MAX_ROW)
            {
                pcol = col;
                col = 0;
                row++;
                if(row >= MAX_ROW)
                    {
                        scroll();
                        row--;
                    }
            }
            /*
                2*(row*MAX_COL+col) permet d'écrire à la colonne et à la ligne désiré, le *2 avant
                est là car une 'case' en VGA fait deux octets, un pour le caractère et un pour l'affichage
                donc pour aller de case en case on avance de deux en deux dans le pointeur de l'adresse vidéo.
                donc si la lettre est H et que le type est 0, le premier octet contiendra H et le deuxieme l'adresse de la couleur blanche
            */
            video[2*(row * MAX_COL + col)] = msg[i];
            video[2*(row * MAX_COL + col) + 1] = code; 
            col++;
        }
        
    }

    //permet de récupérer la position du curseur, pas de 2* ici car le curseur matériel est distinct de l'adressage des 'cases'
    unsigned short cursorPos = row*MAX_COL+col; 
    setCursor(cursorPos);
}

//Fonction qui permet de scroller quand on atteint la dernière ligne et que l'on veut afficher un caractère dans la ligne d'après
void scroll() 
{
    volatile char* video = (volatile char*) 0xb8000;

    //on copie une ligne et on l'affiche dans celle au dessus pour chaque ligne
    for(unsigned short crow = 1; crow < MAX_ROW; crow++)
    {
        for(unsigned short ccol = 0; ccol < MAX_COL; ccol++)
        {
            video[2 * ((crow - 1) * MAX_COL + ccol)] = video[2 * (crow * MAX_COL + ccol)];
            video[2 * ((crow - 1) * MAX_COL + ccol) + 1] = video[2 * (crow * MAX_COL + ccol) + 1];
        }
    }
    //On vide la dernière ligne pour y accueillir le texte à afficher
    for (unsigned short ccol = 0; ccol < MAX_COL; ccol++) 
    {
        video[2 * ((MAX_ROW - 1) * MAX_COL + ccol)] = ' ';
        video[2 * ((MAX_ROW - 1) * MAX_COL + ccol) + 1] = 0x0F;
    }
}

//Pemet d'afficher des variables et des constantes dans la mème fonctions (ca evite les multiples appels qui peuvent etre vite fatigants à la main)
void bprint(char** msg, unsigned short type)
{
    int i = 0;
    while (msg[i] != 0) {
        print(msg[i], type);
        i++;
    }
}