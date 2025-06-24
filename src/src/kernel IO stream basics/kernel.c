//Kernel

#include <inoutlib.h> //penser à le compiler avec -I

void main() {
    while (1) 
    {
        print("Commande > ",0);
        char* cmd = scan();
        bprint((char*[]){"Commande ", cmd, " executee ! \n", 0}, 0);
    }
}
