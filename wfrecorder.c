/*John Lomenick
ENGR 596-63
15 September 2009
Assignment 3

This assignment takes a recording of a given number of seconds, applies
the DC offset if prompted, normalizes if prompted, and gives the maximum and minimum sample
value of the recording
*/

#include <math.h>
#include <stdio.h>
#include <windows.h>

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))



/*wave header struct*/
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

short *record( double duration, int sample_rate, int *num_samples){
	short *sample_buffer;
	WAVEFORMATEX wfx = {WAVE_FORMAT_PCM, 1, 0, 0, 2 ,16};
	MMRESULT result;
	HWAVEIN hwi;
	WAVEHDR wh={0};
	char error_message[MAXERRORLENGTH];
	*num_samples=duration*sample_rate;
	sample_buffer=malloc(*num_samples*2);
	wfx.nSamplesPerSec=sample_rate;
	wfx.nAvgBytesPerSec=2*sample_rate;
	
	if((result=waveInOpen(&hwi, WAVE_MAPPER, &wfx, 0 ,0, CALLBACK_NULL))==MMSYSERR_NOERROR){
		wh.lpData=(LPSTR)sample_buffer;
		wh.dwBufferLength=*num_samples*2;
		if((result = waveInPrepareHeader(hwi, &wh, sizeof(wh)))==MMSYSERR_NOERROR && 
			(result = waveInAddBuffer(hwi, &wh, sizeof(wh)))==MMSYSERR_NOERROR &&
			(result = waveInStart(hwi))==MMSYSERR_NOERROR){
				printf("\nBegin Recording\n\n");
				//wait for recording to finish
				while(!(wh.dwFlags&WHDR_DONE))
					Sleep(10);
				printf("End Recording\n\n");
				if((result=waveInUnprepareHeader(hwi, &wh, sizeof(wh)))==MMSYSERR_NOERROR)
					result=waveInClose(hwi);
					
		}
		
	}
	if(result==MMSYSERR_NOERROR){
		return sample_buffer;
	}
	free(sample_buffer);
	if(waveInGetErrorText(result,error_message, MAXERRORLENGTH)==MMSYSERR_NOERROR){
		printf("waveInError: %s ", error_message);
		return 0;
	}
}

short *dcoffset(short *sample_buffer, double duration, int num_samples){
	int count;
	int avg;
	short max=0;
	short min=0;
	int i, j;
	int answer;
	count = 0;
	
	for(i=0;i<num_samples; i++){
		count= count+sample_buffer[i];
	}
	avg=round(count/num_samples);
	
	printf("DC Offset:  %d\n", avg);
	printf("Would you like to correct the DC offset? Enter '1' for yes or '2' for no: \n");
	scanf("%d", &answer);

	
	if(answer==1){
		for(j=0;j<num_samples;j++){
			sample_buffer[j]=sample_buffer[j]-avg;
		}
		
	}
	
	return sample_buffer;
}

short *normalize( short *sample_buffer, int num_samples){
	short max=0;
	short min=0;
	short k;
	int c;
	int i, j;
	int answer2;
	for(i=0;i<num_samples; i++){
		if(sample_buffer[i]>max){
			max=sample_buffer[i];
		}
		if(sample_buffer[i]<min){
			min=sample_buffer[i];
		}
	}
	
	printf("\nMaximum value in sample:  %d\n", max);
	printf("Minimum value in sample:  %d\n\n", min);
	printf("Would you like to normalize? Enter '1' for yes, '2' for no:  \n");
	scanf("%d", &answer2);
	
	if(answer2==1){
		if(abs(min)>max){
			k=(abs(min));
		}else{
			k=max;
		}
		c=round(32767/k);
		printf("C:  %d", c);
		for(i=0;i<num_samples; i++){
			sample_buffer[i]=sample_buffer[i]*c;
		}
	}
	return sample_buffer;
}



main(){
	char filename[30];
	int duration;
	int rate;
	struct wave_header hdr;
	short *sample_buffer, *sample_buffer2, *sample_buffer3;
	int num_samples;
	char header[sizeof(hdr)];
	FILE *f;

	


	
	
	printf("Enter the number of seconds you wish to record: \n");
	scanf("%d", &duration);
	
	printf("Enter a sample rate in samples/second:  \n");
	scanf("%d", &rate);
	
	printf("Enter a name for the file you wish to record ending in (.wav):  \n");
	scanf("%s", &filename);
	
	num_samples=rate*duration;
	
	sample_buffer = record(duration, rate, &num_samples);
	
	sample_buffer2 = dcoffset(sample_buffer, duration, num_samples);
	
	sample_buffer3 = normalize(sample_buffer2, num_samples);
	
	
	
	
	
	
	
	
	//create header
	strncpy(hdr.id1,"RIFF", 4);
	hdr.file_size=36+(num_samples*2);
	strncpy(hdr.id2,"WAVE",4);
	strncpy(hdr.id3,"fmt ",4);
	hdr.header_size=16;
	hdr.data_format=1;       
	hdr.num_channels=1;
	hdr.sample_rate=rate;
	hdr.byte_rate=2*rate;
	hdr.block_align=2;
	hdr.resolution=16;
	strncpy(hdr.id4,"data",4);
	hdr.data_size=(num_samples*2);
	
	//open file
	if((f=fopen(filename, "wb"))&& fwrite(&hdr,sizeof(hdr),1,f)==1 && fwrite(sample_buffer3,2*num_samples,1,f)==1 && !fclose(f))
		return;
/*
	fputs(hdr.id1, f);
	fputs(hdr.id2, f);
	fputs(hdr.id3, f);
	putc(hdr.header_size, f);
	putc(hdr.data_format, f);
	putc(hdr.num_channels, f);
	putc(rate,f);
	putc(hdr.byte_rate,f);
	putc(hdr.block_align, f);
	putc(hdr.resolution, f);
	fputs(hdr.id4, f);
	if(fwrite(sample_buffer,sizeof(sample_buffer),1,f)!=1)
		printf("Error writing buffer");
	*/
	
	
	
	
	
	
	
	
	
}