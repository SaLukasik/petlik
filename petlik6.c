/*petlik6.c 

> Program zaliczeniowy na zajęcia laboratoryjne WPI.

>Program przyjmuje komendy w postaci języka pętlik na 26 zmiennych oraz polecenia wypisania jednej z nich
Program na wyjściu podaje liczby, których wypisanie mu polecono

> autor: Sara Łukasik
> data: 19.01.2020 r.
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum Polecenie{    /*Polecenie zawiera 7 instrukcji, wykonywanych przez program- 6 z języka pętlik, a WP jest instrukcją wypisania zmiennej*/
    WP, INC, ADD, CLR, JMP, DJZ, HLT
};

typedef struct instrukcja{ 
    enum Polecenie funkcja; /*Jakie polecenie ma wykonać program*/
    int z1;  /*z1 i z2 zależą od funkcji i są albo adresem (stąd int, ponieważ adres może być długi) 
             albo liczbą od 0-25 symbolizującą zmienną, na której operuje funkcja*/
    int z2; 
}instyp;

typedef struct zmienna{   /*definicja struct, w którym przechowywane będą dane o zmiennych a,b,c,d...*/  
        int dlugosc;   /*długość (czyli ile zaalokowanje pamięci zostało wykorzystane), uwaga! dlugosc liczby = 0 wynosi 1!*/
        int size;    /*size (czyli rozmiar zaalokowanej pamięci)*/
        short *liczba;   /*wskaźnik do tablicy zachowującej liczby typu short*/
}zmientyp;

/*dopisz jest funkcją, która bierze tablicę, gdzie przechowywany jest program i dopisuje do jej końca odpowiednią funckję, 
przesuwa *pozycja (miejsce, które jest puste aż do uzupełnienia go hlt) o jeden i gdyby tablica programu była za krótka - powiększa ją*/
instyp *dopisz(instyp *Program,int *pozycja, int *rozmiar,enum Polecenie funkcja, int zjeden, int zdwa){
    
    Program[*pozycja].funkcja = funkcja;
    Program[*pozycja].z1 = zjeden;
    Program[*pozycja].z2 = zdwa;  
    *pozycja += 1;
    if (*pozycja == *rozmiar){
        *rozmiar = 2 * (*rozmiar);    
    }

    Program = realloc(Program,(unsigned) (*rozmiar) * sizeof *Program);
    return Program;
}

/*rekurencja jest funkcją, która pozwala odpowiednio zapisać funckje w tablicy programu dla kodu pętlik zawierającego nawiasy*/
instyp *rekurencja(instyp *Program, int *pozycja, int *rozmiar){
    
    int start = *pozycja;
    int pomoc = (getchar() - 97);
    Program = dopisz(Program, pozycja, rozmiar, DJZ, pomoc, -1);
    /*na początku funkcja zapisuje kod który nie jest zoptymalizowany, jeśli warunki do optymalizacji kodu zajdą, to znaczy wyrażeniu nawiasowym nie
    bedzie innych nawiasów ani zmiennej będącej w warunkującej while bool nawias pozostanie false i rekurencja uprości kod maszynowy*/
    bool nawias = false; 
    int znak = 0;

    while ((znak = getchar()) != 41){
        
        if (znak == 40){
            nawias = true;
            Program = rekurencja(Program, pozycja, rozmiar);
        }
        else{ 
            if ((znak - 97) == (Program[start].z1)){
                
                nawias = true;
            }
            Program = dopisz(Program, pozycja, rozmiar, INC, (znak - 97), -1);             
        }    
    }
    
    if (nawias == false){  /*uproszczenie kodu maszynowego*/
             
        int i = start; 
        int co = Program[start].z1;

        while (i != (*pozycja - 1)){
            Program[i].funkcja = ADD;
            Program[i].z1 = Program[i + 1].z1;
            Program[i].z2 = co;
            ++i;
        }
        Program[i].funkcja = CLR;
        Program[i].z1 = co;    
    } 
    else{    
        Program = dopisz(Program, pozycja, rozmiar, JMP, start, -1);
        Program[start].z2 = *pozycja;        
    }
    return Program;
}

