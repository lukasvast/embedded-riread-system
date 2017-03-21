/**
@file RiRead.h
@brief Header datoteka glavnog prog. koda - sadr�i prototipe funkcija, globalne varijable, konstante, strukturu podataka
*/

#ifndef LCD_USART_PROJECT_H
#define LCD_USART_PROJECT_H

///Makroi za lak�u manipulaciju EEPROM-om
#define read_eeprom_word(address) eeprom_read_word ((const uint16_t*)address)
#define write_eeprom_word(address,value) eeprom_write_word ((uint16_t*)address,(uint16_t)value)

///Deklariranje EEPROM varijable
unsigned int EEMEM   my_eeprom_word;

/**
@struct Korisnik 
@brief Globalna struktura Korisnik s pripadaju�im �lanovima
*/
struct Korisnik {
	char *ime, *prezime, *jmbg, *jmbag, *clanskiBroj, *brojAutokluba, *datumIsteka, *postanskiBroj,
	*ulica, *kucniBroj, *mbo, *oib, *sifraPolice, *vrijediOd, *podrucniUred, *brojIskaznice, *datumRodenja;
	int tipKartice;
};

/// Klju�ne rije�i za prikazivanje pojedinog screena
enum screens {
	WELCOME = 1,
	LOGIN = 2,
	CARD = 3,
	INFO = 4
};

/// Kljucne rije�i za prepoznavanje podr�anih kartica
enum kartice {
	XICA = 1,
	HAK = 2,
	OBVEZNO = 3,
	DOPUNSKO = 4
};

/// Popis podr�anih kartica i njihov ukupan broj znakova koje sadr�e
const int popisKartica[] = {
	XICA, 85,
	HAK, 101,
	OBVEZNO, 139,
	DOPUNSKO, 133
};

/// Klju�ne rije�i za formatiranje zapisa ovisno o privilegiji
enum prava {
	ADMIN = 1,
	MODERATOR = 2,
	GUEST = 3
};

/// Konstanta kojom privilegiju 'ADMIN' dodjeljujemo odre�enom JMBAG-u
const char admin[] = "0069049950";
/// Konstanta kojom privilegiju 'MODERATOR' dodjeljujemo odre�enom JMBAG-u
const char moderator[] = "0069059387";

// Globalne varijable
///buffer
char str[200];
///struktura
struct Korisnik korisnik; 
///ADMIN, MODERATOR, GUEST
int privilegije;
///sprema vrijednost ukupnog broja znakova kartice
int brojKartice;
///broj charactera kartice - na�in po kojem prepoznajemo vrstu kartice (X-ica, HAK, HZZO Obv., HZZO Dop.)
int brojChara; 
///kontrolna varijabla koju koristimo u ISR i main funkciji - odre�uje ho�e li se prikazati 'Welcome screen' 
int RESETIRAN=0;  

/// Konstanta u koju se sprema veli�ina polja 'popisKartica'
const int sizeOfPopisKartica = sizeof(popisKartica)/sizeof(popisKartica[0]);

// Prototipovi funkcija
void DeklarirajNaNulu();
void DohvatiPodatke(char* string, int tipKartice);
void PrintLCD(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, char *Text, uint8_t align, uint16_t color, uint8_t scale);
void Init();
void ShowWelcomeScreen();
void ShowCardScreen();
void ShowInfoScreen();
int InfoSwipe();
int ProvjeriTipKartice(int identifikator);
int LoginXicom();
int ShowLoginScreen(int brojZnakova);
int ClearAndShowNewScreen(int screen, int status);

#endif //LCD_USART_PROJECT_H
