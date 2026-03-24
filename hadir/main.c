#include "validation.h"

int main(int argc, char *argv[]) {
    if(initSDL() != 0) return 1;
    fenetre1();
    cleanSDL();
    return 0;
}
