/**
@file RiRead.c
@brief Glavni programski kod
*/

//avrdude -c usbasp -p m32 -U flash:w:LCDUSARTproject.hex

/**
@brief Definiranje frekvencije oscilatora.
*/
#define F_CPU	8000000UL

#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <inttypes.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "RiRead.h"
#include "SSD1289x16.h"
#include "Font.h"
#include "ft18x34.h"
#include "Graphic.h"
#include "Usart.h"
#include "StringProcessing.h"

/**
@brief Init funkcija - Inicijalizacija portova i pinova, omoguæavanje prekida, odabir fonta, resetiranje LCD-a, postavljanje UBRR vrijednosti za USART.
*/
void Init() {

	
	GICR |= 1<<INT0;  ///Omoguæavanje prekida INT0 
	MCUCR |= _BV(1) | _BV(3); //Prekid se okida na rastuæi brid
	sei(); //Omoguæavanje globlanih prekida

	/// JTAG interface onemoguæen; kako bi se izbjegao statièki eliktricitet na TDO pin
	MCUCSR |= _BV(JTD);
	MCUCSR |= _BV(JTD);

	///Inicijalizacija LCD kontrolnih pinova i MCU I/O pinova
	Orb(LCD_RST_DPRT,LCD_RST_PIN);
	Orb(LCD_RS_DPRT,LCD_RS_PIN);
	Orb(LCD_CS_DPRT,LCD_CS_PIN);
	Orb(LCD_RD_DPRT,LCD_RD_PIN);
	Orb(LCD_WR_DPRT,LCD_WR_PIN);
	
	LCD_Reset();
	LcdFont(ft18x34); //Uèitavanje fonta
	LCD_Clear(BLACK);
    
	UsartInit(51); ///fosc = 8.000MHz, Baud rate 9600bps --> UBRR = 51

}

/**
@brief Funkcija kojom pojednostavljujemo ispis teksta na LCD.
@param left - lijeva koordinata
@param top - gornja koordinata
@param right - desna koordinata
@param bottom - donja koordinata
@param *Text - tekst za ispisati
@param align - željeno poravnanje teksta
@param color - željena boja teksta
@param scale - mijenjanje velièine, tj. skaliranje fonta
*/
void PrintLCD(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom, char *Text, uint8_t align, uint16_t color, uint8_t scale) {
	
	SetFgColor(color);
	FontXScale = scale;
	FontYScale = scale;
	DrawText(left, top, right, bottom, Text, align);
	
}

/**
@brief Funkcija koja vraæa tip kartice.
@details Vraæa se integer (u RiRead.h u enum kartice su definirane vrijednosti). Popis kartica i duljina svake kartice se nalazi u polju popisKartica takoðer definirane u headeru. Vraæa se vrijednost 0 ukoliko kartica ne postoji.
@param identifikator - broj znakova koje proèitana kartica ima
@return popisKartica - broj kartice
*/
int ProvjeriTipKartice(int identifikator) {
	
	int i;
	
	for(i = 0; i < sizeOfPopisKartica; i++) {
		
		if(i%2 != 0) {
			if(popisKartica[i] == identifikator)
			return popisKartica[i-1];
		}
		
	}
	
	return 0;
	
}

