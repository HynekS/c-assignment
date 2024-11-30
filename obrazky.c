#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define BEZ_CHYBY
#define MIN_VALUE 0
#define MAX_VALUE 4
#define MAXBUFLEN 100000


const char SYMBOLS[] = {' ', '.', ':', '+', '#'};

typedef enum operace {
  NEGATIV,
  ZMENA_JASU,
  ZMENA_KONTRASTU
} operace;

typedef struct {
  int h;
  int w;
  short** data;
} obrazek;

obrazek inicializace(int h, int w) {
  short **data = (short**)malloc(h * sizeof(short*));
  if (data == NULL) {
    // handle error
  }
  for (int i = 0; i < h; i++) {
    data[i] = (short *)malloc(w * sizeof(short));
    if (data[i] == NULL) {
      // handle error
    }
  }

  return (obrazek){h, w, data};
}

obrazek cerny(int h, int w) {
  obrazek instance = inicializace(h, w);

  for (int i = 0; i < h; i++)
        for (int j = 0; j < w; j++) {
            instance.data[i][j] = 4;
        }

  return instance;
}

void odstran(obrazek obr) {
  for (int i = 0; i < obr.h; i++) {
    free(obr.data[i]);
  }
    
  free(obr.data);
}

void print_value(short value) {
  if (value >= 0 && value < sizeof(SYMBOLS)) {
        printf("%c", SYMBOLS[value]);
      } else {
        printf("?");
      }
}

void zobraz(obrazek obr) {
  for(int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      print_value(n);
    }
    printf("\n");
  }
}

obrazek otoc90(obrazek obr) {
  // Swap width and height
  obrazek instance = inicializace(obr.w, obr.h);

  int ukazatel_nove_vysky = instance.h - 1;
  int ukazatel_nove_sirky = 0;

  for(int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      instance.data[ukazatel_nove_vysky][ukazatel_nove_sirky] = n;
      --ukazatel_nove_vysky;
      if(ukazatel_nove_vysky < 0) {
        ukazatel_nove_vysky = instance.h - 1;
        ++ukazatel_nove_sirky;
    }
  }
}
return instance;
}

obrazek morfing(obrazek obr1, obrazek obr2) {
  if(obr1.h != obr2.h || obr1.w != obr2.w) {
    // todo handle error
    return obr1;
  }
  obrazek instance = inicializace(obr1.h, obr1.w);
  for(int i = 0; i < obr1.h; i++) {
    for (int j = 0; j < obr1.w; j++) {
      short n1 = obr1.data[i][j];
      short n2 = obr2.data[i][j];

      short morphed = (n1 + n2) / 2;
      instance.data[i][j] = morphed;
    }
  }
  return instance;
}

short min(obrazek obr) {
  short currentMin = MAX_VALUE;
  for(int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
     if(n < currentMin) currentMin = n; 
    }
  }
  return currentMin;
}

short max(obrazek obr) {
  short currentMax = MIN_VALUE;
  for(int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
     if(n > currentMax) currentMax = n; 
    }
  }
  return currentMax;
}

short normalize(double d) {
  if(d < MIN_VALUE) return MIN_VALUE;
  if(d > MAX_VALUE) return MAX_VALUE;
  return (short) d;
}

obrazek jasova_operace(obrazek obr, operace o, ...) {
  int delta = 0;
  double multiplier = 1.0;
  int addition = 0;

  va_list args;
  va_start(args, o);
  if(o == ZMENA_JASU) {
    delta = va_arg(args, int);
  }
  if(o == ZMENA_KONTRASTU) {
    // TODO check if the values exists and are correct
    multiplier = va_arg(args, double);
    addition = va_arg(args, int);
  }
  va_end(args);

  for(int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short pixel = obr.data[i][j];
      switch(o) {
        case NEGATIV: {        
          // Todo check if the result is in range?
          obr.data[i][j] = normalize(MAX_VALUE - pixel);
          break;
        }
        case ZMENA_JASU: {
          obr.data[i][j] = normalize(pixel + delta);
          break;
        }
        case ZMENA_KONTRASTU: {
          obr.data[i][j] = normalize(pixel * multiplier + addition);
          break;
        }
      }
    }
  }
  return obr;
}

obrazek nacti_ze_souboru(const char *soubor) {
  FILE *file = fopen(soubor, "r");
  if(file == NULL) {
    // set error
    printf("error reading file");
    return inicializace(0, 0);
  }
  size_t index = 0;
  int lineCount = 0;
  int charCount = 0;
  int firstLineLength = 0;

  char c;

  char buffer[MAXBUFLEN + 1];

  do {
    c = getc(file);
    buffer[index] = c;
    index++;
    // Just ignore empty lines
    if (c == '\n' && charCount > 0) {
      if(firstLineLength == 0) {
        firstLineLength = charCount;
      }
      if(firstLineLength > charCount) {
        printf("Uneven line length!");
        // TODO throw? early return?
      }  
      charCount = 0;
      lineCount++;
    } else {
      charCount++;
    }
    
  } while (c != EOF);

  if(lineCount == 0) {
    // Todo throw or return empty image?
  }

  int pixelCount = (firstLineLength + 1) / 2;
  printf("%i \n", firstLineLength);

  obrazek instance = inicializace(lineCount, pixelCount);

  int bufferPointer = 0;

  for(int i = 0; i < lineCount; i++) {
    for(int j = 0; j < pixelCount; j++) {
      instance.data[i][j] = atoi(&buffer[bufferPointer]);
      printf("%i ", atoi(&buffer[bufferPointer]));
      bufferPointer += 2;
    }
    printf("\n");
  }

  fclose(file);

  return instance;
}

void uloz_do_souboru(obrazek obr, const char *soubor) {
  FILE *file = fopen(soubor, "w");
  if(file == NULL) {
    // todo handle error
  }
  int lineLength = obr.w / 2 + 1;

  for(int i = 0; i < obr.h; i++) {
    char buffer[lineLength];
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];

      buffer[j * 2] = SYMBOLS[n];
      if(j != lineLength) buffer[j * 2 + 1] = ' ';
    }
    fputs(buffer, file);
    fputs("\n", file);
  }

  fclose(file);
}

int vyska(obrazek obr) {
  return obr.h;
}

int sirka(obrazek obr) {
  return obr.w;
}

char prvek(obrazek obr, int i, int j) {
  return SYMBOLS[obr.data[i][j]];
}

void nastav_prvek(obrazek obr, int i, int j, short hodnota) {
  if(hodnota > MAX_VALUE || hodnota < MIN_VALUE) {
    // normalize?
    return;
  }
  // obr.data[i][j] = normalize(hodnota);
  obr.data[i][j] = hodnota; 
}

int main() {
  //obrazek test = cerny(4, 2);
//
  //zobraz(test);
//
  //obrazek test2 = otoc90(test);
//
  //obrazek test3 = jasova_operace(test, NEGATIV);
  //zobraz(test3);
//
  //obrazek test4 = jasova_operace(test, ZMENA_JASU, -3);
  //zobraz(test4);
//
  //obrazek test5 = jasova_operace(test, ZMENA_KONTRASTU, 2.0, 4);
  //zobraz(test5);
//
  //zobraz(test2);
  obrazek test6 = nacti_ze_souboru("./test.txt");
  zobraz(test6);

  obrazek test7 = otoc90(test6);
  zobraz(test7);

  uloz_do_souboru(test7, "./output.txt");
}