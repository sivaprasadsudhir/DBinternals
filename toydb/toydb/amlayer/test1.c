/**********************************************************************
test1.c: tests simple index insertion and scans. 
************************************************************************/
#include <stdio.h>
#include "am.h"
#include "testam.h"
#include "pf.h"

#define MAXRECS	600
#define MAX_FNAME_LENGTH 80	/* max length for file name */


main()
{
int id0,id1; /* index descriptor */
char ch;
int sd0,sd1; /* scan descriptors */
int i;
RecIdType recid;	/* record id */
char buf[NAMELENGTH]; /* buffer to store chars */
char fnamebuf[MAX_FNAME_LENGTH];	/* file name buffer */
int recnum;	/* record number */
int numrec;		/* # of records retrieved*/
int fileDesc;
	
	PF_Init();
	// create index on the both field of the record
	
	// open the index 
	sprintf(fnamebuf,"%s","testrel");
	AM_Bulkloading(fnamebuf,0,INT_TYPE,sizeof(int),MAXRECS);
	printf("Index created! :D\n");
	

	
	char* filename="testrel.0";
	

	fileDesc = PF_OpenFile(filename);
	if (fileDesc < 0) 
	  {
	   AM_Errno = AME_PF;
	   	return(AME_PF);
       }

    printf("HAHHA %d %d\n", fileDesc, id1);	

    char pageBuf[PF_PAGE_SIZE];



	/*
	int pageNum;
	for(pageNum=0; pageNum<4; pageNum++){
		int errVal = PF_GetThisPage(fileDesc,pageNum,&pageBuf);
		AM_Check;
		printf("please come here\n");
		bcopy(pageBuf,pageBuf,PF_PAGE_SIZE);


		errVal = PF_UnfixPage(fileDesc, pageNum);
		AM_Check;
	}
	*/
	
 
	/* Let's see if the insert works */
	sd1 = AM_OpenIndexScan(fileDesc,INT_TYPE,sizeof(int),EQ_OP,NULL);
	printf("retrieving recid's from scan descriptor %d\n",sd1);
	numrec = 0;
	while((recnum=RecIdToInt(AM_FindNextEntry(sd1)))>= 0){
		printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);


	/* destroy everything */
	printf("closing down\n");
	AM_CloseIndexScan(sd1);
	PF_CloseFile(id1);
	AM_DestroyIndex(RELNAME,RECVAL_INDEXNO);


}
