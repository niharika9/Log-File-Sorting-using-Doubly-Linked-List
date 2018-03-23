#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#define __USE_XOPEN
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "my402list.h"
#include "cs402.h"
#include "warmup1.h"
#define LINE 1026
extern int errno;

static char gszProgName[MAXPATHLENGTH];

static
void Usage()
{
    fprintf(stderr, "Malformed Command:\n" );
    fprintf(stderr,
            "Usage: %s %s %s\n",
            gszProgName, "sort", "[tfile]");
    exit(-1);
}

static void SetProgramName(char *s)
{
  char *ptr=strrchr(s, DIR_SEP);
    if (ptr == NULL) 
     {
        strcpy(gszProgName, s);
    } else 
     {
        strcpy(gszProgName, ++ptr);
    }
}
static
void BubbleForward(My402List *pList, My402ListElem **pp_elem1, My402ListElem **pp_elem2)
{
    My402ListElem *elem1=(*pp_elem1), *elem2=(*pp_elem2);
    void *obj1=elem1->obj, *obj2=elem2->obj;
    My402ListElem *elem1prev=My402ListPrev(pList, elem1);

    My402ListElem *elem2next=My402ListNext(pList, elem2);

    My402ListUnlink(pList, elem1);
    My402ListUnlink(pList, elem2);
    if (elem1prev == NULL) {
        (void)My402ListPrepend(pList, obj2);
        *pp_elem1 = My402ListFirst(pList);
    } else {
        (void)My402ListInsertAfter(pList, obj2, elem1prev);
        *pp_elem1 = My402ListNext(pList, elem1prev);
    }
    if (elem2next == NULL) {
        (void)My402ListAppend(pList, obj1);
        *pp_elem2 = My402ListLast(pList);
    } else {
        (void)My402ListInsertBefore(pList, obj1, elem2next);
        *pp_elem2 = My402ListPrev(pList, elem2next);
    }
}

static
void BubbleSortForwardList(My402List *pList)
{
    My402ListElem *elem=NULL;
    int i=0;

    for (i=0; i < My402ListLength(pList); i++) 
     {
        int j=0, something_swapped=FALSE;
        My402ListElem *next_elem=NULL;

        for (elem=My402ListFirst(pList), j=0; j < My402ListLength(pList)-i-1; elem=next_elem, j++) 
	{
            long int cur_val= ((MyData *)(elem->obj))->timestamp_val;
	    long int next_val=0;

            next_elem=My402ListNext(pList, elem);
            next_val = (long int)((MyData*)(next_elem->obj))->timestamp_val;

            if (cur_val > next_val) 
	    {
                BubbleForward(pList, &elem, &next_elem);
                something_swapped = TRUE;
            }
	    if (cur_val==next_val)
            {
                fprintf(stderr, "Error Message : Identical Timestamp!\n");
                exit(0);
            }
        }
        if (!something_swapped) break;
    }

}
void My402Listtraverse(My402List *list)
{    
          My402ListElem *ptr=NULL;
          int balance=0;
          double bal=0.0f;
          double transamount=0.0f;
          char amount_str[15];
          char bal_str[15];
	 ptr=My402ListFirst(list);
 
        while(ptr != NULL)
        {
               MyData *d=(MyData*) (ptr->obj);
		
		//timestamp values 
                time_t rawtime = (time_t)d->timestamp_val;
                char *t = ctime(&rawtime);               
		
		//transaction amount
                transamount = (double)(d->amount%100)/100 + d->amount/100;                 
                if(strcmp(d->transaction_type,"+")==0)
                {
                  balance = balance +  d->amount;
                  if(transamount>=10000000.00f)
                  	sprintf(amount_str,"?,???,???.??");
                  else 
		  	sprintf(amount_str," %'12.2f ",transamount);
                }

                else if(strcmp(d->transaction_type,"-")==0){
                    balance-= d->amount;
                   if(transamount>=10000000.00f)
                   	sprintf(amount_str,"(?,???,???.?\?)");
                   
                  else 
		    	sprintf(amount_str,"(%'12.2f)",transamount);

                }

		// balance calculation
                bal = (double)(balance%100)/100 + balance/100;
                  if(bal>=10000000.00f)
                  {
                    sprintf(bal_str,"?,???,???.??");
                  }
                  else if (bal<0.0f)
		  {
                    if (fabs(bal)>10000000.00f) 
			sprintf(bal_str,"(?,???,???.?\?)"); 
                    else 
			sprintf(bal_str,"(%'12.2f)",fabs(bal));
                  }

                  else 
			sprintf(bal_str," %'12.2f ",bal);

	
		struct tm timeval;
                char time_str[26];

                memset(&timeval,0,sizeof(struct tm));
                strptime(t,"%a %b %e %H:%M:%S %Y",&timeval);
                strftime(time_str,sizeof(time_str),"%a %b %e %Y",&timeval);

                fprintf(stdout,"| %15s | %-24.24s | %14.14s | %14.14s |\n",time_str,d->description,amount_str,bal_str);
	  	ptr=My402ListNext(list, ptr);
        }
}

