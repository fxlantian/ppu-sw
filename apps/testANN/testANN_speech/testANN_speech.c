// Copyright 2015 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the ?¡ãLicense?¡À); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an ?¡ãAS IS?¡À BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

/*#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ann.h"
#include "int.h"
#include "event.h"
#include "../testANN.h"
#include "../testspeech.h"
#include "malloc.h"
#include "../data.h"
#include <math.h>


#define LEN_PART_MIN 10
#define LEN_MIN 20
#define OVERLAP (N / 8)                //OVERLAP = 64
sample_speech speech_arch[8];
sample_speech *speech = (sample_speech *)&speech_arch;

frame frame_arch;
frame *frames = (frame *)&frame_arch;

mfcc_frame mfcc_arch;
mfcc_frame *mfcc_frames = (mfcc_frame *)&mfcc_arch;

// mfcc_frame mfcc_arch;
// mfcc_frame *mfcc_ = (mfcc_frame *)&mfcc_arch;

mel_feature mel_arch;
mel_feature *mel_features = (mel_feature*)&mel_arch;

double *mfcc_frames1[180][26];
double *mel_features1[180][26];

static int mel_filters[N_FILTER] = {150, 200, 250, 300, 350, 400, 450,  //Linear
        490, 560, 640, 730, 840, 960, 1100, //500-1000Hz Logarithmisch
        1210, 1340, 1480, 1640, 1810, 2000, //1000-2000Hz Logarithmisch
        2250, 2520, 2840, 3190, 3580, 4020};  //2000-4000Hz Logarithmisch

int frame_n;
unsigned int n1 = 180; 
unsigned int n2 = 180;

double compare(double (*mfcc_frames1)[180][26], double (*mel_features1)[180][26])
{
  double distances[n1 + 1][n2 + 1];
  unsigned int i = 0, j = 0, k = 0;

  //Compute the local distances of all pairs of MFCC characteristics
  for (i = 0; i < n1; i++)
  {
    for (j = 0; j < n2; j++)
    {
      distances[i + 1][j + 1] = 0;
      for (k = 0; k < N_MFCC; k++)
        distances[i + 1][j + 1] += pow((*mfcc_frames1)[i][k] - (*mel_features1)[j][k], 2);
      distances[i + 1][j + 1] = sqrt(distances[i + 1][j + 1]);
      mel_features1++;
    }
    mfcc_frames1++;
  }

  //Fill the edges with infinity
  for (i = 0; i <= n1; i++)
    distances[i][0] = atof("Inf");
  for (i = 0; i <= n2; i++)
    distances[0][i] = atof("Inf");
  //The only valid starting point on the edge
  distances[0][0] = 0;

  //Calculate the best way from one end of the matrix to the other
  // for(i = 1; i<=n1; i++)
  //   for(j = 1; j <=n2; j++)
  //   {
  //     transform(distances[i][j]);
  //   }
  for (i = 1; i <= n1; i++)
    for (j = 1; j <= n2; j++)
    {
      //Determine the cheapest predecessor
      double prev_min = distances[i - 1][j];
      if (distances[i - 1][j - 1] < prev_min)
        prev_min = distances[i - 1][j - 1];
      if (distances[i][j - 1] < prev_min)
        prev_min = distances[i][j - 1];
      //Take the step out
      distances[i][j] += prev_min;
    }
  //Normalize the distance
  return distances[n1][n2] / sqrt(pow(n1, 2) + pow(n2, 2));
}

void new_word(mfcc_frame *frames, unsigned int n)
{
  unsigned int i = 0, j = 0;
  //In der Datei weden einfach alle Merkmale von Leerzeichen getrennt ausgegeben
  for (i = 0; i < n; i++)
    for (j = 0; j < N_MFCC; j++)
    {
      transform(frames[i].features[j]);
    }
}*/


