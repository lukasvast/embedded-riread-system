/**
@file StringProcessing.c
@brief Knjižnica funkcija za procesiranje stringova zapisanih na karticama.  
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
@brief Funkcija koja vraæa string u rasponu izmeðu pojave navedenih delimitera.
@param    string - haystack iz kojeg želimo izvuæi needle
@param leftDelim - lijevi delimiter
@param       pos - koje je po redu pojavljivanje lijevog delimitera u haystacku
@param delimiter - desni delimiter
@return haystack - vraæanje stringa u danom rasponu
*/
char* VratiOdNtePojaveDelimiteraDoDelimitera(char* string, char* leftDelim, int pos, char* rightDelim) {

	char *haystack;

	haystack = (char*) malloc(strlen(string));
	strcpy(haystack, string);
 
	while(haystack != '\0') {
		if(*haystack == *leftDelim) {
			pos--;
		}
		if(pos == 0) {
			haystack++;
			break;
		}
		haystack++;
	}

	strtok(haystack, rightDelim);

	return haystack;

}

/**
@brief Funkcija koja vraæa string u rasponu izmeðu navedenih pozicija.
@param        string - haystack iz kojeg želimo izvuæi needle
@param   lefPosition - lijeva pozicija
@param rightPosition - desna pozicija
@return    leftDelim - vraæanje stringa u danom rasponu
*/
char* VratiOdPozicijeDoPozicije(char* string, int leftPosition, int rightPosition) {
	
	char *haystack, *leftDelim;
	
	leftDelim = NULL;
	
	int counter = 0;
	
	if(leftPosition > rightPosition)
	return NULL;
	
	haystack = (char*) malloc(strlen(string));
	strcpy(haystack, string);
	
	while(haystack != '\0') {
		if(counter == (leftPosition-1)) {
			leftDelim = haystack;
			} else if(counter == (rightPosition-1)) {
				*(haystack+1) = '\0';
				break;
		}
		counter++;
		haystack++;
	}
	
	return leftDelim;
	
}

/**
@brief Funkcija koja nepoznate simbole formatira u pripadajuæa slova.
@param haystack - string koji se želi formatirati
*/
void FormatirajDijakritickeNaZdravstvenoj(char* haystack) {
	
	while(*haystack != '\0') {
		if(*haystack == '@')
		*haystack = 'Z';
		if(*haystack == ']')
		*haystack = 'C';
		if(*haystack == '[')
		*haystack = 'S';
		haystack++;
	}
	
}