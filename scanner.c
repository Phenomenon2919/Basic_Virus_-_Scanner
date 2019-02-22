#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<pthread.h>
#include<openssl/md5.h>
#include <unistd.h>

#define MAX 999
#define BYTE 1024
#define T 5

unsigned int flag = 1;
unsigned int count = 0;
char dir_name[MAX] = {0};
char virus_signature[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD};
size_t size_signature = sizeof(virus_signature);
unsigned char md5_array[MD5_DIGEST_LENGTH] = {0};

struct file_info{
	char name[MAX];
	char md5hash[MD5_DIGEST_LENGTH];

};
struct file_info file_list[MAX];

void construct_MD5(char path[])
{
	MD5_CTX md5var;
	//unsigned char c[MD5_DIGEST_LENGTH] = {0};
	unsigned char temp_array[BYTE];
	size_t n;
	FILE *fp = fopen(path,"rb");
	if(fp == NULL)
	{
		fprintf(stderr,"Cannot open file %s\n",path);
		strcpy(md5_array,"\0");
	}
	else
	{
		MD5_Init(&md5var);
		while ((n = fread (temp_array, sizeof(char), sizeof(temp_array), fp)) != 0)
			MD5_Update(&md5var,temp_array,n);
		MD5_Final(md5_array,&md5var);
		fclose(fp);

	}
}

int find_if_present(char path[])
{
	for(int i=0;i<count;i++)
	{
		if(strcmp(file_list[i].name,path)==0)
			return i;
	}
	return -1;
}

void add_file_to_list(char path[])
{
	unsigned char c[MD5_DIGEST_LENGTH];
	construct_MD5(path);
	if(strcmp(md5_array,"\0")==0)
	{
		printf("Cannot add file %s\n",path );
	}
	else
	{
		strcpy(file_list[count].name,path);
		memcpy(file_list[count].md5hash,md5_array, MD5_DIGEST_LENGTH);
		count++;
	}


}

void check_signature(char path[])
{
	FILE *fp = fopen(path,"rb");
	size_t n, pos = 0;
	char buffer_array[size_signature];

	while((n = fread(buffer_array, sizeof(char), size_signature, fp))==size_signature)
	{
		if(memcmp(buffer_array, virus_signature, size_signature) == 0)
		{
			printf("File [%s] is infected\n", path);
			fclose(fp);
			return;
		}
		++pos;
		fseek(fp,pos,SEEK_SET);
	}
	fclose(fp);
}

void print_md5(unsigned char md5_hash[])
{
	for(int x = 0; x < MD5_DIGEST_LENGTH; x++)
        printf("%02x", md5_hash[x]);
    printf("\n");
}


void scan_func(char directory_name[])
{
	struct dirent *file_name;
	DIR *current;
	current = opendir(directory_name);
	int index;

	if(current)
	{
		while((file_name = readdir(current)) != NULL)
		{
			char path[MAX] = {0};
			if(file_name->d_type == DT_DIR && strcmp(file_name->d_name,".")!=0 && strcmp(file_name->d_name,"..")!=0)
			{
				strcpy(path,directory_name);
	        	strcat(path,"/");
	        	strcat(path,file_name->d_name);
	        	scan_func(path);
			}
			else if(file_name->d_type != DT_DIR)
			{
				strcpy(path,directory_name);
	        	strcat(path,"/");
	        	strcat(path,file_name->d_name);

	        	if((index = find_if_present(path))!=-1)
	        	{
	        		construct_MD5(path);
	        		//print_md5(md5_array);
	        		//print_md5(file_list[index].md5hash);
	        		//printf("%ld\t%ld\n",strlen(md5_array),strlen(file_list[index].md5hash));
	        		//printf("%d\n",memcmp(md5_array,file_list[index].md5hash, MD5_DIGEST_LENGTH));
	        		if(memcmp(md5_array,file_list[index].md5hash, MD5_DIGEST_LENGTH)!=0)
	        		{
	        			//printf("%d\n",memcmp(md5_array,file_list[index].md5hash, MD5_DIGEST_LENGTH));
	        			//print_md5(md5_array);
	        			//print_md5(file_list[index].md5hash);
	        			check_signature(path);
	        		}
	        	}
	        	else
	        	{
	        		add_file_to_list(path);
	        		//printf("%s\n",path );
	        	}

			}
		}
	}
	else
	{
		printf("The directory %s cannot be opened.\n",dir_name);
		exit(1);
	}

}

void free_file_list()
{
	/*for(int i=0;i<count;i++)
	{
		free(file_list[i]);
	}*/
	count = 0;
}

void* start_scan(void* args)
{
	while(flag==1)
	{
		scan_func(dir_name);
		sleep(T);
	}
	pthread_exit(NULL);
}

//The path to directory to be scan is sent by command line arguments
int main(int argc, char const *argv[])
{
	/* code */
	char command;
	char dummy;
	pthread_t scan;
	strcpy(dir_name,argv[1]);
	if(pthread_create(&scan, NULL, start_scan,NULL)!=0){
		fprintf(stderr,"The scanner could not start functioning!!");
		exit(1);
	}
	
	printf("\nEnter 'Q' to stop scanner:\n>>>");
	command = getchar();
	dummy = getchar();
	
	while(1)
	{
		switch(command)
		{
			case 'Q':
				printf("\nExiting the scanner..\n");
				flag = 0;
				pthread_join(scan,NULL);
				free_file_list();
				fflush(stdout);
				exit(0);
				break;
			default:
				printf("\nEnter 'Q' to exit\n>>>");
				fflush(stdout);
				command = getchar();
				dummy = getchar();
				fflush(stdin);
		}
	}
	return 0;
}