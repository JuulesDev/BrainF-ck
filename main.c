#include <stdio.h>
#include <stdlib.h>

#define PARSED_CODE_SIZE 50

int isPartOfBf(char c)
{
  char *bfChars = "><+-,.[]";
  int i = 0;

  while (bfChars[i] != '\0')
  {
    if (bfChars[i] == c)
      return 1;
    ++i;
  }
  return 0;
}

int parse(char **target, int *targetLength, FILE *fp)
{
  int i = 0;
  char readChar;

  do
  {
    readChar = fgetc(fp);

    if (isPartOfBf(readChar))
    {
      (*target)[i++] = readChar;
    }

    if (i >= *targetLength)
    {
      *targetLength = 2 * (*targetLength);
      char *tmp = realloc(*target, sizeof(char) * (*targetLength));

      if (tmp == NULL)
      {
        printf("Failure: failed to augment allocated memory.\n");
        free(*target);
        return EXIT_FAILURE;
      }

      *target = tmp;
    }
  } while (readChar != EOF);

  return EXIT_SUCCESS;
}

int main(void)
{
  char *filename = "./tests/test.bf";
  FILE *fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("Failure: specified file could not be open.\n");
    return EXIT_FAILURE;
  }

  int parsedCodeLength = PARSED_CODE_SIZE;
  char *parsedCode = malloc(sizeof(char) * parsedCodeLength);

  if (parsedCode == NULL)
  {
    printf("Failure: failed to allocate memory.\n");
    return EXIT_FAILURE;
  }

  int failedParsing = parse(&parsedCode, &parsedCodeLength, fp);
  fclose(fp);

  if (failedParsing)
  {
    printf("Failure: failed to parse code.\n");
    return EXIT_FAILURE;
  }

  printf("; Parsed Code Length : %d\n", parsedCodeLength);
  printf("; Parsed code : '%s'\n", parsedCode);

  free(parsedCode);
  return EXIT_SUCCESS;
}