/*czytaj jest funkcją, która czyta znaki i odpowiednio je interpretuje zapisując na kod maszynowy. 
Zwraca gotową tablicę Program, która zawiera działający kod maszynowy*/
instyp *czytaj(instyp *Program,char* kontroluj) {
    
    int rozmiar = 1;
    int pozycja = 0; /*a rozmiar tablicy to pozycja+1*/
    *kontroluj = 0;
    int i;
    int znak;
    
    for (i = 0; *kontroluj == 0; ++i) {
        znak = getchar();
        if (znak == EOF){
            *kontroluj = 1;
        }
        else if (znak == 10){
            *kontroluj = 2;
        }
        else if (znak == 61){
            
            Program = dopisz(Program, &pozycja, &rozmiar,WP, (getchar() - 97), -1);     
        }
        else{ /*opcja kiedy jest litera, ( lub )*/
            
            
            if (znak >= 97){ /*znak to litera*/
                
                Program = dopisz(Program, &pozycja, &rozmiar, INC, (znak - 97), -1);
            }
            else{ /*nawias, wchodzimy w rekurencję*/
                
                Program = rekurencja(Program, &pozycja, &rozmiar);
                
            }            
        }        
    }
    /*każdy program, nawet zawierający intrukcję WP, nie należącą do pętlika kończy się HLT*/
    Program[pozycja].funkcja = HLT;
    Program[pozycja].z1 = -1;
    Program[pozycja].z2 = -1;
    
    return Program;        
}


/*funkcja działająca na zmiennej a-z. pozwala wyzerować nowo utworzone miejsca w tablicy, co jest niezbędne podczas wykonywania polecenia ADD*/
void zeruj(zmientyp* Zmienne, int nr){
    int i = Zmienne[nr].dlugosc;
    while (i < Zmienne[nr].size){
        Zmienne[nr].liczba[i] = 0;
        ++i;
    }
}

/*dzięki tej funkcji nie skończy się miejsce w tablicy, funkcja zwiększa tablicę zmiennej, gdy ta zbliża się do jej maksymalnej pojemności*/
void sprawdzrozmiar (zmientyp* Zmienne, int nr){  
    if (Zmienne[nr].size < Zmienne[nr].dlugosc + 2){
        Zmienne[nr].size = 2 * Zmienne[nr].size;
        Zmienne[nr].liczba = realloc(Zmienne[nr].liczba,(unsigned) (Zmienne[nr].size) * sizeof Zmienne[nr].liczba);
        zeruj(Zmienne, nr);                    
    }
}

