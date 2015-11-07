/**********************************************************************
test1.c: tests simple index insertion and scans. 
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "vector.h"
#include "am.h"
#include "testam.h"
#include "pf.h"

int MAXRECS;
#define MAX_FNAME_LENGTH 80	/* max length for file name */
#define SIZE 10000


int allocsDone;
int sizeallocated;
extern AM_RootPageNum;

main()
{
	system("bash ../data/run.sh 100000");
	double totalTime = 0;

	struct timeval tim;
	gettimeofday(&tim, NULL);
	double t1 = tim.tv_sec + (tim.tv_usec/1000000.0);

	system("sort -n ../data/out > ../data/outSorted");

	gettimeofday(&tim, NULL);
	double t2 = tim.tv_sec + (tim.tv_usec/1000000.0);

	totalTime = t2 - t1;

	printf("Time = %.2lfms\n", 1e3*(t2-t1));

	// int it = system("wc -l ../data/outSorted");
	// printf("%d-----------\n", it);

	MAXRECS = 0;
	FILE* file1 = fopen("../data/outSorted", "r");
	int findNumRecs = 0;
	fscanf(file1, "%d", &findNumRecs);    
	MAXRECS ++;
	while (!feof (file1))
	{
		fscanf (file1, "%d", &findNumRecs);      
		MAXRECS ++;
	}
  	fclose(file1);        
  	MAXRECS --;

  	// printf("-----------%d-----------\n", MAXRECS);



	gettimeofday(&tim, NULL);
	double t3 = tim.tv_sec + (tim.tv_usec/1000000.0);

allocsDone=0;
sizeallocated=0;
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
		// printf("file not opened\n");
		return;
	}

	// printf("\n*******************\n");
	AM_BulkLoadLeaf(fileDesc2, fnamebuf2,0,INT_TYPE,sizeof(int),MAXRECS,pages,values,globalindex, leafSize, earlyExit=1, "../data/outSorted");
	earlyExit=0;
	PF_CloseFile(fileDesc2);

	int actualLeafSize = leafSize[0];
	sizeallocated=0;
	

	// leafSize[0]=7;                      // CHANGE THIS TO CHANGE LEAF SIZE **************************************************
	int leafSizeNumber=leafSize[0];

	errVal = PF_CreateFile(fnamebuf);
	AM_Check;

	fileDesc = PF_OpenFile(fnamebuf);
	if(fileDesc < 0){
		// printf("file 2 not opened\n");
		return;
	}

	AM_BulkLoadLeaf(fileDesc, fnamebuf, 0, INT_TYPE, sizeof(int), MAXRECS, pages, values, globalindex, leafSize, earlyExit=0, "../data/outSorted");
	PF_CloseFile(fileDesc);
	// printf("Leaf size %d\n", actualLeafSize);
	int j;
	for(j=0;j<globalindex[0];j++) {
		// printf("Page:%d value:	%d\n", pages[j], values[j]);
	}
	int actualNumLeaves = globalindex[0];
	int numberOfLeaves=actualNumLeaves;

	// printf("\n*******************\n");

	sprintf(fnamebuf2, "%s", "testrelinternal.temp");
		
		
	errVal = PF_CreateFile(fnamebuf2);
	AM_Check;

	fileDesc2=PF_OpenFile(fnamebuf2);
	if(fileDesc2<0){
		// printf("internals temp not opened\n");
		return;
	}
	earlyExit=1;
	leafSize[0]=-1;

	int tempsize=sizeallocated;


	AM_BulkLoadInternal(fileDesc2, fnamebuf2,0,INT_TYPE,sizeof(int),pages,values, newPages, newValues, globalindex, leafSize, earlyExit=1);
	

	// leafSize[0]=6; // 				CHANGE THIS TO CHANGE INTERNAL SIZE *********************************************
	int internalSizeNumber=leafSize[0];

	// printf("Number of pointers from an internal node : %d\n", leafSize[0]);
	// printf("\n*******************\n");
	
	actualLeafSize=leafSize[0];
	sizeallocated=tempsize;
	PF_CloseFile(fileDesc2);
	


	int iter=1;
	// globalindex[0]=-1;
	while(1){
		iter++;
		
		globalindex[0]=actualNumLeaves;
		fileDesc=PF_OpenFile(fnamebuf);

		if(globalindex[0]==1){
			// printf("Entering this\n");
			AM_RootPageNum=pages[0];
			break;
		}

		AM_BulkLoadInternal(fileDesc, fnamebuf, 0, INT_TYPE, sizeof(int), pages, values, newPages, newValues, globalindex, leafSize, earlyExit=0);
		PF_CloseFile(fileDesc);
		// printf("Number of nodes on level %d : %d\n", iter, globalindex[0]);
		actualNumLeaves=globalindex[0];
		for(i=0; i<SIZE; i++){
			pages[i]=newPages[i];
			values[i]=newValues[i];
		}
			
		for(j=0;j<globalindex[0];j++) {
			// printf("Page:%d value:	%d\n", pages[j], values[j]);
		}
		// printf("\n*******************\n");
	}
	allocsDone-=2;



	gettimeofday(&tim, NULL);
	double t4 = tim.tv_sec + (tim.tv_usec/1000000.0);

	double insertTime = t4 - t3;
	printf("Time for loading = %.2lf\nms", 1e3*(insertTime));
	totalTime += t4 - t3;


	int internalSize = (allocsDone-numberOfLeaves)*(AM_sint + AM_si + (AM_si + sizeof(int)*internalSizeNumber));
	int leafSizeData = (numberOfLeaves)*(AM_sl + (AM_ss + sizeof(int))*leafSizeNumber);
	int totalSize = internalSize + leafSizeData;
	printf("Time\t#Levels\tRootID\t#Nodes\t#LeafNodes\tSpaceAlloc\tSpaceUsed\n");
	printf("%.2lfms\t%d\t%d\t%d\t%d\t%d\t\t%d\n",1e3*(totalTime) , iter-1, AM_RootPageNum, allocsDone, numberOfLeaves, totalSize, sizeallocated);

	// printf("\n*******************\n");
	   	
	//int actualRoot = AM_RootPageNum;
	//for(i=0;i<actualRoot; i++){
	//  	AM_RootPageNum=i;
	//   	AM_PrintTreeBulk(fileDesc, sizeof(int));
	//}
	//AM_RootPageNum=actualRoot;
	//printf("\n*******************\n");
	// printf("Root id %d\n", AM_RootPageNum);
	// printf("Total number of nodes = %d\n", allocsDone);
	// printf("Total space allocated in bytes = %d\n", PF_PAGE_SIZE*allocsDone);
	// printf("Total amount of space used = %d\n", sizeallocated);
	int h=100;
	fileDesc=PF_OpenFile(fnamebuf);
	if(fileDesc<0){
		printf("cannot open final\n");
	}
	//errVal = AM_InsertEntry(fileDesc, INT_TYPE, sizeof(int), (char*)&h, 100);
	//printf("errVal %d\n", errVal);



	char* filename="testrel.0";
	fileDesc = PF_OpenFile(filename);
	if (fileDesc < 0) 
	  {
	   AM_Errno = AME_PF;
	   	return(AME_PF);
       }


    char pageBuf[PF_PAGE_SIZE];


   	// printf("\n*******************\n");

	
 
	/* Let's see if the insert works */
	sd1 = AM_OpenIndexScan(fileDesc,INT_TYPE,sizeof(int),EQ_OP,NULL);
	// printf("retrieving recid's from scan descriptor %d\n",sd1);
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