/**
@brief Funkcija koja parsa string proèitan s kartice.
@details Parsani se podaci spremaju u globalnu strukturu.
@param string - string proèitan s kartice
@param tipKartice - broj kartice pomoæu kojeg se odreðuje naèin parsanja
*/
void DohvatiPodatke(char* string, int tipKartice) {
	
	switch(tipKartice) {
		
		case XICA:
		
		korisnik.ime = VratiOdNtePojaveDelimiteraDoDelimitera(string, "/", 1, " ");
		korisnik.prezime = VratiOdNtePojaveDelimiteraDoDelimitera(string, "^", 1, "/");
		korisnik.jmbag = VratiOdPozicijeDoPozicije(string, 11, 20);
		korisnik.jmbg = VratiOdNtePojaveDelimiteraDoDelimitera(string, "^", 2, "?");
		
		break;
		
		case HAK:
		
		korisnik.ime = VratiOdNtePojaveDelimiteraDoDelimitera(string, "/", 1, " ");
		korisnik.prezime = VratiOdNtePojaveDelimiteraDoDelimitera(string, "^", 1, "/");
		korisnik.brojAutokluba = VratiOdPozicijeDoPozicije(string, 3, 10);
		korisnik.clanskiBroj = VratiOdPozicijeDoPozicije(string, 11, 19);
		korisnik.datumIsteka = VratiOdPozicijeDoPozicije(string, 56, 63);
		
		//korisnik.datumIsteka = FormatirajDatumIstekaHak(korisnik.datumIsteka);
		
		break;
		
		case OBVEZNO:
		
		korisnik.ime = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 2, ")");
		korisnik.prezime = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 3, ")");
		//korisnik.postanskiBroj = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 4, ")");
		korisnik.ulica = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 5, " ");
		korisnik.kucniBroj = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 6, ")");
		//korisnik.mbo = VratiOdNtePojaveDelimiteraDoDelimitera(string, ";", 1, "=");
		korisnik.oib = VratiOdNtePojaveDelimiteraDoDelimitera(string, "=", 1, "=");
		korisnik.datumRodenja = VratiOdNtePojaveDelimiteraDoDelimitera(string, "%", 1, ")");
		
		FormatirajDijakritickeNaZdravstvenoj(korisnik.ime);
		FormatirajDijakritickeNaZdravstvenoj(korisnik.prezime);
		FormatirajDijakritickeNaZdravstvenoj(korisnik.ulica);
		
		//korisnik.datumRodenja = FormatirajDatumRodenjaObvezno();
		
		break;
		
		case DOPUNSKO:
		
		korisnik.ime = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 1, ")");
		korisnik.prezime = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 2, ")");
		//korisnik.postanskiBroj = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 3, ")");
		//korisnik.ulica = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 4, " ");
		//korisnik.kucniBroj = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 5, ")");
		korisnik.sifraPolice = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 6, ")");
		korisnik.vrijediOd = VratiOdNtePojaveDelimiteraDoDelimitera(string, ")", 7, ")");
		//korisnik.podrucniUred = VratiOdNtePojaveDelimiteraDoDelimitera(string, ";", 1, "=");
		korisnik.brojIskaznice = VratiOdNtePojaveDelimiteraDoDelimitera(string, "=", 1, "=");
		//korisnik.mbo = VratiOdNtePojaveDelimiteraDoDelimitera(string, "%", 1, ")");
		korisnik.oib = VratiOdNtePojaveDelimiteraDoDelimitera(string, "=", 2, "=");
		
		FormatirajDijakritickeNaZdravstvenoj(korisnik.ime);
		FormatirajDijakritickeNaZdravstvenoj(korisnik.prezime);
		FormatirajDijakritickeNaZdravstvenoj(korisnik.ulica);
		
		//korisnik.vrijediOd = FormatirajDatumVrijediOdDopunsko(korisnik.vrijediOd);
		
		break;
		
	}
	
}

/**
@brief Funkcija koja postavlja sve stringove na nulu.
*/
void DeklarirajNaNulu() {
	
	korisnik.ime = NULL;
	korisnik.prezime = NULL;
	korisnik.brojAutokluba = NULL;
	korisnik.brojIskaznice = NULL;
	korisnik.clanskiBroj = NULL;
	korisnik.datumIsteka = NULL;
	korisnik.jmbag = NULL;
	korisnik.jmbg = NULL;
	korisnik.kucniBroj = NULL;
	korisnik.mbo = NULL;
	korisnik.oib = NULL;
	korisnik.podrucniUred = NULL;
	korisnik.postanskiBroj = NULL;
	korisnik.sifraPolice = NULL;
	korisnik.ulica = NULL;
	korisnik.vrijediOd = NULL;
	korisnik.datumRodenja = NULL;
	korisnik.tipKartice = 0;
	
}