/*funkcja wykonaj bierze tablicę Program z kodem maszynowym i wykonuje ten kod operując na zmiennych a-z*/
instyp* wykonaj(instyp *Program, zmientyp* Zmienne){  
    
    int i = 0; /*obecne położenie w tablicy program*/
    int s = 0;
    bool stop = false;
    short mem = 0;
    short mem2 = 0;
    /*tutaj while, które będzie zatrzymywane przez HLT*/
    while (stop == false){   
        int z1 = Program[i].z1;
        int z2 = Program[i].z2;
        switch (Program[i].funkcja){
            /*polecenie wypisania liczby*/
            case WP:                
                s = Zmienne[z1].dlugosc;
                
                printf("%hi", Zmienne[z1].liczba[s-1]);
                --s;
                while (s>0){
                    printf("%04hi", Zmienne[z1].liczba[s-1]);
                    --s;
                }
                printf("\n");
                break;
            
            case INC: /*zwiekszenie o jeden*/
                sprawdzrozmiar (Zmienne, z1);
                s = 0;
                while (Zmienne[z1].liczba[s] == 9999){
                    Zmienne[z1].liczba[s] = 0;
                    ++s;
                }
                Zmienne[z1].liczba[s] = (short)(Zmienne[z1].liczba[s] + 1);
                if (Zmienne[z1].dlugosc == s){
                    Zmienne[z1].dlugosc += 1;
                }
         
                break;
            case ADD: /*dodaje zmienną z2 do zmiennej z1*/          
                mem = 0;
                mem2 = 0;
                sprawdzrozmiar (Zmienne, z1);
                sprawdzrozmiar (Zmienne, z2);
                if (Zmienne[z2].size > Zmienne[z1].size){
                    
                    Zmienne[z1].size = Zmienne[z2].size;
                    Zmienne[z1].liczba = realloc(Zmienne[z1].liczba,(unsigned) (Zmienne[z1].size) * sizeof Zmienne[z1].liczba);
                    zeruj(Zmienne, z1);
                }
                else if (Zmienne[z2].size < Zmienne[z1].size){
                    
                    Zmienne[z2].size = Zmienne[z1].size;
                    Zmienne[z2].liczba = realloc(Zmienne[z2].liczba,(unsigned) (Zmienne[z2].size) * sizeof Zmienne[z2].liczba);
                    zeruj(Zmienne, z2);
                }
                s=0;
                while ((s<Zmienne[z2].dlugosc) || (s<Zmienne[z1].dlugosc)){
                    
                    mem = (short)(Zmienne[z1].liczba[s] + Zmienne[z2].liczba[s] + mem2);
                    if (mem > 9999){
                        mem2 = 1;
                        mem = (short)(mem - 10000);
                    }
                    else{
                        mem2 = 0;
                    }
                    Zmienne[z1].liczba[s] = mem;
                    ++s;     

                }
                if (Zmienne[z2].dlugosc > Zmienne[z1].dlugosc){
                    Zmienne[z1].dlugosc = Zmienne[z2].dlugosc;
                }
                if (mem2 == 1){
                    Zmienne[z1].dlugosc += 1;
                    Zmienne[z1].liczba[s] = 1;
                }
                break;

            case CLR: /*czyści zmienną*/
                Zmienne[z1].liczba = realloc((Zmienne[z1].liczba), 2 * sizeof Zmienne[z1].liczba);
                Zmienne[z1].liczba[0] = 0;
                Zmienne[z1].liczba[1] = 0;
                Zmienne[z1].size = 2;
                Zmienne[z1].dlugosc = 1;
                break;

            case JMP: /*tu trzeba przezornie zmniejszyć adres o 1, bo pod koniec pętli jest ++i*/
                i = z1 - 1;
                break;

            case DJZ: /*zmniejsza o jeden lub gdy zmienna = 0 skacze na adres w z2*/                
                if (Zmienne[z1].dlugosc == 1 &&  Zmienne[z1].liczba[0] == 0){
                    i = z2 - 1;
                }
                else{
                    
                    s = 0;
                    while(Zmienne[z1].liczba[s] == 0){
                        ++s;
                    }
                    if (Zmienne[z1].liczba[s] == 1){
                        if (s == (Zmienne[z1].dlugosc-1) && (Zmienne[z1].dlugosc != 1)){
                            Zmienne[z1].dlugosc = (short)(Zmienne[z1].dlugosc - 1);
                        }
                        Zmienne[z1].liczba[s] = 0;
                        s -= 1;
                        while (s >= 0){
                            Zmienne[z1].liczba[s] = 9999;
                            --s;
                        }

                    }
                    else{
                        
                        Zmienne[z1].liczba[s] = (short)(Zmienne[z1].liczba[s] - 1);
                    }                    
                }
                
                break;
            case HLT: /*znak, że program się skończył*/
                stop = true;
                Program = realloc(Program,(unsigned) 1 * sizeof *Program);

                break;
        
        }
        ++i;
    } 
    return Program;
}

int main(void){

    char kontroluj = 0;
    zmientyp Zmienne[26]; /*w tej tablicy przechowywane będą zmienne*/
    /*inicjalizacja zmiennych*/
    for (int i = 0; i<26; ++i){
        Zmienne[i].liczba = malloc((size_t)1*sizeof Zmienne[i].liczba);
        Zmienne[i].liczba[0] = 0;
        Zmienne[i].dlugosc = 1; 
        Zmienne[i].size = 1;       
    }
    /*pętla wykonuje się aż pętlik nie wczyta EOF*/
    do{
        instyp *Program = malloc((unsigned) 1 * sizeof *Program);

        Program = czytaj(Program,&kontroluj);

        Program = wykonaj(Program, Zmienne);
        free(Program); 
              
    }
    while(kontroluj != 1);

    for (int i = 0; i<26; ++i){
        free(Zmienne[i].liczba);     
    }
    return 0;
}