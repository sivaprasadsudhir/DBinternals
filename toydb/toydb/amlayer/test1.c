/**********************************************************************
test1.c: tests simple index insertion and scans. 
************************************************************************/
#include <stdio.h>
#include "vector.h"
#include "am.h"
#include "testam.h"
#include "pf.h"

#define MAXRECS	100
#define MAX_FNAME_LENGTH 80	/* max length for file name */
#define SIZE 1000000

main()
{
int id0,id1; /* index descriptor */
char ch;
int sd0,sd1; /* scan descriptors */
int i;
RecIdType recid;	/* record id */
char buf[NAMELENGTH]; /* buffer to store chars */
char fnamebuf[MAX_FNAME_LENGTH];	/* file name buffer */
char fnamebuf2[MAX_FNAME_LENGTH];
int recnum;	/* record number */
int numrec;		/* # of records retrieved*/
int fileDesc;
int pages[SIZE];
int values[SIZE];	

	PF_Init();
	// create index on the both field of the record
	int globalindex[1];
	int leafSize[1];
	leafSize[0]=-1;
	int earlyExit=1;
	// open the index 
	sprintf(fnamebuf2,"%s","testrelleaf");
	sprintf(fnamebuf, "%s", "testrel");
	AM_BulkLoadLeaf(fnamebuf2,0,INT_TYPE,sizeof(int),MAXRECS,pages,values,globalindex, leafSize, earlyExit);
	earlyExit=0;
	int actualLeafSize = leafSize[0];
	AM_BulkLoadLeaf(fnamebuf, 0, INT_TYPE, sizeof(int), MAXRECS, pages, values, globalindex, leafSize, earlyExit);

	printf("Leaf size %d\n", leafSize[0]);
	int j;
	for(j=0;j<globalindex[0];j++) {
		printf("Page:%d value:	%d\n", pages[j], values[j]);
	}
	

	
	char* filename="testrel.0";
	

	fileDesc = PF_OpenFile(filename);
	if (fileDesc < 0) 
	  {
	   AM_Errno = AME_PF;
	   	return(AME_PF);
       }


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