/**
@brief Funkcija koja prikazuje zaslon dobrodošlice, iscrtava logo, ispisuje naziv aplikacije i autora.
*/
void ShowWelcomeScreen() {
	//Nacrtaj logo
	RoundRectangle(45,50,190,130,5,0);
	RoundRectangle(46,51,191,131,5,0);
	RoundRectangle(47,52,192,132,5,0);
	RoundRectangle(48,53,193,133,5,0);
	Line(42,62,195,62);
	Line(42,63,195,63);
	Line(42,83,195,83);
	Line(42,84,195,84);
	//Ispiši tekst
	PrintLCD(8,305,235,20 ,"RiRead", ALINE_CENTER, LIGHTBLUE, 3);
	PrintLCD(5,300,235,300 ,"by BELF", ALINE_CENTER, WHITE, 1);

}

/**
@brief Funkcija koja prikazuje zaslon za prijavu.
@return counter - broj znakova proèitane kartice
*/
int LoginXicom() {
	
	int i=0;
	
	int kontrola=0;
	int counter=0;

	RoundRectangle(5,5,235,315,2,0);
	RoundRectangle(6,6,234,314,2,0);
	RoundRectangle(8,8,232,312,2,0);
	
	PrintLCD(10,10,234,20 ,"RiRead Login", ALINE_CENTER, LIGHTBLUE, 2);
	Line(8,65,232,65);
	PrintLCD(15,175,232,20 ,"Please,", ALINE_CENTER, WHITE, 2);
	PrintLCD(15,250,232,20 ,"swipe your", ALINE_CENTER, WHITE, 2);
	PrintLCD(15,335,232,20 ,"X-ica", ALINE_CENTER, WHITE, 2);
	Line(8,260,232,260);
	
	while(kontrola!=1)
	{
		str[i]=USARTReadChar();
		
		counter++;
		
		if(str[i] == 13) // 0x0D (13 decimalno) --> kod za 'CR' (carriage return), predzadnji character svakog zapisa na kartici
		{
			kontrola++;
			str[i+1] = USARTReadChar(); //zadnji character svakog zapisa na kartici je 'LF' (line feed), riješeno s predzadnji+1
			break;
		}
		i++;
	}
	
	return counter;
	
}

/**
@brief Funkcija koja provjerava je li provuèena X-ica te shodno tome ispisuje status (ERROR || SUCCESS) na Login screenu
@param brojZnakova - broj znakova proèitane kartice
@return 1 SUCCESS
@return -1 ERROR
*/
int ShowLoginScreen(int brojZnakova) {
	
	korisnik.tipKartice = ProvjeriTipKartice(brojZnakova);
	
	if(korisnik.tipKartice == XICA) {
		
		PrintLCD(10,273,235,260 ,"SUCCESS", ALINE_CENTER, GREEN, 2);
		_delay_ms(2000);
		
		return 1;
		
		} else {
		
		PrintLCD(10,273,235,260 ,"ERROR", ALINE_CENTER, RED, 2);
		_delay_ms(2000);
		
		return -1;
		
	}
	
}

