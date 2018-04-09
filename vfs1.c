#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
//#include<io.h>
	
#define MAXINODE 50
#define MAXFILESIZE 1024
#define READ 1
#define WRITE 2
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2


typedef struct superblock
{
    int TotalInodes;
    int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int RefranceCount;
    int permission;
    struct inode *next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;


UFDT UFDTArr[MAXINODE];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;

void man(char *name)
{
    if(name==NULL)
    {
        return;
    }
    if(strcmp(name,"create")==0)
    {
        printf("Description : Used To create new regular file\n");
        printf("Usage : create File_name Permission");
    }
    else if(strcmp(name,"read")==0)
    {
        printf("Description : Used To read data from regular file\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(strcmp(name,"write")==0)
    {
        printf("Description : Used To write into regular file\n");
        printf("Usage : write File_name \n After this enter the dtaa we want to write\n");
    }
    else if(strcmp(name,"ls")==0)
    {
        printf("Description : Used to list all information of files\n");
        printf("Usage : ls\n");
    }
    else if(strcmp(name,"stat")==0)
    {
        printf("Description : Used to display information of files\n");
        printf("Usage : stat File_name\n");
    }
    else if(strcmp(name,"fstat")==0)
    {
        printf("Description : Used to display information of files\n");
        printf("Usage : stat File_descriptor\n");
    }
    else if(strcmp(name,"truncate")==0)
    {
        printf("Description : Used to remove data from files\n");
        printf("Usage : truncate File_name\n");
    }
    else if(strcmp(name,"open")==0)
    {
        printf("Description : Used to open existing files\n");
        printf("Usage : open File_name\n");
    }
    else if(strcmp(name,"close")==0)
    {
        printf("Description : Used to close open of files\n");
        printf("Usage : close File_name\n");
    }
    else if(strcmp(name,"closeall")==0)
    {
        printf("Description : Used to close all opened files\n");
        printf("Usage : stat File_name\n");
    }
    else if(strcmp(name,"lseek")==0)
    {
        printf("Description : Used to change file offset\n");
        printf("Usage : lseek File_name ChangeInOffset StartPoint\n");
    }
    else if(strcmp(name,"rm")==0)
    {
        printf("Description : Used to delete the file\n");
        printf("Usage : rm File_name\n");
    }
    else
    {
        printf("ERROR : No mannualy entry available\n");
    }
    
}

void DisplayHelp()
{
    printf("ls : List out all files\n");
    printf("clear : to clear console\n");
    printf("open : to open the file\n");
    printf("close : to close file\n");
    printf("closeall : to closeall file\n");
    printf("read : to read the content from file\n");
    printf("write : to write the content in the file\n ");
    printf("exit : terminate the file system\n");
    printf("stat : to display all information of file using name\n");
    printf("fstat : to display all information of file using file descriptor\n");
    printf("truncate : to remove data from file\n");
    printf("rm : to delte the file\n");  
}

int GetFDFromName(char *name)
{
    int i=0;
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable!=NULL)
        {
            if(strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
            {
                break;
            }
            i++;
            
        }
    }
    if(i==50)
    {
        return -1;
    }
    else
    {
        return i;
    }    

}

PINODE Get_Inode(char *name)
{
    PINODE temp=head;
    if(name==NULL)
    {
        return NULL;
    }
    while(temp!=NULL)
    {
        if(strcmp(name,temp->FileName)==0)
        {
            break;
        }
        temp=temp->next;
    }
    return temp;
}

void CreateDILB()
{
    int i=0;
    PINODE newn =NULL;
    PINODE temp=head;

    while(i <= MAXINODE)
    {
        newn=(PINODE)malloc(sizeof(INODE));
        newn->LinkCount= newn->RefranceCount=0;
        newn->Buffer=NULL;
        newn->InodeNumber=i;
        if(temp==NULL)
        {
            head = newn;
            temp = head;
        }
        else
        {
            temp->next=newn;
            temp=temp->next;
        }
        i++;
    }
}

void IntialiseSuperBlock()
{
    int i=0;
    while(i<50)
    {
        UFDTArr[i].ptrfiletable=NULL;
        i++;
    }
    SUPERBLOCKobj.TotalInodes=MAXINODE;
    SUPERBLOCKobj.FreeInodes=MAXINODE;
}

int CreatFile(char *name,int permission)
{
    int i=0;
    PINODE temp=head;

    if((name==NULL)||(permission==0)||(permission>3))
    {
        return -1;
    }
    if(SUPERBLOCKobj.FreeInodes==0)
    {
        return -2;
    }
    if(Get_Inode(name)!=NULL)
    {
        return -3;
    }
    (SUPERBLOCKobj.FreeInodes)--;

    while(temp!=NULL)
    {
        if(temp->FileType==0)
        {
            break;
        }
        temp=temp->next;
    }

    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
        {
            break;
        }
        i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable==NULL)
    {
        return -4;
    }
    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=permission;
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    UFDTArr[i].ptrfiletable->ptrinode=temp;
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->ptrinode->FileType=REGULAR;
    UFDTArr[i].ptrfiletable->ptrinode->RefranceCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->LinkCount=1;
    UFDTArr[i].ptrfiletable->ptrinode->FileSize=MAXFILESIZE;
    UFDTArr[i].ptrfiletable->ptrinode->FileActualSize=0;
    UFDTArr[i].ptrfiletable->ptrinode->permission=permission;
    UFDTArr[i].ptrfiletable->ptrinode->Buffer=(char *)malloc(MAXFILESIZE);
    memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);
    return i;
}

