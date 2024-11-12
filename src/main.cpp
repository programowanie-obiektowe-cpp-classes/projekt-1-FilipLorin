#include "gra.cpp"
#include <stdlib.h>
#include <time.h>


int main()
{
    srand(time(NULL));
    Gra gra;
    gra.drukuj_powitanie();
    while( gra.get_stan() ) { gra.akcja_gracza(); }
    return 0;
}
