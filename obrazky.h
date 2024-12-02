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

typedef enum operace {
  NEGATIV,
  ZMENA_JASU,
  ZMENA_KONTRASTU
} operace;

const char SYMBOLS[] = {' ', '.', ':', '+', '#'};

typedef struct {
  int h;
  int w;
  short** data;
} obrazek;

obrazek inicializace(int h, int w);

obrazek cerny(int h, int w);

void odstran(obrazek obr);

void print_value(short value);

void zobraz(obrazek obr);

obrazek otoc90(obrazek obr);

obrazek morfing(obrazek obr1, obrazek obr2);

short min(obrazek obr);

short max(obrazek obr);

short normalize(double d);

obrazek jasova_operace(obrazek obr, operace o, ...);

obrazek nacti_ze_souboru(const char *soubor);

void uloz_do_souboru(obrazek obr, const char *soubor);

int vyska(obrazek obr);

int sirka(obrazek obr);

char prvek(obrazek obr, int i, int j);

void nastav_prvek(obrazek obr, int i, int j, short hodnota);
