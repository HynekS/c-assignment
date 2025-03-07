// Zápočtová úloha z KMI/XJC
// Autor: Hynek Švácha

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "obrazky.h"

#define MAXBUFLEN 100000

STATE chyba = BEZ_CHYBY;

obrazek inicializace(int h, int w) {
  short **data = (short **)malloc(h * sizeof(short *));
  if (data == NULL) {
    chyba = CHYBA_ALOKACE;
    return (obrazek){0, 0, NULL};
  }

  for (int i = 0; i < h; i++) {
    data[i] = (short *)malloc(w * sizeof(short));

    if (data[i] == NULL) {
      for (int j = 0; j < i; j++) {
        free(data[j]);
      }
      free(data);
      chyba = CHYBA_ALOKACE;
      return (obrazek){0, 0, NULL};
    }
  }
  return (obrazek){h, w, data};
}

obrazek cerny(int h, int w) {
  obrazek result = inicializace(h, w);

  for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++) {
      result.data[i][j] = MIN_VALUE;
    }
  return result;
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
  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      print_value(n);
    }
    printf("\n");
  }
  printf("\n");
}

obrazek otoc90(obrazek obr) {
  // Notice: Width and height are swapped here
  obrazek result = inicializace(obr.w, obr.h);

  int ukazatel_nove_vysky = result.h - 1;
  int ukazatel_nove_sirky = 0;

  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      result.data[ukazatel_nove_vysky][ukazatel_nove_sirky] = n;
      --ukazatel_nove_vysky;
      if (ukazatel_nove_vysky < 0) {
        ukazatel_nove_vysky = result.h - 1;
        ++ukazatel_nove_sirky;
      }
    }
  }
  return result;
}

obrazek morfing(obrazek obr1, obrazek obr2) {
  if (obr1.h != obr2.h || obr1.w != obr2.w) {
    chyba = CHYBA_TYPU;
    return obr1;
  }
  obrazek result = inicializace(obr1.h, obr1.w);
  for (int i = 0; i < obr1.h; i++) {
    for (int j = 0; j < obr1.w; j++) {
      short p1 = obr1.data[i][j];
      short p2 = obr2.data[i][j];

      short morphed = zaokrouhli((p1 + p2) / 2);
      result.data[i][j] = morphed;
    }
  }
  return result;
}

short min(obrazek obr) {
  short currentMin = MAX_VALUE;
  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      if (n < currentMin)
        currentMin = n;
    }
  }
  return currentMin;
}

short max(obrazek obr) {
  short currentMax = MIN_VALUE;
  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      if (n > currentMax)
        currentMax = n;
    }
  }
  return currentMax;
}

short zaokrouhli(double d) { return (short)(d < 0 ? (d - 0.5) : (d + 0.5)); }

short normalize(double d) {
  if (d < MIN_VALUE)
    return MIN_VALUE;
  if (d > MAX_VALUE)
    return MAX_VALUE;
  return zaokrouhli(d);
}

obrazek jasova_operace(obrazek obr, operace o, ...) {
  int delta = 0;
  double multiplier = 1.0;
  int addend = 0;

  va_list args;
  va_start(args, o);
  if (o == ZMENA_JASU) {
    delta = va_arg(args, int);
  }
  if (o == ZMENA_KONTRASTU) {
    multiplier = va_arg(args, double);
    addend = va_arg(args, int);
  }

  va_end(args);

  obrazek result = inicializace(obr.h, obr.w);

  double mean = (MAX_VALUE - MIN_VALUE) / 2;

  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short pixel = obr.data[i][j];
      switch (o) {
      case NEGATIV: {
        result.data[i][j] = normalize(MAX_VALUE - pixel);
        break;
      }
      case ZMENA_JASU: {
        result.data[i][j] = normalize(pixel + delta);
        break;
      }
      case ZMENA_KONTRASTU: {
        result.data[i][j] =
            normalize((pixel - mean) * multiplier + mean + addend);
        break;
      }
      }
    }
  }

  return result;
}

obrazek nacti_ze_souboru(const char *soubor) {
  FILE *file = fopen(soubor, "r");
  if (file == NULL) {
    chyba = CHYBA_OTEVRENI;
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

    if (c == '\n' && charCount > 0) {
      if (firstLineLength == 0) {
        firstLineLength = charCount;
      }
      if (firstLineLength != charCount) {
        chyba = CHYBA_JINA;
        return inicializace(0, 0);
      }
      charCount = 0;
      lineCount++;
    } else {
      charCount++;
    }

  } while (c != EOF);

  // Handle the case when trailing newline is not present (so the last line ends
  // with EOF instead of '\n')
  if (charCount > 1) {
    if (firstLineLength != charCount) {
      chyba = CHYBA_JINA;
      return inicializace(0, 0);
    }
    lineCount++;
  }

  if (lineCount == 0) {
    chyba = CHYBA_TYPU;
    return inicializace(0, 0);
  }

  int pixelCount = (firstLineLength + 1) / 2;

  obrazek result = inicializace(lineCount, pixelCount);

  int bufferPointer = 0;

  for (int i = 0; i < lineCount; i++) {
    for (int j = 0; j < pixelCount; j++) {
      result.data[i][j] = atoi(&buffer[bufferPointer]);
      bufferPointer += 2;
    }
  }

  int closed_file = fclose(file);
  if (closed_file == EOF) {
    chyba = CHYBA_ZAVRENI;
  }

  return result;
}

void uloz_do_souboru(obrazek obr, const char *soubor) {
  FILE *file = fopen(soubor, "w");
  if (file == NULL) {
    chyba = CHYBA_OTEVRENI;
    return;
  }

  for (int i = 0; i < obr.h; i++) {
    for (int j = 0; j < obr.w; j++) {
      short n = obr.data[i][j];
      fputc(SYMBOLS[n], file);
    }
    fputs("\n", file);
  }

  int closed_file = fclose(file);
  if (closed_file == EOF) {
    chyba = CHYBA_ZAVRENI;
  }
}

int vyska(obrazek obr) { return obr.h; }

int sirka(obrazek obr) { return obr.w; }

char prvek(obrazek obr, int i, int j) { return SYMBOLS[obr.data[i][j]]; }

void nastav_prvek(obrazek obr, int i, int j, short hodnota) {
  if (i < 0 || i > obr.w || j < 0 || j > obr.h || hodnota > MAX_VALUE ||
      hodnota < MIN_VALUE) {
    chyba = CHYBA_TYPU;
    return;
  }
  obr.data[i][j] = hodnota;
}