/*void transform(double in_data)
{
  int data, unitplace, tenplace, hundredplace, thousandplace, tenthousandplace, hundredthousand;
  double mid_data;
  int data1 = (int)in_data;
  double data2 = in_data / data1;
  if(in_data > 0)
  {
    if(in_data >= 10)
    {
      data = (int)(in_data * 10000);
      unitplace = data / 1 % 10;
      tenplace = data / 10 % 10;
      hundredplace = data / 100 % 10;
      thousandplace = data / 1000 % 10;
      tenthousandplace = data / 10000 % 10;
      hundredthousand = data / 100000 % 10;
      printf("%d%d.%d%d%d%d\n", hundredthousand, tenthousandplace, thousandplace, hundredplace, tenplace, unitplace);
    }
    else
    {
      data = (int)(in_data * 10000);
      unitplace = data / 1 % 10;
      tenplace = data / 10 % 10;
      hundredplace = data / 100 % 10;
      thousandplace = data / 1000 % 10;
      tenthousandplace = data / 10000 % 10;
      printf("%d.%d%d%d%d\n", tenthousandplace, thousandplace, hundredplace, tenplace, unitplace);
    }
  }
  else
  {
    if(in_data == 0)
      printf("0.00000\n");
    else if(in_data <= -10)
    {
      mid_data = fabs(in_data);
      data = (int)(mid_data * 10000);
      unitplace = data / 1 % 10;
      tenplace = data / 10 % 10;
      hundredplace = data / 100 % 10;
      thousandplace = data / 1000 % 10;
      tenthousandplace = data / 10000 % 10;
      hundredthousand = data / 100000 % 10;
      printf("-%d%d.%d%d%d%d\n", hundredthousand, tenthousandplace, thousandplace, hundredplace, tenplace, unitplace);
    }
    else
    {
      mid_data = fabs(in_data);
      data = (int)(mid_data * 10000);
      unitplace = data / 1 % 10;
      tenplace = data / 10 % 10;
      hundredplace = data / 100 % 10;
      thousandplace = data / 1000 % 10;
      tenthousandplace = data / 10000 % 10;
      printf("-%d.%d%d%d%d\n", tenthousandplace, thousandplace, hundredplace, tenplace, unitplace);
    }
  }
}*/