/**
@brief Funkcija koja prikazuje 'Card screen' zaslon nakon uspješne prijave X-icom, ispisuje pozdravnu poruku prijavljenom korisniku, njegovu privilegiju te menu podržanih kartica.
*/
void ShowCardScreen(){
	//Nacrtaj obrub
	RoundRectangle(5,5,235,315,2,0);
	RoundRectangle(6,6,234,314,2,0);
	RoundRectangle(8,8,232,312,2,0);
	
	char text[20];

	strcpy(text, "Welcome, ");
	strcat(text, korisnik.ime);
	strcat(text, ".");
	
	PrintLCD(25,25,235,22 ,text, ALINE_LEFT, WHITE, 1);

	if(strcmp(korisnik.jmbag, admin) == 0) {
		strcpy(text, "ADMIN");
		privilegije = ADMIN;
		} else if(strcmp(korisnik.jmbag, moderator) == 0) {
		strcpy(text, "MODERATOR");
		privilegije = MODERATOR;
		} else {
		privilegije = GUEST;
		strcpy(text, "GUEST");
	}
	
	
	PrintLCD(25,50,235,39 ,text, ALINE_LEFT, LIGHTORANGE, 1);
	//PrintLCD(50,50,220,39, "LOGOUT", ALINE_RIGHT, LIGHTBLUE,1);
	Line(8,65,232,65);
	//Menu podržanih kartica
	PrintLCD(30,80,235,105 ,"PODRZANE KARTICE", ALINE_LEFT, WHITE, 1);
	Line(28,105,200,105);
	PrintLCD(20,120,235,145 ," > X-ica", ALINE_LEFT, GOLD, 1);
	PrintLCD(20,150,235,175 ," > HAK", ALINE_LEFT, YELLOW, 1);
	PrintLCD(20,180,235,205 ," > HZZO Obvezno", ALINE_LEFT, SILVER, 1);
	PrintLCD(20,210,235,235 ," > HZZO Dopunsko", ALINE_LEFT, ORANGE, 1);
	
	PrintLCD(8,260,235,250 ,"SWIPE IT!", ALINE_CENTER, WHITE, 2);
	
	InfoSwipe();

}

/**
@brief Funkcija koja èita jednu od podržanih kartica na ShowCard screenu.
@return 0 - SUCCESS
*/
int InfoSwipe(){
	
	DeklarirajNaNulu();
	int kontrola = 0, i = 0;
	int counter = 0;
	
	while(kontrola!=1)
	{
		str[i]=USARTReadChar();
		
		counter++;
		
		if(str[i] == 13) // 0x0D (13 decimalno) --> kod za 'CR' (carriage return), predzadnji character svakog zapisa na kartici 
		{
			kontrola++;
			str[i+1] = USARTReadChar(); //zadnji character svakog zapisa na kartici je 'LF' (line feed), riješeno s predzadnji+1
			break;
		}
		i++;
	}
	
	brojKartice = ProvjeriTipKartice(counter);
	
	DohvatiPodatke(str, brojKartice);

	brojChara=counter;
	return 0;
}

