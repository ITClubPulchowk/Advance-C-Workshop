#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_SIZE 300
#define SYMBOL_SIZE 4

char symbols[SYMBOL_SIZE] = {'+', '-', '/', '*'};

float doMafs(float a, float b, char c) {
  switch (c) {
  case '+':
    return a + b;
  case '-':
    return a - b;
  case '/':
    return a / b;
  case '*':
    return a * b;
  default:
    return 0;
  }
}

int checkIfSymbol(char a) {
  for (int i = 0; i < SYMBOL_SIZE; i++) {
    if (a == symbols[i]) {
      return 1;
    }
  }
  return 0;
}

// IDK if this works
float recursiveSolver(char **tokens, int tokenCount, int *depth) {
  int offset = 1;
  int functionDepth = 0;
  char symbol = *tokens[tokenCount - offset];
  offset++;
  float val1, val2;
  int isSymbol;

  char *arg1 = tokens[tokenCount - offset];
  if (!checkIfSymbol(*arg1)) {
    val1 = atof(arg1);
    offset++;
  } else {
    val1 = recursiveSolver(tokens, tokenCount - offset + 1, &functionDepth);
  }
  char *arg2 = tokens[tokenCount - offset - functionDepth];
  if (!checkIfSymbol(*arg2)) {
    val2 = atof(arg2);
  } else {
    offset--;
    val2 =
        recursiveSolver(tokens, tokenCount - offset - functionDepth, &offset);
  }
  *depth = offset + functionDepth;

  return doMafs(val2, val1, symbol);
}

void pushToken(char *token, char **tokens, int *tokenCount) {
  if (strlen(token) != 0) {
    tokens[*tokenCount] = token;
    (*tokenCount) = *tokenCount + 1;
  }
}

void subString(char *src, char *dest, int startIndex, int length) {
  src = src + startIndex;
  int i;
  for (i = 0; i < length; i++) {
    dest[i] = src[i];
  }
  dest[i] = '\0';
}

int createTokens(char expression[], char **tokens) {
  char *token;
  int tokenCount = 0;

  // Tokens
  int startIndex = -1;
  for (int i = 0; i < strlen(expression); i++) {
    if (startIndex == -1) {
      startIndex = i;
    }
    if (expression[i] == '\0') {
      break;
    }

    if (expression[i] == ' ' && i != strlen(expression) - 1) {
      char *token = (char *)malloc((i - startIndex + 1) * sizeof(char));
      subString(expression, token, startIndex, (i - startIndex));
      pushToken(token, tokens, &tokenCount);
      startIndex = -1;
    }
    // Basically a copy paste of the top and bottom one and i am lazy
    if (checkIfSymbol(expression[i]) && expression[i - 1] != ' ') {
      char *token = (char *)malloc((i - startIndex + 1) * sizeof(char));
      subString(expression, token, startIndex, (i - startIndex));
      pushToken(token, tokens, &tokenCount);
      startIndex = -1;
      token = (char *)malloc((2) * sizeof(char));
      token[0] = expression[i];
      token[1] = '\0';
      pushToken(token, tokens, &tokenCount);
      continue;
    }

    if (i == strlen(expression) - 1) {
      if (expression[i] == ' ')
        continue;
      char *token = (char *)malloc((2) * sizeof(char));

      token[0] = expression[i];
      token[1] = '\0';
      pushToken(token, tokens, &tokenCount);
    }
  }
  return tokenCount;
}

void freeTokens(char **tokens, int tokenCount) {
  for (int i = 0; i < tokenCount; i++) {
    free(tokens[i]);
  }
  free(tokens);
}

float solve(char *expression) {
  char **tokens = (char **)malloc(INPUT_SIZE * sizeof(char *));
  int tokenCount = createTokens(expression, tokens);
  // printTokens(tokens, tokenCount);

  int depth = 0;
  // float answer = solveWithStack(tokens, tokenCount);
  float answer = recursiveSolver(tokens, tokenCount, &depth);
  freeTokens(tokens, tokenCount);
  return answer;
}

int main() {
  int choice = 1;
  if (choice == 1) {
    FILE *input = fopen("expressions.txt", "rb");
    FILE *output = fopen("results.txt", "w");
    char expression[INPUT_SIZE];
    float value = 0;

    while (fscanf(input, "%[^\n]", expression) == 1) {
      fgetc(input);

      value = solve(expression);
      fprintf(output, "%f\n", value);
    }
    fclose(input);
    fclose(output);
  } else {
    char expression[200];
    scanf("%[^\n]", expression);

    float output = solve(expression);
    printf("%f\n", output);
  }
}
