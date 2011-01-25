/*John Lomenick
ENGR 596-63
19 October 2009
Assignment 5

 This assignment does a Fast Fourier Transform of the input .wav file
*/




#include <stdio.h>
#include <windows.h>
#include "FFT.c"
#include <math.h>
#include <stdlib.h>




/************************************Wave header struct**********************************/
struct wave_header{
	char id1[4];
	long file_size;
	char id2[4];
	char id3[4];
	long header_size;
	short data_format;
	short num_channels;
	long sample_rate;
	long byte_rate;
	short block_align;
	short resolution;
	
	char id4[4];
	long data_size;
};

/********************* Returns a pointer to an array of the samples**************/
short *getBuffer(char filename[FILENAME_MAX], int *rate, int *num_samples){
	struct wave_header hdr;
	short *sample_buffer;
	FILE *f;

	
	f=fopen(filename,"rb");
		if(!f){
			printf("File does not exist");
			return;
		}
	
	/*read in header data*/
		if(fread(&hdr,sizeof(hdr),1,f)!=1){
			printf("Error reading header data");	
			return;
		}
		
		
	
	
	/*Check if this is supported wave file*/
		if(
			strncmp(hdr.id1,"RIFF", 4)!=0||
			strncmp(hdr.id2,"WAVE", 4)!=0||
			strncmp(hdr.id3,"fmt ", 4)!=0||
			(hdr.header_size!=16)        ||
			(hdr.data_format!=1)         ||
			(hdr.num_channels!=1)        ||
			(hdr.byte_rate!=(2*hdr.sample_rate))||
			(hdr.block_align!=2)         ||
			(hdr.resolution!=16)         ||
			(strncmp(hdr.id4,"data",4))!=0
			){
				printf("Error, Unsupported file (not wave file)\n");
				return;
			}
			
		*rate=hdr.sample_rate;
		*num_samples=hdr.data_size/2;
		sample_buffer=malloc(hdr.data_size);
		
		if(fread(sample_buffer,hdr.data_size,1, f)!=1){
			printf("Error reading sound data");
			return;
		}
		return sample_buffer;
		
}


/*************************************HANNING FUNCTION***********************************/
double hanning(int k, int num_samples){
	double pi=3.14158265359;
	
	return 0.50-0.50*(cos((double)(2*pi*k)/(num_samples-1)));
}

/*************************************POWER OF TWO FUNCTION*********************************/
int IsPowerOfTwo(int n)
{
    return ((n&(n-1))==0);
}

/****************************************squares number**************************************/
double square(double n){
	return n*n;
}

/***************************************MAIN METHOD********************************************/
main(){
	char inputname[FILENAME_MAX];
	char outputname[FILENAME_MAX];
	
	Complex *a;

	
	short *sample_buffer;
	
	double *scaled_buffer, *appended_buffer;
	double freq;
	double pk;
	double pi;
	
	int r; //sample rate
	int num_samples, num_samples2;
	int i,j,k,l;
	
	
	
	FILE *f;
	FILE *o;
    

	

	printf("Enter the name of the input file %d: \n");
	scanf("%s", &inputname);
		
	printf("Enter the name of the output file name:\n");
	scanf("%s", &outputname);
	
	sample_buffer=getBuffer(inputname, &r, &num_samples);
	
	
		
		
		
		printf("\nSample Rate:  %d\n", r);
		printf("Number of Samples:  %d\n", num_samples);
		
		//scale using hanning function
		 scaled_buffer=(malloc(num_samples*sizeof(double)));
		 for(k=0;k<num_samples;k++){
			scaled_buffer[k]=(double)sample_buffer[k]*hanning(k,num_samples);
		 }
		
		//append zeros so that num_samples is power of 2
		num_samples2=num_samples;
		while(!IsPowerOfTwo(num_samples2)){
			num_samples2++;
		}
	
		appended_buffer=(malloc(num_samples2*sizeof(double)));
		for(j=0;j<num_samples2;j++){
			if(j<num_samples){
			appended_buffer[j]=scaled_buffer[j];
			}
			else{
				appended_buffer[j]=0.0;
			}
		}
		

		
		
		//create array of complex numbers
		a= malloc(num_samples2*sizeof(Complex));
		
		

		for(k=0;k<num_samples2;k++){
			a[k].real=appended_buffer[k];
			a[k].imag=0.0;
		}
		
		
		
		//Fast Fourier Transform
		FFT(a,num_samples2);
		
		
		o=fopen(outputname, "w");
		for(k=0;k<(num_samples2/2);k++){
			
			freq=(k*(double)r)/num_samples2;
			pk=log10(square(a[k].real)+square(a[k].imag));
		
			fprintf(o, "%1.4f %1.4f\n", freq, pk);
			
		}	
		fclose(o);
		printf("Printed to file");
	
	
}
/************************************END MAIN**************************************/


