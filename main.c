#include <stdio.h>
#include <stdlib.h>

#define PARSED_CODE_SIZE 50
#define MEMORY_SIZE 100

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
      // Increase target memory size if needed
      *targetLength = 2 * (*targetLength);
      char *tmp = realloc(*target, sizeof(char) * (*targetLength));
      if (tmp == NULL)
      {
        printf("Error: failed to augment allocated memory.\n");
        free(*target);
        return EXIT_FAILURE;
      }
      *target = tmp;
    }
  } while (readChar != EOF);
  (*target)[i] = '\0';

  return EXIT_SUCCESS;
}

int execute(char *code, int **memory, int *memorySize, int *ptr)
{
  int i = 0;
  while (code[i] != '\0')
  {
    switch (code[i])
    {
    case '+':
      (*memory)[*ptr]++;
      break;
    case '-':
      (*memory)[*ptr]--;
      break;
    case '>':
      (*ptr)++;
      if ((*ptr) >= *memorySize)
      {
        printf("Error: pointer above memory limit.\n");
        return EXIT_FAILURE;
      }
      break;
    case '<':
      (*ptr)--;
      if ((*ptr) < 0)
      {
        printf("Error: pointer assigned to a value below 0.\n");
        return EXIT_FAILURE;
      }
      break;
    case '.':
      putchar((*memory)[*ptr]);
      break;
    case ',':
      (*memory)[*ptr] = getchar();
      break;
    case '[':
      int loopSize = MEMORY_SIZE;
      char *loop = malloc(sizeof(char) * loopSize);
      int openedLoops = 0;
      int j = 0;
      while (openedLoops != 0 || code[i + j + 1] != ']')
      {
        if (code[i + j + 1] == '\0')
        {
          printf("Error: EOF reached before end of the loop.\n");
          return EXIT_FAILURE;
        }
        if (code[i + j + 1] == '[')
          openedLoops++;
        if (code[i + j + 1] == ']')
          openedLoops--;
        loop[j] = code[i + j + 1];
        j++;
        if (j >= loopSize)
        {
          // Increase target memory size if needed
          loopSize = 2 * (loopSize);
          char *tmp = realloc(loop, sizeof(char) * (loopSize));
          if (tmp == NULL)
          {
            printf("Error: failed to augment allocated memory for a loop.\n");
            free(loop);
            return EXIT_FAILURE;
          }
          loop = tmp;
        }
      }
      loop[j] = '\0';
      while ((*memory)[*ptr] != 0)
      {
        if (execute(loop, memory, memorySize, ptr))
          return EXIT_FAILURE;
      }
      i = i + j + 1;
      free(loop);
      break;
    case ']':
      break;
    default:
      printf("Error: unknown char in parsed code.\n");
      return EXIT_FAILURE;
    }
    i++;
  }
  return EXIT_SUCCESS;
}

int main(void)
{
  char *filename = "./tests/hello_world.bf";
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
  {
    printf("Error: specified file could not be open.\n");
    return EXIT_FAILURE;
  }

  int parsedCodeLength = PARSED_CODE_SIZE;
  char *parsedCode = malloc(sizeof(char) * parsedCodeLength);
  if (parsedCode == NULL)
  {
    printf("Error: failed to allocate memory for the parsed code.\n");
    return EXIT_FAILURE;
  }

  int failedParsing = parse(&parsedCode, &parsedCodeLength, fp);
  fclose(fp);
  if (failedParsing)
  {
    printf("Error: failed to parse code.\n");
    return EXIT_FAILURE;
  }

  printf("; Parsed Code Length : %d\n", parsedCodeLength);
  printf("; Parsed code : '%s'\n\n", parsedCode);

  int memorySize = MEMORY_SIZE;
  int *memory = calloc(memorySize, sizeof(int));
  if (memory == NULL)
  {
    printf("Error: failed to allocate memory for the program memory.\n");
    return EXIT_FAILURE;
  }

  int ptr = 0;
  int failedExecution = execute(parsedCode, &memory, &memorySize, &ptr);
  if (!failedExecution)
  {
    printf("\n; Successfuly executed brainfuck code.\n\n");
  }

  free(memory);
  free(parsedCode);
  return EXIT_SUCCESS;
}