/*unsigned int make_frames_hamming(sample_speech *speech, unsigned int n, frame **frames)
{
  unsigned int frame_number = n;
  comp *result = malloc(sizeof(comp) * frame_number * N);
  comp *data = malloc(sizeof(comp) * frame_number * N);
  // printf("data_addr is %08x\n", (int *)malloc(sizeof(comp) * frame_number * N));
  // printf("data value is %08x\n", data);
  unsigned int pos = 0, i = 0, j = 0;
  double pi = 4 * atan(1.0);

  *frames = malloc(sizeof(frame) * frame_number);
  //Erzeuge die Rahmen
  for (i = 0; i < frame_number; i++)             //frame_number is speech length
  {
    for (j = 0; j < N; j++)
    {
      //Filtere die Werte, sodass sie am Rand duerfen weniger Gewicht haben.
      //Die diskrete Fourier-Transformation setzt eine periodische Funktion voraus, somit wuerden die Werte am Rand des Rahmens diese verfaelschen.
      data[i * N + j].real = (speech->value[i][j]) * (0.54 + 0.46 * cos(2 * pi * (j - N / 2) / N));
      data[i * N + j].imag = 0;
    }
  }
  //Transformiere die einzelnen Rahmen
  for (i = 0; i < frame_number; i++)
    fft(data + i * N, result + i * N, N);
  //Berechne die Intensitaeten und ignoriere die Phasenverschiebung
  for (i = 0; i < frame_number; i++)
    for (j = 0; j < N; j++)
      (*frames)[i].magnitudes[j] = sqrt(pow(result[i * N + j].real, 2) + pow(result[i * N + j].imag, 2));
  //Normalisiere Intensitaeten
  for (i = 0; i < frame_number; i++)
  {
    double mean = 0;
    for (j = 0; j < N; j++)
      mean += (*frames)[i].magnitudes[j];
    mean /= N;
    for (j = 0; j < N; j++)
      (*frames)[i].magnitudes[j] /= mean;
  }
  return frame_number;
}

void fft(comp *time, comp *freq, unsigned int n)
{
  double pi = 4 * atan(1.0);
  unsigned int i = 0, j = 0, k = 0;
  unsigned int bits = 0;
  double cosine, midcos;
  double sine, midsin;
  unsigned int length_groups;
  comp off1_val, off2_val;
  comp off[1000];
  if (n == 1)
  {
    freq[0] = time[0];
    return;
  }

  //Zaehle die benoetigten Bits zur binaeren Darstellung von n
  for (i = n; i > 1; i /= 2)
    bits++;

  //Veraendere die Indizierung fuer das Aufteilen in die Haelften
  for (i = 0; i < n; i++)
  {
    int reversed = 0;
    for (j = 0; j < bits; j++)
      reversed |= (((i >> j) % 2) << (bits - j - 1));
    freq[reversed] = time[i];
  }

  //Fuer alle Laengen, die die Haelften haben
  for (i = bits; i > 0; i--)
  {
    //Laenge der Haelften
    length_groups = (n >> (i - 1));
    //Anzahl der Untergruppen
    unsigned int num_groups = n / length_groups;
    //Fuer alle Untergruppen
    for (j = 0; j < num_groups; j++)
    {
      //Fasse zwei Untergruppen zusammen
      for (k = 0; k < length_groups / 2; k++)
      {
        cosine = cos((-2 * pi * k * 1.0) / (length_groups * 1.0));
        sine = sin((-2 * pi * k * 1.0) / (length_groups * 1.0));
        //Zahl in der ersten Haelfte
        unsigned int off1 = length_groups * j + k;
        //Zahl in der zweiten Haelfte
        unsigned int off2 = off1 + length_groups / 2;
        //Multipliziere die komplexen Zahlen
        off1_val.real = freq[off1].real + (freq[off2].real * cosine - freq[off2].imag * sine);
        off1_val.imag = freq[off1].imag + (freq[off2].imag * cosine + freq[off2].real * sine);
        off2_val.real = freq[off1].real - (freq[off2].real * cosine - freq[off2].imag * sine);
        off2_val.imag = freq[off1].imag - (freq[off2].imag * cosine + freq[off2].real * sine);
        freq[off1] = off1_val;
        freq[off2] = off2_val;
      }
    }
  }
  //Normalisiere das Ergebnis
  for (i = 0; i < n; i++)
  {
    freq[i].real /= n;
    freq[i].imag /= n;
    freq[i].imag *= -1;
  }
}

void mfcc_features(frame *frames, unsigned int n, mfcc_frame *mfcc_frames)
{

  unsigned int i = 0, j = 0, k = 0;
  double filterOutput[N_FILTER];
  double filterSpectrum[N_FILTER][N] = {{0}};
  double c0 = sqrt(1.0 / N_FILTER);
  double cn = sqrt(2.0 / N_FILTER);
  double pi = 4 * atan(1.0);

  //Erzeuge fuer jeden Filter sein Spektrum.
  for (i = 0; i < N_FILTER; i++)
  {
    double maxFreq = 0, minFreq = 0, centerFreq = 0;

    //Der erste Mel-Filter besitzt unten keinen Nachbarfilter, also wird er symmetrisch gemacht.
    if (i == 0)
      minFreq = mel_filters[0] - (mel_filters[1] - mel_filters[0]);
    else
      minFreq = mel_filters[i - 1];
    centerFreq = mel_filters[i];

    //Der letzte Mel-Filter besitzt oben keinen Nachbarfilter, also wird er symmetrisch gemacht.
    if (i == N_FILTER - 1)
      maxFreq = mel_filters[N_FILTER - 1] + (mel_filters[N_FILTER - 1] - mel_filters[N_FILTER - 2]);
    else
      maxFreq = mel_filters[i + 1];

    //Berechne den Koeffizienten des Filters fuer jede Frequenz.
    for (j = 0; j < N; j++)
    {
      double freq = 1.0 * j * WAVE_SAMPLE_RATE / N;
      //Ist die Frequenz innerhalb des Filterbereiches?
      if (freq > minFreq && freq < maxFreq)
        //Bei der aufsteigenden oder absteigenden Flanke?
        if (freq < centerFreq)
          filterSpectrum[i][j] = 1.0 * (freq - minFreq) / (centerFreq - minFreq);
        else
          filterSpectrum[i][j] = 1 - 1.0 * (freq - centerFreq) / (maxFreq - centerFreq);
      else
        filterSpectrum[i][j] = 0;
    }
  }

  //Berechne die MFCC-Merkmale fuer jeden Rahmen.
  for (i = 0; i < n; i++)
  {
    //Setze jeden Filter ein und errechne die Summe.
    for (j = 0; j < N_FILTER; j++)
    {
      filterOutput[j] = 0;
      for (k = 0; k < N; k++)
        filterOutput[j] += frames[i].magnitudes[k] * filterSpectrum[j][k];
    }
    //Berechne die MFCC-Merkmale mithilfe der Summen und der diskreten Kosinus-Transformation.
    for (j = 0; j < N_MFCC; j++)
    {
      mfcc_frames[i].features[j] = 0;

      for (k = 0; k < N_FILTER; k++)
        mfcc_frames[i].features[j] += log(fabs(filterOutput[k]) + 1e-10) * cos((pi * (2 * k + 1) * j) / (2 * N_FILTER));

      if (j)
        mfcc_frames[i].features[j] *= cn;
      else
        mfcc_frames[i].features[j] *= c0;
    }
  }
}

int sample(unsigned int *buffer, unsigned int n, sample_speech *speech)
{
  unsigned int i = 0, j = 0, k = 0;
  k = (n / M) - 1;
  for(i = 0; i < k; i++)
  {
     for(j = 0; j < N; j++)
     {
      if(i*M +j < n)
      {
        speech->value[i][j] = buffer[i*M+j];
      }
      else 
        speech->value[i][j] = 0;
     }
  }
  return k;
}

void words_capture(unsigned int *buf, unsigned int size, int flag)
{
    unsigned int n = 0;
    n = sample(buf, size, speech);
    frame_n = make_frames_hamming(speech, n, &frames);
    mfcc_frames = malloc(sizeof(mfcc_frame) * frame_n);
    mfcc_features(frames, frame_n, mfcc_frames);
    if(flag)
    new_word(mfcc_frames, frame_n);
}

int main(void)
{
  int length,i,j;
  int pti = 0;
  double best = 1e10;
#if 0
  length = sizeof(speech_input[7])/sizeof(speech_input[0][0]);
  printf("the length value is %d\n", length);
  words_capture(speech_input[7], length, 1);
#endif
#if 1
  for(j=0;j<8;j++)
  {
   double (*mfcc_frames1)[180][26] = &mel_value[1];
   double (*mel_features1)[180][26] = &mel_value[j];
    double now = compare(mfcc_frames1, mel_features1);
    //transform(now);
    if (now < best)
    {
      best = now;
      printf("speaker\n");
    }
      if (best < 3.5)
        printf("speaker%d matches with speaker%d\n", pti, j);
    pti++;
  }
#endif
  printf("success\n");
return 0;
}*/








