/**
 * Just another brainfuck interpreter... Not much more to say!
 * It parses the code to keep only useful chars and then execute
 * it through another function.
 *
 * author: Jules.#7341
 * date: November 7 2022
 */

#include <stdio.h>
#include <stdlib.h>

#define PARSED_CODE_SIZE 50
#define MEMORY_SIZE 100
#define BF_CHARS "><+-,.[]"

/**
 * Tells if a specific character is part a brainfuck operator.
 *
 * Args:
 *    char c - The character to check.
 *
 * Returns:
 *    int - The result of the search.
 */
int isPartOfBf(char c)
{
  char *bfChars = BF_CHARS;
  int i = 0;

  while (bfChars[i] != '\0')
  {
    if (bfChars[i] == c)
      return 1;
    ++i;
  }
  return 0;
}

/**
 * Doubles allocated memory to a char *.
 *
 * Args:
 *    char **memory - The pointer to the pointer, eheh.
 *    int *memoryLength - The pointer to the initial size of the memory.
 *
 * Returns:
 *    int - Tells if everything went well.
 */
int augmentAllocatedMemory(char **memory, int *memoryLength)
{
  *memoryLength = 2 * (*memoryLength);
  char *tmp = realloc(*memory, sizeof(char) * (*memoryLength));
  if (tmp == NULL)
  {
    printf("Error: failed to augment allocated memory.\n");
    free(*memory);
    return EXIT_FAILURE;
  }
  *memory = tmp;
  return EXIT_SUCCESS;
}

/**
 * Parse a file to extract brainfuck code from it.
 *
 * Args:
 *    char **target - A pointer to the variable where the code will be saved.
 *    int *targetLength - A pointer to the length of the '*target' variable.
 *    FILE *fp - A pointer to the file to extract memory from.
 *
 * Returns:
 *    int - Tells if everything went well.
 */
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
      if (augmentAllocatedMemory(target, targetLength))
      {
        return EXIT_FAILURE;
      }
    }
  } while (readChar != EOF);
  (*target)[i] = '\0';

  return EXIT_SUCCESS;
}

/**
 * Parse the content of a loop (between '[' and ']').
 *
 * Args:
 *    char **target - A pointer to the variable where the code will be saved.
 *    int *targetLength - A pointer to the length of the '*target' variable.
 *    char **sourceCode - A pointer to the brainfuck code.
 *    int *posI - A pointer to the advancement of the execution of the code.
 *
 * Returns:
 *    int - Tells if everything went well.
 */
int parseLoop(char **target, int *targetLength, char **sourceCode, int *posI)
{
  int openedLoops = 0;
  int j = 0;

  while (openedLoops != 0 || (*sourceCode)[*posI + j + 1] != ']')
  {
    if ((*sourceCode)[*posI + j + 1] == '\0')
    {
      printf("Error: EOF reached before end of the loop.\n");
      return EXIT_FAILURE;
    }
    if ((*sourceCode)[*posI + j + 1] == '[')
      openedLoops++;
    if ((*sourceCode)[*posI + j + 1] == ']')
      openedLoops--;
    (*target)[j] = (*sourceCode)[*posI + j + 1];
    j++;
    if (j >= *targetLength)
    {
      augmentAllocatedMemory(target, targetLength);
    }
  }
  (*target)[j] = '\0';
  *posI = *posI + j + 1;

  return EXIT_SUCCESS;
}

/**
 * Executes a parsed code of brainfuck.
 *
 * Args:
 *    char *code - The code to execute.
 *    int *memory - The memory of the program.
 *    int memorySize - The memory size.
 *    int *ptr - A pointer to the position of the brainfuck 'fake' pointer.
 *
 * Returns:
 *    int - Tells if everything went well.
 */
int execute(char *code, int *memory, int memorySize, int *ptr)
{
  int i = 0;
  while (code[i] != '\0')
  {
    switch (code[i])
    {
    case '+':
      memory[*ptr]++;
      break;
    case '-':
      memory[*ptr]--;
      break;
    case '>':
      (*ptr)++;
      if ((*ptr) >= memorySize)
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
      putchar(memory[*ptr]);
      break;
    case ',':
      memory[*ptr] = getchar();
      break;
    case '[':
      // Parse the loop
      int loopSize = PARSED_CODE_SIZE;
      char *loop = malloc(sizeof(char) * loopSize);

      if (parseLoop(&loop, &loopSize, &code, &i))
        return EXIT_FAILURE;

      // Execute it while the cell is not empty
      while (memory[*ptr] != 0)
      {
        if (execute(loop, memory, memorySize, ptr))
          return EXIT_FAILURE;
      }
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

  /* Parse the code */
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

  // printf("; Parsed Code Length : %d\n", parsedCodeLength);
  // printf("; Parsed code : '%s'\n\n", parsedCode);

  /* Execute the code */
  int memorySize = MEMORY_SIZE;
  int *memory = calloc(memorySize, sizeof(int));
  if (memory == NULL)
  {
    printf("Error: failed to allocate memory for the program memory.\n");
    return EXIT_FAILURE;
  }

  int ptr = 0;
  int failedExecution = execute(parsedCode, memory, memorySize, &ptr);
  if (failedExecution)
  {
    printf("\nError: something went wrong while executing the code.\n\n");
    return EXIT_FAILURE;
  }
  printf("\n; Successfuly executed brainfuck code.\n\n");

  /* Clean up */
  free(memory);
  free(parsedCode);
  return EXIT_SUCCESS;
}