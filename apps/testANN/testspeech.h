/*
 * Copyright 2012, 2013 Fabian Ebner
 * The program is licensed under the GPL version 3 or (at your opinion) any later version, see the file COPYING for details.
 */

#ifndef __SPEECH_H__
#define __SPEECH_H__

#include <utils.h>



//Die Anzahl der Werte in jedem Rahmen
#define N 512

#define M 100

//Die Anzahl der Mel-Filter
#define N_FILTER 26

//Die Anzahl der MFCC-Merkmale
#define N_MFCC N_FILTER

/*
 * Konstanten, die die wave-Dateien, mit denen gearbeitet wird, erfuellen muessen.
 */
#define WAVE_FORMAT 0x001 //PCM
#define WAVE_CHANNELS 1
#define WAVE_SAMPLE_RATE 12500
#define WAVE_BITS_PER_SAMPLE 16
#define WAVE_FRAME_SIZE ((WAVE_BITS_PER_SAMPLE + 7) >> 3 * WAVE_CHANNELS) //(bits/sample + 7) / 8 * channels
#define WAVE_BYTES_PER_SECOND (WAVE_SAMPLE_RATE * WAVE_FRAME_SIZE) //framesize * samplerate

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long
/*
 * Eine Struktur, die eine komplexe Zahl darstellt.
 * (double) real	Der Realteil der komplexen Zahl
 * (double) imag	Dar Imaginaerteil der komplexen Zahl
 */
typedef
	struct
	{
		double real;
		double imag;
	}
comp;

/*
 * Struktur, die die MFCC-Merkmale eines Rahmens aufnimmt.
 * (double) features	Die errechneten MFCC-Merkmale
 */
typedef
	struct
	{
		double features[N_MFCC];
	} mfcc_frame; 

/*
 * Provides a list of spoken words by their MFCC characteristics.
 * (mfcc_frame *) frames	The MFCC characteristics of the current word
 * (unsigned int) n		The number of feature frames
 * (char *) name		The name of the word
 * (struct _word *) next	The next word in the linked list
 */
typedef
	struct _word
	{
		mfcc_frame *frames;
		unsigned int n;
		char *name;
		struct _word *next;
	} word;

/*
 * Struktur, die einen Rahmen darstellt.
 * Die Werte im Rahmen sind die Intensitaeten der Darstellung der Funktion im Frequenzbereich.
 * (double) magnitudes	Die Intensitaeten der Fourier-transformierten Werte.
 */
typedef
	struct
	{
		double magnitudes[N];
	} frame;
	
/*
 * Struktur, die ein Sprachsignal aufnimmt, indem es dessen PCM-Daten abspeichert
 * (u16 *) buffer	Die PCM-Werte des Signals
 * (unsigned int) n	Die Anzahl der Werte
 */
typedef
	struct
	{
		u16 *buffer;
		unsigned int number;
	} voice_signal;

/*
 * Sucht im Audiosignal, das in buffer enthalten ist, Woerter und Laute.
 * Werden solche erkannt, so wird der Bereich in eine voice_signal Struktur kopiert und diese der Liste hinzugefuegt.
 * Der Speicher der voice_signal-Liste wird innerhalb der Funktion reserviert und muss wieder frei gegeben werden.
 * <<<INPUT>>>
 * (u16 *) buffer		The PCM values ​​of the entire audio signal
 * (unsigned int) n		The number of values
 * (voice_signal **) signals	Address of a voice_signal pointer so that the memory can be reserved and filled with the PCM values ​​of the detected sounds and words
 * <<<OUTPU>>>
 * (int)			The number of recognized words, ie the length of the result vector
 */
int split(u16 *buffer, unsigned int n, voice_signal **signals);



/*
 * Erzeugt die Rahmen aus den Rohdateien.
 * Der Speicher fuer die Rahmen wird innerhalb der Funktion reserviert und sollte wieder frei gegeben werden.
 * <<<INPUT>>>
 * (u16) buffer	Puffer mit den PCM-Rohdateien, deren Rahmen erzeugt werden sollen
 * (unsigned int) n	Die Anzahl der Werte im Puffer
 * (frame **) frames	Adresse eines frame-Zeigers, damit Speicher reserviert und mit den Rahmen gefuellt werden kann
 * <<<OUTPUT>>>
 * (unsigned int)	Die Anzahl der erzeugten Rahmen, also die Laenge des frames-Vektors
 */
unsigned int make_frames_hamming(u16 *buffer, unsigned int n, frame **frames);

/*
 * Errechnet die MFCC-Merkmale jedes uebergebenen Rahmens des Eingangs-Vektors frames mit Laenge n.
 * Der Ausgabe-Vektor mfcc_frames besitzt die selbe Laenge, der Speicher muss vorher reserviert werden.
 * <<<INPUT>>>
 * (frame *) frames		Die Rahmen, deren MFCC-Merkmale berechnet werden sollen
 * (unsigned int) n		Die Anzahl der Rahmen im Eingans-Vektor
 * (mfcc_frame *) mfcc_frames	Die errechneten MFCC-Merkmale
 */