#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <uart.h>
#include "ee_printf.h"
double a[3][3] = {{1.0,2.0,3.0},
                    {4.0,5.0,6.0},
                    {7.0,8.0,9.0}};
double b[3][3] = {{3.0,5.0,7.0},
                    {2.0,8.0,4.0},
                    {1.0,9.0,6.0}};
  
double c[5][3] = {{1.0,2.0,3.0},
                      {4.0,5.0,6.0},
                    {7.0,8.0,9.0},
                      {0.0,0.0,0.0},
                      {0.0,0.0,0.0}};
  
double *mfcc_frames1[3][3];
double *mel_features1[5][3];
unsigned int n1 = 3, n2 = 5;
unsigned int N_MFCC = 3;
  
  double compare(double (*mfcc_frames1)[3][3], double (*mel_features1)[5][3])
  {
        double distances[n1 + 1][n2 + 1];
        unsigned int i = 0, j = 0, k = 0;
  
        for (i = 0; i < n1; i++)
        {
            for (j = 0; j < n2; j++)
            {
                distances[i + 1][j + 1] = 0;
                for (k = 0; k < N_MFCC; k++)
                    distances[i + 1][j + 1] += pow((*mfcc_frames1)[i][k] - (*mel_features1)[j][k], 2);
                  distances[i + 1][j + 1] = sqrt(distances[i + 1][j + 1]);
                  mel_features1++;
            }
        mfcc_frames1++;
        }
  
              for (i = 0; i <= n1; i++)
                      distances[i][0] = atof("Inf");
                for (i = 0; i <= n2; i++)
                        distances[0][i] = atof("Inf");
                  //The only valid starting point on the edge
                  distances[0][0] = 0;
                     for(i = 1; i<=n1; i++)
                       for(j = 1; j <=1; j++)
                       { 
                         ee_printf("distance is %lf\n", distances[i][j]);
                         //printf("distances=%f\n", distances[i][j]);
                       }
                    
                    for (i = 1; i <= n1; i++) 
                            for (j = 1; j <= 1; j++)
                                    { 
                                              //Determine the cheapest predecessor
                                              double prev_min = distances[i - 1][j];
                                                    if (distances[i - 1][j - 1] < prev_min)
                                                             prev_min = distances[i - 1][j - 1];
                                                          if (distances[i][j - 1] < prev_min)
                                                                      prev_min = distances[i][j - 1];
                                                                //Take the step out
                                                                distances[i][j] += prev_min;
                                                                    }
                                        for(i = 1; i<=n1; i++)
                                                for(j = 1; j <=n2; j++)
                                                {
                                                  ee_printf("distances1=%f\n", distances[i][j]);
                                                            }
                      //Normalize the distance
                      return distances[n1][n2] / sqrt(pow(n1, 2) + pow(n2, 2));
  }
  int main()
  {
          uart_set_cfg(0, 107);
          double (*mfcc_frames1)[3][3] = &a;
          double (*mel_features1)[5][3] = &c;
          double now = compare(mfcc_frames1, mel_features1);
          ee_printf("the value is %f\n", now);
          double c = cos(0.0);
          ee_printf("the c %f\n", c);
          return 0;
        }

