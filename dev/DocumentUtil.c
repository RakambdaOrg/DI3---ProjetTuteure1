#include <printf.h>
#include <stdlib.h>
#include "MyString.h"
#include "DocumentUtil.h"

char * formatDate(int day, int month, int year)
{
	char dateFormat[11];
	sprintf(dateFormat, "%02d/%02d/%04d", day, month, year);
	return duplicateString(dateFormat);
}

void intToString(const int number, char * const string, size_t stringLength, const int fillZeros)
{
    int i = 0;
    int numberCopy = number;
	unsigned int negative = number < 0 ? 1 : 0;
    unsigned int intLength = charInNumber(number);
    size_t maxCopy = (fillZeros || ((stringLength - 1) < intLength)) ? (stringLength - 1) : intLength;
	if(negative)
	{
		numberCopy *= -1;
	}
    while(i < maxCopy)
    {
        string[maxCopy - i - 1] = (char) ('0' + (numberCopy) % 10);
        numberCopy /= 10;
		i++;
    }
    if(negative)
    {
        string[0] = '-';
    }
    string[maxCopy] = '\0';
}

long power(const int base, const int power)
{
    int i;
    long result = 1;
    if((power == 0) || (base == 1))
    {
        return 1;
    }
    for(i = 0; i < power; i++)
    {
        result *= base;
    }
    return result;
}

long BaseB2Dec(int B, int N, int tab[])
{
    int i;
    long result = tab[0];
    for(i = 1; i < N; i++)
    {
        result = tab[i] + B * result;
    }
    return result;
}

long Bin2Dec(int tab[])
{
    long result = BaseB2Dec(2, 15, &tab[1]);
    if(tab[0] == 1)
    {
        result -= (1L << 15);
    }
    return result;
}

long Hex2Dec(const char * hex)
{
    size_t hexLength = stringLength(hex);
    int * values;
    long result = 0;
    int i;
    if((values = (int *) malloc((hexLength - 2) * sizeof(int))) == NULL)
    {
        printf("Erreur allocation malloc");
        exit(1);
    }
    i = 2;
    if(hexLength < 3 || hex[0] != '0' || toLowerChar(hex[1]) != 'x') /* If string doesn't starts with '0x' */
    {
        printf("Hex number isn't formatted correctly!");
        exit(1);
    }
    while(hex[i] != '\0')
    {
        values[i - 2] = getNumberValue(toUpperChar(hex[i])); /* Build values array, converting A-F to integers */
        i++;
    }
    result = BaseB2Dec(16, (int) (hexLength - 2), values);
    free(values);
    return result;
}

char * computeDocumentNumber(long id)
{
    return dec2Base(id, 36);
}

char * dec2Base(long number, const int base)
{
	char * converted;
	int i;
	char * tempChar;
	unsigned int charCount = 0;
	if((tempChar = (char *) malloc((base < 10 ? charInNumber(number) : power(2, charInNumber(number))) * sizeof(char))) == NULL)
	{
		printf("Erreur allocation malloc!");
		exit(1);
	}
	while(number != 0)
	{
		tempChar[charCount] = getCharNumber((unsigned int) number % base);
		charCount++;
		number /= base;
	}
	if((converted = (char *) malloc((charCount + 1) * sizeof(char))) == NULL)
	{
		printf("Erreur allocation malloc!");
		exit(1);
	}
	for(i = 0; i < charCount; i++)
	{
		converted[i] = tempChar[i];
	}
	converted[charCount] = '\0';
	free(tempChar);
	reverseString(converted, charCount + 1);
	return converted;
}

void Dec2Bin(short n, int tab[])
{
	int i = 0;
	int negative = 0;
	if(n < 0)
	{
		negative = 1;
		n += (1 << 15);
	}
	do
	{
		tab[i] = n % 2;
		n /= 2;
		i++;
	} while (n != 0);
	if(negative)
	{
		tab[15] = 1;
	}
}