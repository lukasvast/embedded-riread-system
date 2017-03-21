/**
@file StringProcessing.h
@brief Header knji�nice funkcija za procesiranje stringova zapisanih na karticama.
*/
#ifndef STRING_PROCESSING_H
#define STRING_PROCESSING_H

char* VratiOdNtePojaveDelimiteraDoDelimitera(char* string, char* needle, int pos, char* delimiter);
char* VratiOdPozicijeDoPozicije(char* string, int leftPosition, int rightPosition);
void FormatirajDijakritickeNaZdravstvenoj(char* haystack);

#endif //STRING_PROCESSING_H
