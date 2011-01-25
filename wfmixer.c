/*John Lomenick
ENGR 596-63
15 September 2009
Assignment 3

This assignment takes a number of .wav files with the same sample rate
and mixes them according to the input weight given.

*/

#include <math.h>
#include <stdio.h>
#include <windows.h>

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))


MMRESULT play(short *sample_buffer, int sample_rate, int num_samples){
  WAVEFORMATEX wfx={WAVE_FORMAT_PCM, 1, 0 , 0, 2, 16};
  MMRESULT result;
  HWAVEOUT hwo;
  WAVEHDR wh={0};

  
 wfx.nSamplesPerSec=sample_rate;
  wfx.nAvgBytesPerSec=2*sample_rate;
  if((result=waveOutOpen(&hwo,WAVE_MAPPER,&wfx,0,0,CALLBACK_NULL))==MMSYSERR_NOERROR){
    wh.lpData=(LPSTR)sample_buffer;
    wh.dwBufferLength=2*num_samples;
    if((result=waveOutPrepareHeader(hwo, &wh,sizeof(wh)))==MMSYSERR_NOERROR && (result=waveOutWrite(hwo, &wh, sizeof(wh)))==MMSYSERR_NOERROR){
      //wait for playing to finish                                                                                                                                                
      while(!(wh.dwFlags & WHDR_DONE)){
        Sleep(10);
        if((result=waveOutUnprepareHeader(hwo,&wh,sizeof(wh)))==MMSYSERR_NOERROR && (result=waveOutReset(hwo))==MMSYSERR_NOERROR){
          return waveOutClose(hwo);
        }
      }
    }
}
  return result; 
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
	
	if(abs(min)>max){
		k=(abs(min));
	}else{
		k=max;
	}
	c=(32767/k);
	for(i=0;i<num_samples; i++){
		sample_buffer[i]=sample_buffer[i]*c;
	}
	return sample_buffer;
}
  
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

struct data{
	short *sample_buffer;
	int weight;
	struct wave_header hdr;
	int num_samples;
}

main(){
	int num_files;
	struct data data[20];
	char filename[30];
	char outfilename[30];
	MMRESULT result;
	MMRESULT final_result;
	FILE *f;
	int curr_sample;
	int weight;
	int sample_rate;
	int i,j,k,l,m,n, p,q;
	char outfile[30];
	FILE *o;
	short *out_buffer;
	short *out_buffer_norm;
	int long_length;
	int total_weight;
	double current_sample;
	struct wave_header header;
	
	printf("Enter the number of files you wish to mix: \n");
	scanf("%d", &num_files);
	
	
	
	for(i=0;i<num_files;i++){
		printf("Enter the name of file %d: \n", i+1);
		scanf("%s", &filename);
	
		f=fopen(filename,"rb");
		if(!f){
			printf("File does not exist");
		}
	
	/*read in header data*/
		if(fread(&data[i].hdr,sizeof(data[i].hdr),1,f)!=1){
			printf("Error reading header data");						
		}
		
		
	
	
	/*Check if this is supported wave file*/
		if(
			strncmp(data[i].hdr.id1,"RIFF", 4)!=0||
			strncmp(data[i].hdr.id2,"WAVE", 4)!=0||
			strncmp(data[i].hdr.id3,"fmt ", 4)!=0||
			(data[i].hdr.header_size!=16)        ||
			(data[i].hdr.data_format!=1)         ||
			(data[i].hdr.num_channels!=1)        ||
			(data[i].hdr.byte_rate!=(2*data[i].hdr.sample_rate))||
			(data[i].hdr.block_align!=2)         ||
			(data[i].hdr.resolution!=16)         ||
			(strncmp(data[i].hdr.id4,"data",4))!=0
			){
				printf("Error, Unsupported file (not wave file)\n");
			}
			
			
			
		/*get sound data*/
		data[i].sample_buffer=malloc(data[i].hdr.data_size);
	
		if(fread(data[i].sample_buffer,data[i].hdr.data_size,1, f)!=1){
			printf("Error reading sound data");
			return;
		}
	
		data[i].num_samples=data[i].hdr.data_size/2;
		result=play(data[i].sample_buffer, data[i].hdr.sample_rate, data[i].num_samples);
		
		//get weight
		printf("Enter the weight of this file:  \n");
		scanf("%d", &weight);
		data[i].weight=weight;
		
		//find number of samples in each file
		data[i].num_samples=data[i].hdr.data_size/2;
	}
	
	printf("Enter the name of the output file (ending in '.wav'):\n");
	scanf("%s", &outfilename);
	
	
	//check for matching sample rates
	sample_rate=data[0].hdr.sample_rate;
	for(j=1;j<num_files;j++){
		if(data[j].hdr.sample_rate!=sample_rate){
			printf("Sample rates do not match");
			return;
		}
	}
	
	
	//find length of longest file
	long_length=data[0].num_samples;
	for(k=1;k<num_files;k++){
		if(data[k].num_samples>long_length){
			long_length=data[k].num_samples;
		}
	}

	
	
	
	
	//get total weight
	total_weight=0;
	for(p=0;p<num_files;p++){
		total_weight+=data[p].weight;
	}
	
	
	out_buffer=malloc(long_length*2);
	//create outbuffer
	for(m=0;m<long_length;m++){
		current_sample=0;
		for(n=0;n<num_files;n++){
			if(m>data[n].num_samples){
			}
			else{
				current_sample= current_sample + (data[n].sample_buffer[m]*data[n].weight);
			}
		}
		out_buffer[m]=(short)(current_sample/total_weight);
		
	}
	
	

	
	//create header for outgoing buffer
	strncpy(header.id1,"RIFF", 4);
	header.file_size=36+(long_length*2);
	strncpy(header.id2,"WAVE",4);
	strncpy(header.id3,"fmt ",4);
	header.header_size=16;
	header.data_format=1;       
	header.num_channels=1;
	header.sample_rate=data[0].hdr.sample_rate;
	header.byte_rate=2*data[0].hdr.sample_rate;
	header.block_align=2;
	header.resolution=16;
	strncpy(header.id4,"data",4);
	header.data_size=(long_length*2);
	

	//normalize outbuffer
	out_buffer_norm=normalize(out_buffer, long_length);

	
	
	final_result=play(out_buffer_norm, header.sample_rate, long_length);
	
	if((o=fopen(outfilename, "wb"))&& fwrite(&header,sizeof(header),1,o)==1 && fwrite(out_buffer_norm,2*long_length,1,o)==1 && !fclose(o))
		return;
		

	//play result
	
	
	
}