/**
@brief Funkcija koja prikazuje formatirani ispis sadržaja ovisno o privilegijama i kartici.
*/
void ShowInfoScreen(){
	
	char txt[20];
	
	switch(brojKartice) {
		case XICA:
		Line(1,25,239,25);
		Line(1,26,239,26);
		PrintLCD(5,0,235,22, "X-ica", ALINE_CENTER, GOLD, 1);
		//PrintLCD(15,0,230,22, "BACK", ALINE_RIGHT, LIGHTBLUE,1);
		
		//PrintLCD(8,280,235,305 , "BACK", ALINE_CENTER, LIGHTBLUE,1);
		strcpy(txt,korisnik.ime);
		strcat(txt," ");
		PrintLCD(10,20,235,65 ,"Ime i prezime", ALINE_CENTER,WHITE,1);
		Rectangle(5,60,235,95);
		Rectangle(6,61,234,96);
		PrintLCD(10,62,235,90, strcat(txt,korisnik.prezime), ALINE_CENTER, YELLOW, 1);
		
		if(privilegije==ADMIN || privilegije==MODERATOR ){
			PrintLCD(10,90,235,135 ,"JMBAG", ALINE_CENTER,WHITE,1);
			Rectangle(5,130,235,165);
			Rectangle(6,131,234,166);
			PrintLCD(10,132,235,160, korisnik.jmbag, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			PrintLCD(10,160,235,205 ,"JMBG", ALINE_CENTER,WHITE,1);
			Rectangle(5,200,235,235);
			Rectangle(6,201,234,236);
			PrintLCD(10,202,235,230, korisnik.jmbg, ALINE_CENTER, YELLOW, 1);
		}
		break;
		
		case HAK:
		Line(1,25,239,25);
		Line(1,26,239,26);
		PrintLCD(5,0,235,22, "HAK card", ALINE_CENTER, YELLOW, 1);
		//PrintLCD(15,0,230,22, "BACK", ALINE_RIGHT, LIGHTBLUE,1);
		strcpy(txt,korisnik.ime);
		strcat(txt," ");
		PrintLCD(10,20,235,65 ,"Ime i prezime", ALINE_CENTER,WHITE,1);
		Rectangle(5,60,235,95);
		PrintLCD(10,62,235,90, strcat(txt,korisnik.prezime), ALINE_CENTER, YELLOW, 1);
		
		if(privilegije==ADMIN || privilegije==MODERATOR ){
			PrintLCD(10,90,235,135 ,"Broj autokluba", ALINE_CENTER,WHITE,1);
			Rectangle(5,130,235,165);
			PrintLCD(10,132,235,160, korisnik.brojAutokluba, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			PrintLCD(10,160,235,205 ,"Clanski broj", ALINE_CENTER,WHITE,1);
			Rectangle(5,200,235,235);
			PrintLCD(10,202,235,230, korisnik.clanskiBroj, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			PrintLCD(10,230,235,275 ,"Datum isteka", ALINE_CENTER,WHITE,1);
			Rectangle(5,270,235,305);
			PrintLCD(10,272,235,300, korisnik.datumIsteka, ALINE_CENTER, YELLOW, 1);
		}
		break;
		
		case OBVEZNO:
		Line(1,25,239,25);
		Line(1,26,239,26);
		PrintLCD(5,0,235,22, "HZZO Obvezno", ALINE_CENTER, SILVER, 1);
		//PrintLCD(15,0,230,22, "BACK", ALINE_RIGHT, LIGHTBLUE,1);
		strcpy(txt,korisnik.ime);
		strcat(txt," ");
		PrintLCD(10,20,235,65 ,"Ime i prezime", ALINE_CENTER,WHITE,1);
		Rectangle(5,60,235,95);
		PrintLCD(10,62,235,90, strcat(txt,korisnik.prezime), ALINE_CENTER, YELLOW, 1);
		
		if(privilegije==ADMIN || privilegije==MODERATOR ){
			strcpy(txt,korisnik.ulica);
			strcat(txt," ");
			PrintLCD(10,90,235,135 ,"Adresa", ALINE_CENTER,WHITE,1);
			Rectangle(5,130,235,165);
			PrintLCD(10,132,235,160, strcat(txt,korisnik.kucniBroj), ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			PrintLCD(10,160,235,205 ,"OIB", ALINE_CENTER,WHITE,1);
			Rectangle(5,200,235,235);
			PrintLCD(10,202,235,230, korisnik.oib, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			PrintLCD(10,230,235,275 ,"Datum rodenja", ALINE_CENTER,WHITE,1);
			Rectangle(5,270,235,305);
			PrintLCD(10,272,235,300, korisnik.datumRodenja, ALINE_CENTER, YELLOW, 1);
		}
		break;
		
		case DOPUNSKO:
		Line(1,25,239,25);
		Line(1,26,239,26);
		PrintLCD(5,0,235,22, "HZZO Dopunsko", ALINE_CENTER, ORANGE, 1);
		//PrintLCD(15,0,230,22, "BACK", ALINE_RIGHT, LIGHTBLUE,1);
		strcpy(txt,korisnik.ime);
		strcat(txt," ");
		PrintLCD(10,20,235,65 ,"Ime i prezime", ALINE_CENTER,WHITE,1);
		Rectangle(5,60,235,95);
		PrintLCD(10,62,235,90, strcat(txt,korisnik.prezime), ALINE_CENTER, YELLOW, 1);
		
		if(privilegije==ADMIN || privilegije==MODERATOR ){
			
			PrintLCD(10,90,235,135 ,"OIB", ALINE_CENTER,WHITE,1);
			Rectangle(5,130,235,165);
			PrintLCD(10,132,235,160, korisnik.oib, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			
			PrintLCD(5,160,115,205 ,"Sifra police", ALINE_CENTER,WHITE,1);
			Rectangle(5,200,115,235);
			PrintLCD(10,202,115,230, korisnik.sifraPolice, ALINE_CENTER, YELLOW, 1);
			
			PrintLCD(120,160,235,205 ,"Br. iskaznice", ALINE_CENTER,WHITE,1);
			Rectangle(120,200,235,235);
			PrintLCD(122,202,235,230, korisnik.brojIskaznice, ALINE_CENTER, YELLOW, 1);
		}
		
		if(privilegije==ADMIN){
			
			PrintLCD(5,230,235,275 ,"Vrijedi od", ALINE_CENTER,WHITE,1);
			Rectangle(5,270,235,305);
			PrintLCD(10,272,235,300, korisnik.vrijediOd, ALINE_CENTER, YELLOW, 1);
		}
		break;
		
		default:
		PrintLCD(5,20,235,22, "Nepodrzana kartica!", ALINE_CENTER, RED, 1);
		//PrintLCD(8,280,235,305 , "BACK", ALINE_CENTER, LIGHTBLUE,1);
		break;
	}
	
}

/**
@brief Funkcija koja oèisti ekran te poziva razlièite screenove.
@param screen - screen koji želimo prikazati
@param status - opcionalna vrijednost za uspješnost
@return error - uspješnost (-1 za neuspjeh, 0 za uspjeh)
*/
int ClearAndShowNewScreen(int screen, int status) {
	
	LCD_Clear(BLACK);
	
	int error, brojZnakova;

	switch(screen) {

		case WELCOME:

		ShowWelcomeScreen();

		return 0;

		case LOGIN:

		brojZnakova = LoginXicom();
		
		error = ShowLoginScreen(brojZnakova);
		
		return error;

		case CARD:
		
		ShowCardScreen();
		return 0;
		
		case INFO:
		
		ShowInfoScreen();
		return 0;

		default:
		return -1;
	}

}

/**
@brief Funkcija za Hard Reset pomoæu Watchdog timera.
*/
void forceHardReset()
{
	cli(); // disable interrupts
	wdt_enable(WDTO_15MS); // enable watchdog
	while(1); // wait for watchdog to reset processor
}

/**
@brief Main funkcija - izvoðenje programa.
*/
int main() {
	
	Init();
	int status;
	RESETIRAN=read_eeprom_word(&my_eeprom_word);
	
	if(RESETIRAN==0)
	{
		ClearAndShowNewScreen(WELCOME, 0);
		_delay_ms(2000);
		
		DeklarirajNaNulu(korisnik);
		
		do {
			status = ClearAndShowNewScreen(LOGIN, 0);
		} while (status != 1);

		DohvatiPodatke(str, XICA);
		
		ClearAndShowNewScreen(CARD, 0);
		
		ClearAndShowNewScreen(INFO, 0);
		
		_delay_ms(5000);
		
		RESETIRAN=1;
		write_eeprom_word(&my_eeprom_word,RESETIRAN);
		forceHardReset();
		}else{
		DeklarirajNaNulu(korisnik);
		
		while(1) {
			do {
				status = ClearAndShowNewScreen(LOGIN, 0);
			} while (status != 1);

			DohvatiPodatke(str, XICA);
			
			ClearAndShowNewScreen(CARD, 0);
			
			ClearAndShowNewScreen(INFO, 0);
			
			_delay_ms(5000);
			
			RESETIRAN=1;
			write_eeprom_word(&my_eeprom_word,RESETIRAN);
			forceHardReset();
		}
	}

}

/**
@brief Interrupt service routine - prekid INT0 za reset i èišæenje EEPROM-a.
*/
ISR(INT0_vect) {
	RESETIRAN=0;
	write_eeprom_word(&my_eeprom_word,RESETIRAN);
	forceHardReset();
}
