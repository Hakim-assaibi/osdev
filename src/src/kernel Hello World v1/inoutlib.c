#include <inoutlib.h>
#define MAX_COL 80
#define MAX_ROW 25

//le mode texte VGA permet d'afficher des caractères selon un tableau de 80 colonnes * 25 lignes

static unsigned short row = 0; 
static unsigned short col = 0;


//Fonction qui envoie des des données à un port
static inline void outb(unsigned short port, unsigned char val) //l'adresse du port fait 16 bits et le port lui meme ne peut prendre que 8, d'ou un char pour la valeur
{
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port)); //utilise la fonction outb d'Intel, on met la valeur dans le registre al et le port dans dx ou bien en valeur immediate
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
        //si il y a un retour à la ligne et qu'on dépasse pas la ligne 25 (théoriquement impossible mais ne sait-on jamais) on remet la colonne à zero et on va à la prochaine ligne 
        if(msg[i] == '\n' && row < MAX_ROW) 
        {
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