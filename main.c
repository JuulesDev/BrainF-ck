#include <stdio.h>

void manageChar(char c)
{
  switch (c)
  {
  case '>':
    break;
  case '<':
    break;
  case '+':
    break;
  case '-':
    break;
  case '.':
    break;
  case ',':
    break;
  case '[':
    break;
  case ']':
    break;
  default:
    break;
  }
}

int main(void)
{
  char readChar;
  char *filename = "./tests/hello_world.bf";
  FILE *fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("Failure: specified file could not be open.\n");
    return 1;
  }

  do
  {
    readChar = fgetc(fp);
    manageChar(readChar);

  } while (readChar != EOF);

  fclose(fp);
  return 0;
}