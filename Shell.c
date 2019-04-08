void fileInfo(char*, char**, int*);
int spaceIndex(char*);
int strCmp(char*, char*, int);
int interpret(char*,char**, int*, char*);

void main()
{
    char buffer[12288];
    char dir[512];
    char input[200];
    char* filenames[16];
    int sizes[17];
    int commandFlag;
    int k;
    for(k=0; k<16; ++k)
    {
        filenames[k] = '\0';   
    }
    sizes[16]= 0;
    interrupt(33,0,"    Welcome to BlackDos Command Shell \r\n\0",0,0);
    while(1)
    {
        interrupt(33,2,dir,257,0);
        fileInfo(dir, filenames, sizes);
        do
        {
            commandFlag=1;
            interrupt(33,0,"^(~(oo)~)^:  \0",0,0);
            interrupt(33,1,input,0,0);
            commandFlag=interpret(input, sizes, buffer);
        }while(commandFlag);
    }
}

void fileInfo(char* dir, char** filenames, int* sizes)
{
    int i, j;
    int dirInd=0;
    for(i=0; i<16; ++i)
    {
        if(dir[dirInd]=='\0')
        {
            dirInd+=32;
        }
        else if(dir[dirInd]<'a')
        {
            for(j=0; j<24; ++j)
            {
                if(dir[dirInd+8+j]==0)
                {
                    break;
                }
            }
            sizes[17]+= j;
            dirInd+=32;
        }
        else
        {
            filenames[i] = &dir[dirInd];
            for(j=0; j<24; ++j)
            {
                if(dir[dirInd+8+j]==0)
                {
                    break;
                }
            }
            sizes[i] = j;
            sizes[17]+= j;
            dirInd+=32;
        }
    }
}

int spaceIndex(char* input)
{
    int i=0;
    while(input[i] != ' ')
    {
        ++i;
    }
    return i;
}

int strCmp(char* a, char* b, int length)
{
    int i;
    for(i=0; i<length; ++i)
    {
        if(a[i] != b[i])
        {
            return 0;
        }
    }
    return 1;
}
int interpret(char* input, char** filenames, int* sizes, char* buffer)
{
    char file1[8];
    char file2[8];
    int i, end, numSect;
    int found = 0;
    int total = 0;
    int free=0;
    int spaceInd = spaceIndex(input);
    if(spaceInd!=4)
    {
        interrupt(33,0,"    Invalid Command \r\n\0",0,0);
        return 1;
    }

    if(strCmp(input, "boot", 4))
    {
        interrupt(33,11,0,0,0);
    }
    else if(strCmp(input, "clrs", 4))
    {
        interrupt(33,12,0,0,0);
    }
    else if(strCmp(input, "copy", 4))
    {
        end = spaceIndex(&input[5]);
        if(end>8)
        {
            interrupt(33,0,"    Invalid Command \r\n\0",0,0);
            return 1;
        }
        for(i=5; i<end; ++i)
        {
            file1[i-5]=input[i];
        }
        for(i=0; i<8; ++i)
        {
            file2[i]=input[end+1+i];
        }
        for(i=0; i<16; ++i)
        {
            if(strCmp(file1,filenames[i],8))
            {
                found = 1;
                numSect=sizes[i];
                break;
            }
        }
        if(found && file2[0]>='a')
        {
            interrupt(33,3,file1,buffer, &numSect);
            interrupt(33,8,file2,buffer, numSect);
        }
        else 
        {
            interrupt(33,15,1,0,0);
        }

    }
    else if(strCmp(input, "ddir", 4))
    {
        interrupt(33,0,"Directory Files and Sizes \r\n\0", 0,0);
        for(i=0; i<16; ++i)
        {
            if(filenames[i]!=0)
            {
                interrupt(33,0,filenames[i],0,0);
                interrupt(33,0,"    Size:  ",0,0);
                interrupt(33,13,sizes[i],0,0);
                interrupt(33,0," \r\n\0", 0,0);
            }
        }
        free = 255 - sizes[17];
        interrupt(33,0,"Space Used:  ",0,0);
        interrupt(33,13,sizes[17],0,0);
        interrupt(33,0,"    Free Space:  ", 0,0);
        interrupt(33,13,free,0,0);
        interrupt(33,0," \r\n\0", 0,0);
    }
    else if(strCmp(input, "echo", 4))
    {
        interrupt(33,0, &input[5], 0,0);
        interrupt(33,0," \r\n\0", 0,0);
    }
    else if(strCmp(input, "exec", 4))
    {
        interrupt(33,4,&input[5],4);
    }
    else if(strCmp(input, "help", 4))
    {
        interrupt(33,0,"    User Manual \r\n\0",0,0);
        interrupt(33,0,"boot: reboots system \r\n\0",0,0);
        interrupt(33,0,"clrs: clears screen \r\n\0",0,0);
        interrupt(33,0,"copy file1 file2: creates file2 and copies the content of file1 to it \r\n\0",0,0);
        interrupt(33,0,"ddir: prints user file names and sizes, total space used, and free space \r\n\0",0,0);
        interrupt(33,0,"echo comment: prints comment to display \r\n\0",0,0);
        interrupt(33,0,"exec filename: executes filename  \r\n\0",0,0);
        interrupt(33,0,"help: displays user manual \r\n\0",0,0);
        interrupt(33,0,"prnt filename: prints content of filename to printer  \r\n\0",0,0);
        interrupt(33,0,"remv filename: deletes file \r\n\0",0,0);
        interrupt(33,0,"senv: allows user to change set enviroment colors \r\n\0",0,0);
        interrupt(33,0,"show filename: displays file content on screen \r\n\0",0,0);
        interrupt(33,0,"twet filename: creates text file of 140 characters of less \r\n\0",0,0);
        interrupt(33,0,"Press Enter to continue r\n\0",0,0);
        interrupt(33,1,buffer,0,0);
    }
    else if(strCmp(input, "prnt", 4))
    {
        if(input[5]<'a')
        {
            interrupt(33,0,"    Invalid Command \r\n\0",0,0);
            return 1;
        }
        interrupt(33,3,&input[5],buffer,sizes);
        interrupt(33,0,buffer,1,0);
    }
    else if(strCmp(input, "remv", 4))
    {
        if(input[5]<'a')
        {
            interrupt(33,0,"    Invalid Command \r\n\0",0,0);
            return 1;
        }
        interrupt(33,7,&input[5],0,0);
    }
    else if(strCmp(input, "senv", 4))
    {
        interrupt(33,4,"Stenv\0",4);
    }
    else if(strCmp(input, "show", 4))
    {
        if(input[5]<'a')
        {
            interrupt(33,0,"    Invalid Command \r\n\0",0,0);
            return 1;
        }
        interrupt(33,3,&input[5],buffer, sizes);
        interrupt(33,0,buffer,0,0);
    }   
    else if(strCmp(input, "twet", 4))
    {
        if(input[5]<'a')
        {
            interrupt(33,0,"    Invalid Command \r\n\0",0,0);
            return 1;
        }
        interrupt(33,0,"Input line of text less that 140 characters:  \0",0,0);
        interrupt(33,1,buffer,0,0);
        buffer[141]='\0';
        interrupt(33,0,buffer,0,0);
    }
    else
    {
        interrupt(33,0,"    Invalid Command \r\n\0",0,0);
        return 1;
    }
    return 0;
}



