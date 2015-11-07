/**********************************************************************
test1.c: tests simple index insertion and scans. 
************************************************************************/
#include <stdio.h>
#include "vector.h"
#include "am.h"
#include "testam.h"
#include "pf.h"

#define MAXRECS	32000
#define MAX_FNAME_LENGTH 80	/* max length for file name */
#define SIZE 10000

main()
{
int id0,id1; /* index descriptor */
char ch;
int sd0,sd1; /* scan descriptors */
int i;
RecIdType recid;	/* record id */
char buf[NAMELENGTH]; /* buffer to stor	e chars */
char fnamebuf[MAX_FNAME_LENGTH];	/* file name buffer */
char fnamebuf2[MAX_FNAME_LENGTH];
int recnum;	/* record number */
int numrec;		/* # of records retrieved*/
int fileDesc, fileDesc2;
int pages[SIZE];
int values[SIZE];	
int newPages[SIZE];
int newValues[SIZE];

	PF_Init();
	// create index on the both field of the record
	int globalindex[1];
	int leafSize[1];
	leafSize[0]=-1;
	int earlyExit=1;
	// open the index 
	sprintf(fnamebuf2,"%s","testrelleaf.temp");
	sprintf(fnamebuf, "%s", "testrel.0");
	int errVal = PF_CreateFile(fnamebuf2);
	AM_Check;

	fileDesc2 = PF_OpenFile(fnamebuf2);
	if(fileDesc2 < 0){
		printf("file not opened\n");
		return;
	}

	printf("\n*******************\n");
	AM_BulkLoadLeaf(fileDesc2, fnamebuf2,0,INT_TYPE,sizeof(int),MAXRECS,pages,values,globalindex, leafSize, earlyExit=1);
	earlyExit=0;
	PF_CloseFile(fileDesc2);

	int actualLeafSize = leafSize[0];
	leafSize[0]=leafSize[0];
	errVal = PF_CreateFile(fnamebuf);
	AM_Check;

	fileDesc = PF_OpenFile(fnamebuf);
	if(fileDesc < 0){
		printf("file 2 not opened\n");
		return;
	}

	AM_BulkLoadLeaf(fileDesc, fnamebuf, 0, INT_TYPE, sizeof(int), MAXRECS, pages, values, globalindex, leafSize, earlyExit=0);
	PF_CloseFile(fileDesc);
	printf("Leaf size %d\n", actualLeafSize);
	int j;
	for(j=0;j<globalindex[0];j++) {
		printf("Page:%d value:	%d\n", pages[j], values[j]);
	}
	int actualNumLeaves = globalindex[0];
	printf("\n*******************\n");




	int iter=0;
	while(1){
		iter++;
		sprintf(fnamebuf2, "%s.%d", "testrelinternal.temp", iter);
		
		
		errVal = PF_CreateFile(fnamebuf2);
		AM_Check;

		fileDesc2=PF_OpenFile(fnamebuf2);
		if(fileDesc2<0){
			printf("internals temp not opened\n");
			return;
		}
		earlyExit=1;
		leafSize[0]=-1;


		AM_BulkLoadInternal(fileDesc2, fnamebuf2,0,INT_TYPE,sizeof(int),pages,values, newPages, newValues, globalindex, leafSize, earlyExit=1);
		printf("Number of pointers from a node on level 2: %d\n", leafSize[0]);
		printf("\n*******************\n");

		PF_CloseFile(fileDesc2);
		fileDesc=PF_OpenFile(fnamebuf);

		globalindex[0]=actualNumLeaves;
		AM_BulkLoadInternal(fileDesc, fnamebuf, 0, INT_TYPE, sizeof(int), pages, values, newPages, newValues, globalindex, leafSize, earlyExit=0);
		PF_CloseFile(fileDesc);
		printf("Number of nodes on level 2 : %d\n", globalindex[0]);

		for(i=0; i<SIZE; i++){
			pages[i]=newPages[i];
			values[i]=newValues[i];
		}
			
		for(j=0;j<globalindex[0];j++) {
			printf("Page:%d value:	%d\n", pages[j], values[j]);
		}
		if(globalindex[0]==1){
			AM_RootPageNum=pages[0];
			break;
		}
	}






	char* filename="testrel.0";
	fileDesc = PF_OpenFile(filename);
	if (fileDesc < 0) 
	  {
	   AM_Errno = AME_PF;
	   	return(AME_PF);
       }


    char pageBuf[PF_PAGE_SIZE];


   	printf("\n*******************\n");

	
 
	/* Let's see if the insert works */
	sd1 = AM_OpenIndexScan(fileDesc,INT_TYPE,sizeof(int),EQ_OP,NULL);
	printf("retrieving recid's from scan descriptor %d\n",sd1);
	numrec = 0;
	while((recnum=RecIdToInt(AM_FindNextEntry(sd1)))>= 0){
		//printf("%d\n",recnum);
		numrec++;
	}
	printf("retrieved %d records\n",numrec);


	/* destroy everything */
	printf("closing down\n");
	AM_CloseIndexScan(sd1);
	PF_CloseFile(id1);
	AM_DestroyIndex(RELNAME,RECVAL_INDEXNO);

}