void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames);


/*
 * Speichert ein neues Wort in das Verzeichnis words mit dem Dateinamen, die im Parameter name enthalten ist.
 * <<<INPUT>>>
 * (mfcc_frame *) frames	Der Vektor mit den MFCC-Merkmalen des Wortes
 * (unsigned int) n		Die Anzahl der Merkmals-Rahmen
 * (char *) bezeichnung		The name of the word and at the same time the file name, under which is stored
 */
void new_word(mfcc_frame *frames, unsigned int n);

/*
 * Liefert eine Liste aller im Verzeichnis words abgespeicherten Woertern.
 * Die Woerter bekommen als Bezeichnung den Dateinamen unter dem sie abgespeichert wurden.
 * Der Speicher der einzelnen Woerter in der Liste muss spaeter wieder frei gegeben werden.
 * <<<INPUT>>>
 * (word *) head	Wortlisten-Zeiger, in den der Beginn der Wortliste hineingeschrieben wird
 * <<<OUTPUT>>>
 * (int)		Die Anzahl der Woerter
 */
int get_list(word *head);


/**
 * Versucht die mit filename spezifizierte wave-Datei zu oeffnen und liefert folgenden Wert zurueck:
 * (0) Erfolg
 * (-1) Datei kann nicht geoffnet werden
 * (-2) Ungueltiger wave-Header
 * Nach einem erfolgreichen open_wave-Aufruf koennen read_pcm und write_pcm aufgerufen werden.
 * War der Aufruf erfolgreich, darf diese Funktion erst wieder aufgerufen werden, wenn in der Zwischenzeit close_wave aufgerufen wurde.
 * <<<INPUT>>>
 * (const char *) path	Der Pfad zu der zu oeffnenden wave-Datei
 * <<<OUTPUT>>>
 * (int)		0, falls erfolgreich geoffnet wurde, sonst eine negative Zahl (siehe oben)
 */
int open_wave(const char *path);

/**
 * Liest aus der vorher geoffneten wave-Datei den ersten PCM-Datenblock aus (es sollte nur einen geben).
 * Der Zeiger der uebergeben wird sollte auf keinen Speicherbereich zeigen, die Funktion reserviert selbstaendig Speicher und liefert dessen Groesse zurueck.
 * <<<INPUT>>>
 * (void **) buffer	Zeiger auf den Puffer, in den die gelesenen PCM-Daten geschrieben werden
 * <<<OUTPUT>>>
 * (unsigned int)	Die Groesse des Puffers
 */
unsigned int read_pcm(void **buffer);

/**
 * Schreibt die sich in buffer befindlichen PCM-Daten der Laenge size in eine Datei.
 * Wird fuer path NULL uebergeben so muss zuvor open_wave aufgerufen werden, da in diesem Fall in die zuvor geoffnete Datei hinein geschrieben wird (schon vorhandene Daten werden geloescht!).
 * Wird path gesetzt, so wird diese Datei erstellt und der PCM-Block wird in die neu erzeugte Datei hinen geschrieben.
 * Wird nach write_pcm read_pcm aufgerufen so stehen die neuen Werte noch nicht zur Verfuegung, nur wenn die Datei geschlossen und neu geoeffnet wird.
 * Konnte die Datei nicht angelegt werden wird -1 zurueck gegeben, sonst 0
 * <<<INPUT>>>
 * (const void *) buffer	The buffer containing the data
 * (const unsigned int) size	The size of the buffer
 * (const char *) path		NULL, if you want to write to existing file, otherwise the path for the new file
 * <<<OUTPUT>>>
 * (int)			In case of error -1, otherwise 0
 */
int write_pcm(const void *buffer, const unsigned int size, const char *path);

/**
 * Schliesst die zuvor mit open_wave geoffnete Datei wieder.
 * Diese Funktion muss aufgerufen werden bevor ein weiterer open_wave-Aufruf erfolgt.
 */
void close_wave(void);


/*
 * Berechnet die diskrete Fourier-Transformation des Parameters time, der einen komplexen Vektor der Laenge n darstellt.
 * <<<INPUT>>>
 * (comp) time		Komplexer Vektor, der transformiert werden soll (Darstellung der Funktion im Zeitraum)
 * (comp) freq		Komplexer Vektor, in den die Ergebnisse der Transformation (also die Darstellung der Funktion im Frequenzraum) abgespeichert werden
 * (unsigned int) n	Die Anzahl der Werte im Eingangs- bzw. Ausgangsvektor
 */
void fft(comp *time, comp *freq, unsigned int n);

#endif