int rm_File(char *name)
{
    int fd = 0;
    fd=GetFDFromName(name);
    if(fd==-1)
    {
        return -1;
    }
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable=NULL;
    (SUPERBLOCKobj.FreeInodes)++;
    return 1;
}

int ReadFile(int fd,char *arr,int isize)
{
    int read_size=0;
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->mode != READ) &&
       (UFDTArr[fd].ptrfiletable->mode != READ+WRITE))
    {
        return -2;
    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->permission != READ) &&
       (UFDTArr[fd].ptrfiletable->ptrinode->permission != READ+WRITE))
    {
        return -2;
    }
    if(UFDTArr[fd].ptrfiletable->readoffset==UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
    {
        return -3;
    }
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
    {
        return -4;
    }
    read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)-(UFDTArr[fd].ptrfiletable->readoffset);
    if(read_size<isize)
    {
        strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);
        UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+read_size;
    }
    else
    {
       strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),isize);
        UFDTArr[fd].ptrfiletable->readoffset=UFDTArr[fd].ptrfiletable->readoffset+isize; 
    }
    return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode)!=WRITE)&&((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))
    {
        return -1;
    }
    if(((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=WRITE)&&((UFDTArr[fd].ptrfiletable->ptrinode->permission)!=READ+WRITE))
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->writeoffset)==MAXFILESIZE)
    {
        return -2;
    }
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)
    {
        return -3;
    }

    
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);
    (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+isize;
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
    return isize;
}

int OpenFile(char *name,int mode)
{
    int i=0;
    PINODE temp=NULL;

    if(name==NULL || mode <= 0)
    {
        return -1;
    }
    temp=Get_Inode(name);

    if(temp==NULL)
    {
        return -2;
    }
    if(temp->permission < mode)
    {
        return -3;
    }
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
        {
            break;
        }
        i++;
    }
    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable==NULL)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=mode;

    if(mode==READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    else if(mode==READ)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
    }
    else if(mode==WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    UFDTArr[i].ptrfiletable->ptrinode=temp;
    (UFDTArr[i].ptrfiletable->ptrinode->RefranceCount)++;
    return i;
}

/*void CloseFileByName(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    (UFDTArr[fd].ptrfiletable->ptrinode->RefranceCount)--;
}*/

int CloseFileByName(char *name)
{
    int i=0;

    i=GetFDFromName(name);
    if(i==-1)
    {
        return -1;
    }
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;
    (UFDTArr[i].ptrfiletable->ptrinode->RefranceCount)--;
    return 0;
}

void CloseAllFile()
{
    int i=0;
    while(i<50)
    {
        if(UFDTArr[i].ptrfiletable!=NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset=0;
            UFDTArr[i].ptrfiletable->writeoffset=0;
            (UFDTArr[i].ptrfiletable->ptrinode->RefranceCount)--;
        
        }
        i++;
    }
}
int LseekFile(int fd,int size,int from)
{
    if((fd<0)||(from>2))
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -1;
    }
    if((UFDTArr[fd].ptrfiletable->mode==READ)||(UFDTArr[fd].ptrfiletable->mode==READ+WRITE))
    {
        if(from==CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+size;

        }
        else if(from==START)
        {
            if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                return -1;
            }
            if(size<0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset)=size;
        }
        else if(from==END)
        {
           if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>MAXFILESIZE)
            {
                return -1;
            } 
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->readoffset)=((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size);
        }
    }
    else if(UFDTArr[fd].ptrfiletable->mode==WRITE)
    {
        if(from==CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) < 0)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
            }
        }
        else if(from==START)
        {
            if(size>MAXFILESIZE)
            {
                return -1;
            }
            if(size<0)
            {
                return -1;
            }
            if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=size;
            }
            (UFDTArr[fd].ptrfiletable->writeoffset)=size;
        }
        else if(from== END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size>MAXFILESIZE)
            {
                return -1;
            }
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
            {
                return -1;
            }
            (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
        }
    }
    return 1;
}

