/* Prototypes */
void handleInterrupt21(int,int,int,int);
void printLogo();
void readString(char*);
void readInt(int*);
void writeInt(int);
void writeSector(char*,int);
void readSector(char* buffer, int sector);
void clearScreen(int,int);



void main()
{
   char buffer[512];
   makeInterrupt21();
   interrupt(33,2,buffer,258,0);
   interrupt(33,12,buffer[0]+1,buffer[1]+1,0);
   printLogo();
   interrupt(33,4,"Shell\0",2,0);
   interrupt(33,0,"Bad or missing command interpreter.\r\n\0",0,0);
   while (1) ;
  }

void printString(char* c, int d)
{
   int i = 0;
   while (c[i] != '\0')
   {
    if(d == 1)
      interrupt(23, c[i], 0, 0, 0);
    else
      interrupt(16 , 14*256+c[i],0,0,0);
   i++;
   }
   return;
}

void printLogo()
{
   interrupt(33,0,"       ___   `._   ____  _            _    _____   ____   _____ \r\n\0",0,0);
   interrupt(33,0,"      /   \\__/__> |  _ \\| |          | |  |  __ \\ / __ \\ / ____|\r\n\0",0,0);
   interrupt(33,0,"     /_  \\  _/    | |_) | | __ _  ___| | _| |  | | |  | | (___ \r\n\0",0,0);
   interrupt(33,0,"    // \\ /./      |  _ <| |/ _` |/ __| |/ / |  | | |  | |\\___ \\ \r\n\0",0,0);
   interrupt(33,0,"   //   \\\\        | |_) | | (_| | (__|   <| |__| | |__| |____) |\r\n\0",0,0);
   interrupt(33,0,"._/'     `\\.      |____/|_|\\__,_|\\___|_|\\_\\_____/ \\____/|_____/\r\n\0",0,0);
   interrupt(33,0," BlackDOS2020 v. 1.01, c. 2018. Based on a project by M. Black. \r\n\0",0,0);
   interrupt(33,0," Author(s): Alexis Alves.\r\n\r\n\0",0,0);
}

/* MAKE FUTURE UPDATES HERE */
/* VVVVVVVVVVVVVVVVVVVVVVVV */
void readString(char* input)
{
  int i = 0;
  char c;

  do
  {
    c = interrupt(22,0,0,0,0);
    interrupt(16,(14 * 256 + c),0,0,0);
    if(c == '\b')
	{
	  if(i > 0)
    {
	    --i;
      interrupt(16,(14 * 256 + ' '),0,0,0);
      interrupt(16,(14 * 256 + '\b'),0,0,0);
    }
	}
    else
	{
    if( c != '\n' && c != '\r')
      {
        input[i] = c;
        ++i;
      }
	}
}while(c != '\r');
    input[i] = '\0';
    interrupt(33,0,"\r",0,0);
    interrupt(33,0,"\n",0,0);
    interrupt(33,0,"\0",0,0);
}

void readInt(int* intorg)
{
  int i = 1;
  char n[6];
  readString(n);
  *intorg = n[0] - '0';
  while(n[i] != 0 && i<6)
  {
    *intorg *= 10;
    *intorg = (*intorg) + n[i] - '0';
    ++i;
  }
  printString("\r\n\0", 0);
}

int StsCompare(char* x, char* y)
{
  int i = 0;
  int flag = 1;
  while(x[i] != '\0')
  {
    if(x[i] != y[i])
    {
      flag = 0;
      break;
    }
    ++i;
  }
  return flag;
}

int mod(int a, int b)
{
    int x = a;
    while (x >= b) x = x - b;
    return x;
}

int div(int a, int b)
{
    int q = 0;
    while (q * b <= a) q++;
    return (q - 1);
}

void writeInt(int x)
{
    char n[6];
    int a = x;
    int r = 0;
    char* c = n + 5;
    *c = 0;
    --c;
    while(a > 0)
    {
      r = mod(a,10);
      a = div(a,10);
      *c = r + '0';
      --c;
    }
  ++c;
  printString(c, 0);
}

void readSector(char* buffer, int sector)
{

    int relSecNo = mod(sector, 18) + 1;
    int temp = div(sector,18);
    int head = mod(temp, 2);
    int track = div(sector, 36);
    int DX = head * 256;
    int CX = track*256+relSecNo;
    interrupt(19,513,buffer,CX,DX);
}

void writeSector(char* buffer, int sector)
{
    int relSecNo = mod(sector, 18) + 1;
    int temp = div(sector,18);
    int head = mod(temp, 2);
    int track = div(sector, 36);
    int DX = head * 256;
    int CX = track*256+relSecNo;
    interrupt(19,769,buffer,CX,DX);
}

