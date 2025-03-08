// Zápočtová úloha z KMI/XJC
// Autor: Hynek Švácha

#ifndef OBRAZKY_H

#define MIN_VALUE 0
#define MAX_VALUE 4
typedef enum {
  BEZ_CHYBY,
  CHYBA_ALOKACE,
  CHYBA_OTEVRENI,
  CHYBA_ZAVRENI,
  CHYBA_TYPU,
  CHYBA_JINA
} STATE;
extern STATE chyba;
typedef enum { NEGATIV, ZMENA_JASU, ZMENA_KONTRASTU } OPERACE;

static const char SYMBOLS[] = {' ', '.', ':', '+', '#'};

typedef struct {
  int h;
  int w;
  short **data;
} obrazek;

extern obrazek inicializace(int h, int w);

extern obrazek cerny(int h, int w);

extern void odstran(obrazek obr);

extern void print_value(short value);

extern void zobraz(obrazek obr);

extern obrazek otoc90(obrazek obr);

extern obrazek morfing(obrazek obr1, obrazek obr2);

extern short min(obrazek obr);

extern short max(obrazek obr);

extern obrazek jasova_operace(obrazek obr, OPERACE o, ...);

extern obrazek nacti_ze_souboru(const char *soubor);

extern void uloz_do_souboru(obrazek obr, const char *soubor);

extern int vyska(obrazek obr);

extern int sirka(obrazek obr);

extern char prvek(obrazek obr, int i, int j);

extern void nastav_prvek(obrazek obr, int i, int j, short hodnota);

#endif