#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIGNS lineBuffer[i] == '+' || lineBuffer[i] == '-' || lineBuffer[i] == '*' || lineBuffer[i] == '/'
#define SIGNS_T lineBuffer[j] == '+' || lineBuffer[j] == '-' || lineBuffer[j] == '*' || lineBuffer[j] == '/'

int main()
{
	char *lineBuffer;
	FILE *expressions = fopen("expressions.txt", "r");
	FILE *resulting = fopen("result.txt", "w");

	fseek(expressions, 0, SEEK_END);
	int len = ftell(expressions);
	lineBuffer = malloc(len * sizeof(char));
	fseek(expressions, 0, SEEK_SET);

	while (fgets(lineBuffer, len, expressions) != 0)
	{
		int operands[500];
		float result = 0;
		int oldPos = 0;
		int pos = 0;
		int notSpaced = 0;
        int noOfOperands = 0;
        int id = 0;

		for (int i = 0; i < strlen(lineBuffer); i++)
		{
			// FOR EACH LINE
			if (SIGNS)
			{
                noOfOperands = 0;
                oldPos = i;
				for (int j = pos; j <= oldPos; j++)
				{
					// FOR EACH ENCOUNTER OF SIGNS

					if (lineBuffer[j] == (int)' ' || SIGNS_T)
					{
						notSpaced = 0;
					}

					if (lineBuffer[j] >= 48 && lineBuffer[j] <= 57)
					{
                        noOfOperands++;
						if (notSpaced == 0)
						{
                            id++;
                            notSpaced++;
							operands[id] = lineBuffer[j] - '0';
							printf("operand: %d\n", operands[id]);

						} else if(notSpaced >=1) {
                            id++;
							operands[id - 1] = operands[id - 1] * pow(10, notSpaced) + (lineBuffer[j] - '0');
							printf("operands: %d\n", operands[id - 1]);
                            id--;
                            noOfOperands--;
                        }

					}

                if(noOfOperands == 1) {
                    switch (lineBuffer[j])
                    {
                    case '+':
                        printf("Operator: %c\n", '+');
                        result +=  operands[id];
                        id = 0;
                        break;
                    case '-':
                        printf("Operator: %c\n", '-');
                        result -= operands[id];
                        id = 0;
                        break;
                    case '/':
                        printf("Operator: %c\n", '/');
                        result /= operands[id];
                        id = 0;
                        break;
                    case '*':
                        printf("Operator: %c\n", '*');
                        result *= operands[id];
                        id = 0;
                        break;
                    default:
                        break;
                        }
                    } else if(noOfOperands == 2){
                    switch (lineBuffer[j])
                    {
                    case '+':
                        printf("Operator: %c\n", '+');
                        result += operands[id-1] + operands[id];
                        id = 0;
                        break;
                    case '-':
                        printf("Operator: %c\n", '-');
                        result += operands[id-1] - operands[id];
                        id = 0;
                        break;
                    case '/':
                        result /= operands[id];
                        printf("Operator: %c\n", '/');
                        result += (float)operands[id-1] / operands[id];
                        id = 0;
                        break;
                    case '*':
                        if(id == 1) {
                            result *= operands[id];
                        } else {
                        printf("Operator: %c\n", '*');
                        result += (float)operands[id-1] * operands[id];
                        id = 0;
                        }
                        break;
                    default:
                        break;
                        }

                }

				}


				pos = oldPos;
			}
		}
		printf("Result : %f \n", result);
        fprintf(resulting, "%f\n", result);
	}
	free(lineBuffer);
	fclose(expressions);
	fclose(resulting);
	return 0;
}