void ls_file()
{
    PINODE temp=head;

    if(SUPERBLOCKobj.FreeInodes==MAXINODE)
    {
        printf("ERROR : There are no file\n");
        return;
    }

    printf("\n Filw Name\tInode Number\tFile Size \tlink count\n");
    while(temp!=NULL)
    {
        if(temp->FileType!=0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileSize,temp->LinkCount);
        }
        temp=temp->next;
    }
    
}

int fstat_file(int fd)
{
    PINODE temp=head;

    if(fd<0)
    {
        return -1;
    }
    if(UFDTArr[fd].ptrfiletable==NULL)
    {
        return -2;
    }
    temp=UFDTArr[fd].ptrfiletable->ptrinode;
    printf("\n-------------------Statical Information---------------------------\n");
    printf("File Name : %s\n",temp->FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp->FileSize);
    printf("File Actual Size : %d\n",temp->FileActualSize);
    printf("Link Count : %d\n",temp->LinkCount);
    printf("Refrance Count : %d\n",temp->RefranceCount);
    if(temp->permission==1)
    {
        printf("File Permission : Read Only\n");
    }
    else if(temp->permission==2)
    {
        printf("File Permission : Write Only\n");
    }
    if(temp->permission==3)
    {
        printf("File Permission : Read & WRITE \n");
    }
    return 0;
}
int stat_file(char *name)
{
    PINODE temp=head;
    if(name==NULL)
    {
        return-1;
    }
    while(temp!=NULL)
    {
        if(strcmp(name,temp->FileName)==0)
        {
            break;
        }
        temp=temp->next;
    }
    if(temp==NULL)
    {
        return -2;
    }
    printf("\n-------------------Statical Information---------------------------\n");
    printf("File Name : %s\n",temp->FileName);
    printf("Inode Number : %d\n",temp->InodeNumber);
    printf("File Size : %d\n",temp->FileSize);
    printf("File Actual Size : %d\n",temp->FileActualSize);
    printf("Link Count : %d\n",temp->LinkCount);
    printf("Refrance Count : %d\n",temp->RefranceCount);
    if(temp->permission==1)
    {
        printf("File Permission : Read Only\n");
    }
    else if(temp->permission==2)
    {
        printf("File Permission : Write Only\n");
    }
    if(temp->permission==3)
    {
        printf("File Permission : Read & WRITE \n");
    }
    return 0;

}
int truncate_File(char *name)
{
    int fd=GetFDFromName(name);
    if(fd==-1)
    {
        return-1;
    }
    memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize=0;
    
    return 1;//added by me for warning : control may reach end of non void function
}
int main()
{
    char *ptr=NULL;
    int ret=0,fd=0,count=0;
    char command[4][80],str[80],arr[1024];
    IntialiseSuperBlock();
    CreateDILB();
    while(1)
    {
        fflush(stdin);
        strcpy(str,"");
        printf("\n[Hydra: VFS]$ ");
        fgets(str,80,stdin);
        count=sscanf(str,"%s%s%s%s",command[0],command[1],command[2],command[3]);

        if(count==1)
        {
            if(strcmp(command[0],"ls")==0)
            {
                ls_file();
            }
            else if(strcmp(command[0],"closeall")==0)
            {
                CloseAllFile();
                printf("\nclose all files\n");
                continue;
            }
            else if(strcmp(command[0],"clear")==0)
            {
                system("cls");
                continue;
            }
            else if(strcmp(command[0],"help")==0)
            {
                DisplayHelp();
                continue;
            }
            else
            {
                printf("\nERROR : Command not Found\n");
            }

        }
        else if(count==2)
        {
            if(strcmp(command[0],"stat")==0)
            {
                ret = stat_file(command[1]);
                if(ret==-1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }
                if(ret==-2)
                {
                    printf("ERROR : There is no such file\n");
                }
                continue;
            }
            
            else if(strcmp(command[0],"fstat")==0)
            {
                ret = fstat_file(atoi(command[1]));
                if(ret==-1)
                {
                    printf("ERROR : Incorrect Parameters\n");
                }
                if(ret==-2)
                {
                    printf("ERROR : There is no such file\n");
                }
                continue;
            }
            else if(strcmp(command[0],"close")==0)
            {
                ret=CloseFileByName(command[1]);
                if(ret==-1)
                {
                    printf("ERROR : There is no such file\n");
                }
                continue;
            }
            else if(strcmp(command[0],"rm")==0)
            {
                ret=rm_File(command[1]);
                if(ret==-1)
                {
                    printf("ERROR : There is no such File\n");
                }
                continue;
            }
            else if(strcmp(command[0],"man")==0)
            {
                man(command[1]);
            }
            else if(strcmp(command[0],"write")==0)
            {
                fd=GetFDFromName(command[1]);
                if(fd==-1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                printf("Enter the Data :\n");
                scanf("%[^\n]",arr);
                ret = strlen(arr);
                if(ret==0)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret = WriteFile(fd,arr,ret);
                if(ret==-1)
                {
                    printf("ERROR : Permission Dennied\n");
                }
                else if(ret==-2)
                {
                    printf("ERROR : There is No suffiecient Memory to write\n");
                }
                else if(ret==-3)
                {
                    printf("ERROR : it is not regular file\n");
                }
            }
            else if(strcmp(command[0],"truncate")==0)
            {
                ret=truncate_File(command[1]);
                if(ret==-1)
                {
                    printf("ERROR : Incorrect Parameter");
                }
            }
            else
            {
                printf("ERROR : Command not found!!!\n");
                continue;
            }
        }
        else if(count==3)
        {
            if(strcmp(command[0],"create")==0)
            {
                ret = CreatFile(command[1],atoi(command[2]));
                if(ret>=0)
                {
                    printf("File Sucessfully created\n");
                }
                if(ret==-1)
                {
                    printf("ERROR : Incorrect Parameter\n");
                }
                if(ret==-2)
                {
                    printf("ERROR : There Is no Inodes\n");
                }
                if(ret==-3)
                {
                    printf("ERROR : File already exist\n");
                }
                if(ret==-4)
                {
                    printf("ERROR : Memory Allocation Failluare\n");
                }
                continue;
            }
            else if(strcmp(command[0],"open")==0)
            {
                ret=OpenFile(command[1],atoi(command[2]));
                if(ret>=0)
                {
                    printf("file is opend sucessfully with fd %d\n",ret);
                }
                if(ret==-1)
                {
                    printf("ERROR : Incorrect Parameter\n");
                }
                if(ret==-2)
                {
                    printf("ERROR : file not peresent\n");
                }
                if(ret==-3)
                {
                    printf("ERROR : Permission Denied\n");
                }
                continue;
            }
            else if(strcmp(command[0],"read")==0)
            {
                fd=GetFDFromName(command[1]);
                if(fd==-1)
                {
                    printf("ERROR : Incorrect Parameter\n");
                    continue;
                }
                ptr=(char *)malloc(sizeof(atoi(command[2]))+1);
                if(ptr==NULL)
                {
                    printf("ERROR : Memory allocation failluare\n");
                    continue;
                }
                ret=ReadFile(fd,ptr,atoi(command[2]));
                if(ret==-1)
                {
                    printf("ERROR : Memory Allocation \n");
                }
                if(ret==-2)
                {
                    printf("ERROR : File Not existing \n");
                }
                if(ret==-3)
                {
                    printf("ERROR : Reached end of file \n");
                }
                if(ret==-4)
                {
                    printf("ERROR : It is not Regular File \n");
                }           
                if(ret==0)
                {
                    printf("ERROR : File Is empty \n");
                }
                if(ret>0)
                {
                    write(2,ptr,ret);
                }
                continue;
            }
            else
            {
                printf("ERROR : Command not Found\n");
                continue;
            }
        }
        else if(count==4)
        {
            if(strcmp(command[0],"lseek")==0)
            {
                fd=GetFDFromName(command[1]);
                if(fd==-1)
                {
                    printf("ERROR : Incorrect parameter\n");
                    continue;
                }
                ret=LseekFile(fd,atoi(command[2]),atoi(command[3]));
                if(ret==-1)
                {
                    printf("ERROR : Unable to perform lseek\n");
                }

            }
            else
            {
                printf("\n ERROR : Command Not found\n");
                continue;
            }
        }
        else
        {
            printf("\n ERROR : Command Not found\n");
            continue;
        }
    }
    return 0;
}