void clearScreen(int back,int front)
{
    char clear[25];
    int i = 0;
    clear[25] = '\0';
    while( i < 24)
    {
      clear[i] = "\r\n";
      ++i;
    }
    printString(clear, 0);
    interrupt(16,512,0,0,0);
    if(back>0 && front>0)
    {
        if(back<10 && front<18)
        {
            interrupt(16, 1536, 4096*(back-1)+256*(front-1), 0, 6223);
        }
    }
}


void readFile(char* fname, char* buffer, char* size)
{
    char dir[512];
    int i;
    int index = 0;
    int found =1;
    int bufferindex = 0;
    int BoolFlag;
    readSector(dir, 257);
    while(bufferindex < 512)
    {
      BoolFlag = StsCompare(fname, &dir[bufferindex]);
        if(BoolFlag == 1)
      {
        index = bufferindex + 8;
          for(index ; dir[index] != 0; ++index)
          {
          *size += 1;
          readSector(buffer, dir[index]);
          buffer += 512;
          }
        return;
      }
      bufferindex = bufferindex + 32;
    }
    interrupt(33,15,0,0,0);
}

void writeFile(char* fname,char* buffer,int numberOfSectors)
{
  char dir[512];
  char map[512];
  int index = 0;
  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  int m = 0;
  int Is_Free = 0;
  int BoolFlag;
  readSector(dir, 257);
  readSector(map, 256);

    while( i < 17)
    {
        if(dir[m] == 0x0 )
        {
            ++Is_Free;
        }
        m += 32;
        i += 1;
    }
    if(Is_Free == 0)
    {
        interrupt(33, 15, 3, 0, 0);
        return;
    }
    while(index < 512)
    {
      BoolFlag = StsCompare(fname, &dir[index]);
        if(dir[index] != 0x0 && BoolFlag == 1 )
        {
                interrupt(33, 15, 1, 0, 0);
                return;
        }
        else if(dir[index] == 0)
        {
            break;
        }
        index += 32;
    }

    while(fname[j] != '\0')
    {
        dir[index + j] = fname[j];
        j += 1;
    }

    for(j = index; j < (index + 8); ++j)
    {
        dir[j] == 0;
    }

    index += 8;

    for(j = 0; j < numberOfSectors; ++j)
    {
          while(k <  512)
          {
            if(map[k] == 0)
            {
                map[k] = 255;
                dir[index + j] = k;

                writeSector(buffer, k);
                break;
            }
          k += 1;
        }
    }

    writeSector(dir, 257);
    writeSector(map, 256);
}

void deleteFile(char* fname)
{
   char dir[512];
   char map[512];
   int i, j;
   int bufferindex = 0;
   int mapindex =0;
   int BoolFlag;

   readSector(dir, 257);
   readSector(map, 256);

    while(i <16)
    {
      BoolFlag = StsCompare(fname, &dir[bufferindex]);
        if(BoolFlag == 1)
        {
            for(j; j < 8; ++j)
            {
                dir[bufferindex + j] = 0;
            }
            for(j = bufferindex + 8;dir[j] != 0;++j)
            {
                mapindex = dir[j];
                dir[j] = 0;
                map[mapindex] = 0;
            }

            writeSector(dir, 257);
            writeSector(map, 256);
            return;
        }
        i += 1;
    }

    interrupt(33, 15,0, 0, 0);

  }

  void ExecutesProgram(char* fname, int segment)
  {
    char buffer[4000];
    int offset=0;
    readFile(fname, buffer, 1);
    for(offset = 0 ; offset<4000 ; ++offset)
    {
        putInMemory(segment*4096, offset, buffer[offset]);
    }
    launchProgram(segment*4096);
  }

  void stop()
  {
    launchProgram(8192);
  }
  void error(int bx)
  {
    switch (bx) {
    case 0 :
      interrupt(33,0,"File not found.\r\n\0",0,0);
      stop();
      break;
    case 1 :
      interrupt(33,0,"Bad file name.\r\n\0",0,0);
      stop();
      break;
    case 2 :
      interrupt(33,0,"Disk full.\r\n\0",0,0);
      stop();
      break;
    default:
      interrupt(33,0,"General error \r\n\0",0,0);
      stop();
      break;
    }


  }


/* ^^^^^^^^^^^^^^^^^^^^^^^^ */
/* MAKE FUTURE UPDATES HERE */

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
   switch(ax) {
   case 0: printString(bx,cx); break;
   case 1: readString(bx); break;
   case 2: readSector(bx,cx); break;
   case 3: readFile(bx,cx,dx); break;
   case 4: ExecutesProgram(bx,cx); break;
   case 5: stop(); break;
   case 6: writeSector(bx,cx); break;
   case 7: deleteFile(bx); break;
   case 8: writeFile(bx,cx,dx); break;
   case 11: interrupt(25,0,0,0,0); break;
   case 12: clearScreen(bx,cx); break;
   case 13: writeInt(bx); break;
   case 14: readInt(bx); break;
   case 15: error(bx); break;
   default: printString("General BlackDOS error.\r\n\0", 0);

   }
}