void My402listOutput(My402List* list)
{
    BubbleSortForwardList(list);
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n" );
    fprintf(stdout, "|       Date      | Description              |         Amount |        Balance |\n" );
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n" );   
    My402Listtraverse(list);
    fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n" ); 
}

int main(int argc, char *argv[])
{

SetProgramName(*argv);    

    if (argc<2 || argc >3)
    {
        Usage();
    }

    if (strcmp(argv[1],"sort")!=0)
    {
      Usage();
    }



My402List list;
memset(&list, 0, sizeof(My402List));

if(My402ListInit(&list) == 0)
{
  fprintf(stderr,"Error Message : Insufficient memory , Anchor node couldn't be intialized !\n");
  exit(0);
}
setlocale(LC_NUMERIC, "en_US");
int err_num;
FILE *fp;

    struct stat statbuf;

    stat(argv[2], &statbuf);
    
    if (S_ISDIR (statbuf.st_mode)) 
    {
     fprintf (stdout,"Error Message : %s is a directory !\n",argv[2]);
     exit(0);
    }



if(argv[2]==NULL)     
    fp = stdin;
else
    fp = fopen(argv[2],"r");

if(fp!= NULL)
{
     char buffer[LINE];
     char *trans_type;
     char *timestamp_str;
     long int timestamp;
     char *amount_str;
     double amountd;
     int amount;
     char *details;
     long int linenumber=0;
     
      while(fgets(buffer,sizeof(buffer),fp) != NULL)
	{
	  linenumber++;

         if(strlen(buffer) >1024)
	{
	  fprintf(stderr,"Error Message : Length of the line exceeds more than 1024 on line %ld !\n",linenumber);
	  exit(0);
	}

          char *space_ptr;          
	  char *string_copy;

	  string_copy = (char *)malloc(strlen(buffer)+ 1);
          strcpy(string_copy,buffer);
	if(string_copy==NULL)
	{
	    fprintf(stderr,"Error Message : Transaction history is empty on line number %ld !\n",linenumber);
	    exit(0);
	}
          char *start_str = string_copy;
	   
	// for the type of transaction
         if(*start_str == '\n')
	 {
	    fprintf(stderr,"Error Message : Transaction has no type on line %ld !\n",linenumber);
		exit(1);
	 }   

	  char *tabcount = strchr(start_str,'\t');
          char *start_str2 = start_str;              
          
             int num_tabs=0;
             while(tabcount!=NULL)
              {
                num_tabs++;
                start_str2 = ++tabcount;
                tabcount=strchr(start_str2,'\t');
              }

              if (num_tabs < 3)    
              {
                fprintf(stderr, "Error Message : Improper input format , too little fields on line %ld !\n",linenumber );
                exit(1);
              }
              else if (num_tabs>3)
              {
                fprintf(stderr, "Error Message : Improper input format , too many fields on line %ld !\n",linenumber );
                exit(1);
              }

         space_ptr = strchr(start_str,'\t');
	 
         if(space_ptr != NULL)
	 {
	    *space_ptr = '\0';
	    space_ptr++;
            trans_type = start_str;
 		if(strcmp(trans_type,"+")!=0 && strcmp(trans_type,"-")!=0)
	         {
		   fprintf(stderr,"Error Message : Improper Transaction type on line %ld !\n",linenumber);
		   exit(0); 
		 }
 	 }else
 	 {
           fprintf(stderr,"Error Message : Incorrect input format on line %ld !\n",linenumber);
 	     exit(1);	
         }
	  start_str = space_ptr ;  
	  
        //for timestamp
        if (*space_ptr=='\n')
          {
            fprintf(stderr, "Error Message : Transaction has no Timestamp on line %ld !\n",linenumber);
            exit(1);
          }
        space_ptr = strchr(start_str,'\t');
        if(space_ptr != NULL)
        {
          *space_ptr = '\0';
          space_ptr++;
	  timestamp_str = start_str;
        }        
	timestamp = strtol(timestamp_str,NULL,10);
         if(timestamp < 0)
	{
	   fprintf(stderr,"Error Message : Time stamp cannot be negative on line %ld !\n",linenumber);
	   exit(1);
	}
	time_t currtime = time(NULL);
	
       if(timestamp > currtime)
       {
	 fprintf(stderr,"Error Message : Time stamp greater than current time on line %ld !\n",linenumber);
         exit(0);
       }
        start_str = space_ptr;
	space_ptr = strchr(start_str,'\t');
        if(space_ptr !=NULL)
 	{
            *space_ptr = '\0';
	    space_ptr++;
 	    amount_str = start_str;	
	}
	  char *dot = strchr(amount_str,'.');
          if(dot ==NULL)
         {
	   fprintf(stderr,"Error Message : Amount has no decimals on line %ld !\n",linenumber);
		exit(1);           
	 }
         dot++;
          int count=0;
 	 while(strcmp(dot,"")!=0)
 	 {
	    dot++;
	    count++;
	 }
         if(count >2)
	 {
      	  fprintf(stderr,"Error Message : Amount has more than two digits after decimal on line %ld !\n",linenumber);
    exit(1);
	 }
	// convert string to decimal
 	sscanf(amount_str,"%lf",&amountd);
	amountd += 0.001f;
	if(amountd < 0)
	{
	 fprintf(stderr,"Error Message : Negative amount value on line %ld !\n",linenumber);
	 exit(0);
	}

 	char *amount_strcpy;
	amount_strcpy = (char *)malloc(sizeof(char) * strlen(amount_str));
	strcpy(amount_strcpy,amount_str);
	char *token;
	token = strtok(amount_strcpy,".");
	int len = strlen(token);
	 if(len >7)
         {
	   fprintf(stderr,"Error Message : Amount exceeds more than permitted amount pf 10,000,000.00 on line %ld !\n",linenumber);
	exit(0);
	 }	
         amountd = amountd*100.00f;
 	 amount = (int)amountd; 
	 start_str = space_ptr;

	// for description of data
         space_ptr = strchr(start_str,'\t');
	 if(space_ptr != NULL)
 	 {
	    fprintf(stderr,"Error Message : More than allowed fields on line %ld!\n",linenumber);
            exit(1);          
	 }
        space_ptr = strchr(start_str,'\n');  //copy it till end
        if(space_ptr !=NULL)
        {
	  *space_ptr = '\0';
            space_ptr++;
	    details = start_str;
        }
        
        // moving the pointer to skip the spaces
       while(isspace((unsigned char)*details))
        details++;
      
       if(*details == 0)
       {
        fprintf(stderr,"Error Message : Descrptions has only spaces on line %ld !\n",linenumber);
        exit(0);
       }

	MyData* data = (MyData*)malloc(sizeof(MyData));
        if(data==NULL)
	{
 	  fprintf(stderr,"Error Message : Insufficient memory, data structure couldn't be initialised !\n");
	  exit(0);
	}

	data->transaction_type = trans_type;
	data->timestamp_val = timestamp;
	data->description = details;
	data->amount = amount;

       My402ListAppend(&list,data);
	  
	}
	
   My402listOutput(&list);    
}else
{
err_num = errno;
fprintf(stderr, "Error Message : Could not open the file %s !\n",strerror(err_num));
exit(0);

}

return 0;
}
