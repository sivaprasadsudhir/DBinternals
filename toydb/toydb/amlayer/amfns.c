# include <stdio.h>
# include "pf.h"
# include "am.h"
# include "vector.h"



# define help(X) printf("Debug %s: %s %d %s\n", (X), __FILE__,__LINE__, __func__)


extern int allocsDone;
extern int sizeallocated;

int myAtoi(char *str)
{
    int res = 0; // Initialize result
 	int i;
    // Iterate through all characters of input string and update result
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
 
    // return result.
    return res;
}


AM_BulkLoadLeaf(int fileDesc, char* fileName,int indexNo,char attrType,int attrLength,int MAXRECS, int* pagenNumNextLevel, int* valuesNextLevel, int* globalindex, int* leafSize, int earlyExit, char *readFilename)
{

	char *pageBuf; /* buffer for holding a page */
	char indexfName[AM_MAX_FNAME_LENGTH]; /* String to store the indexed
					 files name with extension           */
	int pageNum; /* page number of the root page(also the first page) */
	int errVal;
	int maxKeys;/* Maximum keys that can be held on one internal page */
	// vector_init(&pagenNumNextLevel);
	// vector_init(&valuesNextLevel);
	globalindex[0]=0;
	AM_LEAFHEADER head,*header;
	int recSize = attrLength + AM_ss;

	/* Check the parameters */
	if ((attrType != 'c') && (attrType != 'f') && (attrType != 'i'))
		{
		 AM_Errno = AME_INVALIDATTRTYPE;
		 return(AME_INVALIDATTRTYPE);
                 }      
	if ((attrLength < 1) || (attrLength > 255))
		{
		 AM_Errno = AME_INVALIDATTRLENGTH;
		 return(AME_INVALIDATTRLENGTH);
                }
	if (attrLength != 4)
		if (attrType !='c')
			{
			 AM_Errno = AME_INVALIDATTRLENGTH;
			 return(AME_INVALIDATTRLENGTH);
                        }
	
	header = &head;
	
	/* Get the filename with extension and create a paged file by that name*/
	sprintf(indexfName,"%s.%d",fileName,indexNo);
	


	
	/* allocate a new page for the root */
	
    
	int previousValue;
	int haveToAllocate = 1;
	// int recNum;
	int index;
	int first=1;

	FILE* file = fopen(readFilename, "r");
	int recNum = 0;
	fscanf(file, "%d", &recNum);

	// for(recNum=0; recNum<MAXRECS; ) {
	while (!feof (file))
	{
		// printf("%d\n", recNum);
		int value = recNum;
		
		if(haveToAllocate ==1 && first==1){
			first=0;
			index=1;
			previousValue=-1;
			errVal = PF_AllocPage(fileDesc, &pageNum, &pageBuf);
			AM_Check;
			allocsDone++;
			

			//if(earlyExit != 1)
			//printf("MARKER :%d\n", pageNum);
			
			pagenNumNextLevel[globalindex[0]]=pageNum;
			valuesNextLevel[globalindex[0]]=value;
			globalindex[0]++;

			// vector_add(&pagenNumNextLevel, temp1);
			// vector_add(&valuesNextLevel, temp2);

			/* initialise the header */
			header->pageType = 'l';
			header->nextLeafPage = AM_NULL_PAGE;
			header->recIdPtr = PF_PAGE_SIZE;
			header->keyPtr = AM_sl;
			header->freeListPtr = AM_NULL;
			header->numinfreeList = 0;
			header->attrLength = attrLength;
			header->numKeys = 0;
			/* the maximum keys in an internal node- has to be even always*/
			maxKeys = (PF_PAGE_SIZE - AM_sint - AM_si)/(AM_si + attrLength);
			if (( maxKeys % 2) != 0) 
				header->maxKeys = maxKeys - 1;
			else 
				header->maxKeys = maxKeys;
			/* copy the header onto the page */
			bcopy(header,pageBuf,AM_sl);
			// sizeallocated=sizeallocated+AM_sl;

			AM_LeftPageNum = pageNum;
			haveToAllocate=0;

		}
		else if(haveToAllocate==1 && first==0){

			first=0;
			haveToAllocate=0;
			previousValue=-1;
			// point old guy to new guy, unfix old guy
			AM_LEAFHEADER temp;
			AM_LEAFHEADER *tempheader=&temp;
			
			int newPageNum;
			char* newPageBuf;
			if(leafSize[0]==-1){
				if(earlyExit==1){
					leafSize[0]=index-1;
					return (AME_OK);
				}
			}
			index=1;

			errVal = PF_AllocPage(fileDesc,&newPageNum,&newPageBuf);

			AM_Check;
			allocsDone++;

			//printf("MARKER old, page:%d, %d\n", pageNum, newPageNum);
			bcopy(pageBuf,tempheader,AM_sl);
			tempheader->nextLeafPage = newPageNum;
			bcopy(tempheader,pageBuf,AM_sl);
			
			// sizeallocated=sizeallocated+()AM_ss+attrLength;	
			AM_LEAFHEADER thead1,*theader1;
			theader1=&thead1;
			bcopy(pageBuf,theader1,AM_sl);
			sizeallocated=sizeallocated+(theader1->numKeys)*(AM_ss+attrLength) + AM_sl;		
			errVal = PF_UnfixPage(fileDesc, pageNum, TRUE);
			AM_Check;

			pageNum = newPageNum;
			pageBuf = newPageBuf;
			
			pagenNumNextLevel[globalindex[0]]=pageNum;
			valuesNextLevel[globalindex[0]]=value;
			globalindex[0]++;

			// char temp1[20];
			// char temp2[20];
			// sprintf(temp1, "%d", pageNum);
			// sprintf(temp2, "%d", value);
			// printf("%d\n",myAtoi(temp1) );

			// vector_add(&pagenNumNextLevel, temp1);
			// vector_add(&valuesNextLevel, temp2);
			
			
			/* initialise the header */
			
			header->pageType = 'l';
			header->nextLeafPage = AM_NULL_PAGE;
			header->recIdPtr = PF_PAGE_SIZE;
			header->keyPtr = AM_sl;
			header->freeListPtr = AM_NULL;
			header->numinfreeList = 0;
			header->attrLength = attrLength;
			header->numKeys = 0;
			/* the maximum keys in an internal node- has to be even always*/
			maxKeys = (PF_PAGE_SIZE - AM_sint - AM_si)/(AM_si + attrLength);
			if (( maxKeys % 2) != 0) 
				header->maxKeys = maxKeys - 1;
			else 
				header->maxKeys = maxKeys;
			/* copy the header onto the page */
		
			bcopy(header,pageBuf,AM_sl);
			
		}

		if(earlyExit==0){
			int alreadyDone = index-1;
			int leftInLeaf = leafSize[0]-alreadyDone;
			int leftInFile = MAXRECS - recNum;
			if(leftInFile == (leafSize[0]+1)/2){
				if(leftInLeaf < leftInFile){
					haveToAllocate=1;
					continue;
				}

			}
		}

		if(index> leafSize[0] && leafSize[0]!=-1){
			haveToAllocate=1;
			continue;
		}

		if (previousValue == value)
		/* key is already present */ 
		{

			if (header->freeListPtr == 0)
				if ((header->recIdPtr - header->keyPtr) <(AM_si + AM_ss) || index>=leafSize[0] && leafSize[0]!=-1)
				{
					/* no room for one more record */
					haveToAllocate=1;
					continue;
				}
			/* insert into leaf - no need to split */
			AM_InsertToLeafFound(pageBuf,recNum,index,header);
			// sizeallocated=sizeallocated+AM_ss+attrLength;
			help("do not happen yet");	
			bcopy(header,pageBuf,AM_sl);
			haveToAllocate=0;
			// recNum++;

		fscanf (file, "%d", &recNum);  
			index++;

			previousValue=value;
			continue;
		}

		if ((header->freeListPtr) == 0){
    
			/* freelist empty */
			if ((header->recIdPtr - header->keyPtr) < (AM_si + AM_ss + recSize)){
				haveToAllocate=1;
				continue;
			}
				
			else
			{   
    			AM_InsertToLeafNotFound(pageBuf,&value,recNum,index,header);
				// sizeallocated=sizeallocated+AM_ss+attrLength;
				header->numKeys++;
    			bcopy(header,pageBuf,AM_sl);
				haveToAllocate=0;
				// recNum++;

		fscanf (file, "%d", &recNum);  
				index++;
				previousValue=value;				
				continue;
			}
		}

		else{ /* no place in the middle */
			if (((header->numinfreeList)*(AM_si + AM_ss) + header->recIdPtr -  header->keyPtr) > (recSize + AM_si + AM_ss))
			/*there is enough space in the freelist and in the middle put together */
			{
				char tempPage[PF_PAGE_SIZE];
	
				/* Compact the freelist so that we get enough space in the middle so that the new key can be inserted */
				AM_Compact(1,header->numKeys,pageBuf,tempPage,header);
				
				help("We think this should never happen\n");

				bcopy(tempPage,pageBuf,PF_PAGE_SIZE);
				bcopy(pageBuf,header,AM_sl);
				/* Insert into leaf a new key - no need to split */
				AM_InsertToLeafNotFound(pageBuf,&value,recNum,index,header);
				// sizeallocated=sizeallocated+AM_ss+attrLength;
				header->numKeys++;
				bcopy(header,pageBuf,AM_sl);
				haveToAllocate=0;
				// recNum++;
		fscanf (file, "%d", &recNum);  
				previousValue=value;
				continue;

			}
			else{ /* there is not enough room in the page */
				haveToAllocate=1;
				// recNum++;
		fscanf (file, "%d", &recNum);  
				index++;
				previousValue=value;
			}
		}


	}
	if(leafSize[0]==-1)
		leafSize[0]=MAXRECS;

	AM_LEAFHEADER thead,*theader;
	theader=&thead;
	bcopy(pageBuf,theader,AM_sl);
	sizeallocated=sizeallocated+(theader->numKeys)*(AM_ss+attrLength) + AM_sl;
	errVal = PF_UnfixPage(fileDesc,pageNum,TRUE);
	//AM_Check;
	
	/* Close the file */
	errVal = PF_CloseFile(fileDesc);
	AM_Check;
  	fclose(file);        


	return(AME_OK);

}


AM_BulkLoadInternal(int fileDesc, char* fileName,int indexNo,char attrType,int attrLength,int* pages, int* values, int* newPages, int* newValues, int* globalindex, int* leafSize, int earlyExit)
{

	char *pageBuf; /* buffer for holding a page */
	char indexfName[AM_MAX_FNAME_LENGTH]; /* String to store the indexed
					 files name with extension           */
	int pageNum=-1; /* page number of the root page(also the first page) */
	int errVal;
	int maxKeys;/* Maximum keys that can be held on one internal page */
	// vector_init(&pagenNumNextLevel);
	// vector_init(&valuesNextLevel);
	
	maxKeys = (PF_PAGE_SIZE - AM_sint - AM_si)/(AM_si + attrLength);
	if (( maxKeys % 2) != 0) 
		maxKeys--;
	

	int numLeaves = globalindex[0];
	globalindex[0]=0;

	AM_INTHEADER head,*header;
	int recSize = attrLength + AM_si;



	/* Check the parameters */
	if ((attrType != 'c') && (attrType != 'f') && (attrType != 'i'))
		{
		 AM_Errno = AME_INVALIDATTRTYPE;
		 return(AME_INVALIDATTRTYPE);
                 }      
	if ((attrLength < 1) || (attrLength > 255))
		{
		 AM_Errno = AME_INVALIDATTRLENGTH;
		 return(AME_INVALIDATTRLENGTH);
                }
	if (attrLength != 4)
		if (attrType !='c')
			{
			 AM_Errno = AME_INVALIDATTRLENGTH;
			 return(AME_INVALIDATTRLENGTH);
                        }
	
	header = &head;
	

	
	/* open the new file */
	
	/* allocate a new page for the root */
	
    
	int haveToAllocate = 1;
	int recNum;
	int index;
	int first=1;
	//printf("Numleaves %d\n", numLeaves);	
	for(recNum=0; recNum<numLeaves; ) {

		//printf("Recnum: %d\n", recNum);
		int value = values[recNum];
		int page = pages[recNum];

		if(haveToAllocate==1 && first!=1){
			if(leafSize[0]==-1){
				if(earlyExit==1){
					leafSize[0]=index-1;
					printf("exiting\n");
					return (AME_OK);
				}
			}
		}
		

		if(haveToAllocate==1){
			//printf("having to allocate\n");
			first=0;
			index=1;

			if(pageNum>0) {
				AM_LEAFHEADER thead,*theader;
				theader=&thead;
				bcopy(pageBuf,theader,AM_sl);
				sizeallocated=sizeallocated+(theader->numKeys)*(AM_si+attrLength) + AM_sint;
				PF_UnfixPage(fileDesc, pageNum, TRUE);
			}
			errVal = PF_AllocPage(fileDesc, &pageNum, &pageBuf);
			
			AM_Check;
			allocsDone++;
			//help("alloced");
			
			//printf("MARKER :%d\n", pageNum);
			
		
			newPages[globalindex[0]]=pageNum;
			newValues[globalindex[0]]=value;
			globalindex[0]++;

			// vector_add(&pagenNumNextLevel, temp1);
			// vector_add(&valuesNextLevel, temp2);

			/* initialise the header */
			header->pageType = 'i';
			header->numKeys=0;
			header->maxKeys=maxKeys;
			header->attrLength=attrLength;

			/* copy the header onto the page */
			bcopy(header,pageBuf,AM_sint);

			haveToAllocate=0;
			

		}
		
		if(earlyExit==0){
			int alreadyDone = index-1;
			int leftInLeaf = leafSize[0]-alreadyDone;
			int leftInFile = numLeaves - recNum;
			if(leftInFile == (leafSize[0]+1)/2){
				if(leftInLeaf < leftInFile){
					haveToAllocate=1;
					continue;
				}
			}
		}

		if(header->numKeys >= header->maxKeys || index>leafSize[0]&&leafSize[0]!=-1){
			haveToAllocate=1;
			continue;
		}

		if(index!=1){
			//bcopy((char*) &values[recNum], pageBuf + AM_sl + (index-1)*recSize + AM_si, attrLength);
			//bcopy((char*) &pages[recNum], pageBuf + AM_sl + (index-1)*recSize + attrLength, AM_si);
			bcopy((char*) &values[recNum], pageBuf + AM_sint + (index-1)*recSize + AM_si, attrLength);
			bcopy((char*) &pages[recNum], pageBuf + AM_sint + (index-1)*recSize + AM_si + attrLength, AM_si);
			// sizeallocated=sizeallocated+AM_si+attrLength;
			bcopy(pageBuf, header, AM_sint);
			header->numKeys++;
			bcopy(header, pageBuf, AM_sint);


		}
		if(index==1){
			bcopy((char*) &pages[recNum], pageBuf + AM_sint, AM_si);
			//bcopy((char*) &pages[recNum], pageBuf + AM_sl, AM_si);
			// sizeallocated=sizeallocated+AM_si;

			bcopy(pageBuf, header, AM_sint);
			header->numKeys++;
			bcopy(header, pageBuf, AM_sint);

		}
		index++;
		recNum++;
	}
	if(leafSize[0]==-1)
		leafSize[0]=numLeaves;

	AM_LEAFHEADER thead1,*theader1;
	theader1=&thead1;
	bcopy(pageBuf,theader1,AM_sl);
	sizeallocated=sizeallocated+(theader1->numKeys)*(AM_si+attrLength) + AM_sint;
	errVal = PF_UnfixPage(fileDesc,pageNum,TRUE);
	//AM_Check;
	
	/* Close the file */
	errVal = PF_CloseFile(fileDesc);
	AM_Check;

	return(AME_OK);

}


/* Creates a secondary idex file called fileName.indexNo */
AM_CreateIndex(char* fileName,int indexNo,char attrType,int attrLength)
{
	char *pageBuf; /* buffer for holding a page */
	char indexfName[AM_MAX_FNAME_LENGTH]; /* String to store the indexed
					 files name with extension           */
	int pageNum; /* page number of the root page(also the first page) */
	int fileDesc; /* file Descriptor */
	int errVal;
	int maxKeys;/* Maximum keys that can be held on one internal page */
	AM_LEAFHEADER head,*header;

	/* Check the parameters */
	if ((attrType != 'c') && (attrType != 'f') && (attrType != 'i'))
		{
		 AM_Errno = AME_INVALIDATTRTYPE;
		 return(AME_INVALIDATTRTYPE);
                 }
        
	if ((attrLength < 1) || (attrLength > 255))
		{
		 AM_Errno = AME_INVALIDATTRLENGTH;
		 return(AME_INVALIDATTRLENGTH);
                }
	
	if (attrLength != 4)
		if (attrType !='c')
			{
			 AM_Errno = AME_INVALIDATTRLENGTH;
			 return(AME_INVALIDATTRLENGTH);
                        }
	
	header = &head;
	
	/* Get the filename with extension and create a paged file by that name*/
	sprintf(indexfName,"%s.%d",fileName,indexNo);
	errVal = PF_CreateFile(indexfName);
	AM_Check;

	/* open the new file */
	fileDesc = PF_OpenFile(indexfName);
	if (fileDesc < 0) 
	  {
	   AM_Errno = AME_PF;
	   return(AME_PF);
          }

	/* allocate a new page for the root */
	errVal = PF_AllocPage(fileDesc,&pageNum,&pageBuf);
	AM_Check;
	allocsDone++;
	/* initialise the header */
	header->pageType = 'l';
	header->nextLeafPage = AM_NULL_PAGE;
	header->recIdPtr = PF_PAGE_SIZE;
	header->keyPtr = AM_sl;
	header->freeListPtr = AM_NULL;
	header->numinfreeList = 0;
	header->attrLength = attrLength;
	header->numKeys = 0;
	/* the maximum keys in an internal node- has to be even always*/
	maxKeys = (PF_PAGE_SIZE - AM_sint - AM_si)/(AM_si + attrLength);
	if (( maxKeys % 2) != 0) 
		header->maxKeys = maxKeys - 1;
	else 
		header->maxKeys = maxKeys;
	/* copy the header onto the page */
	bcopy(header,pageBuf,AM_sl);
	
	errVal = PF_UnfixPage(fileDesc,pageNum,TRUE);
	AM_Check;
	
	/* Close the file */
	errVal = PF_CloseFile(fileDesc);
	AM_Check;
	
	/* initialise the root page and the leftmost page numbers */
	AM_RootPageNum = pageNum;
	return(AME_OK);
}


/* Destroys the index fileName.indexNo */
AM_DestroyIndex(char* fileName,int indexNo)
{
	char indexfName[AM_MAX_FNAME_LENGTH];
	int errVal;

	sprintf(indexfName,"%s.%d",fileName,indexNo);
	errVal = PF_DestroyFile(indexfName);
	AM_Check;
	return(AME_OK);
}


/* Deletes the recId from the list for value and deletes value if list
becomes empty */
AM_DeleteEntry(int fileDesc,char attrType,int attrLength,char* value,int recId)
{
	char *pageBuf;/* buffer to hold the page */
	int pageNum; /* page Number of the page in buffer */
	int index;/* index where key is present */
	int status; /* whether key is in tree or not */
	short nextRec;/* contains the next record on the list */
	short oldhead; /* contains the old head of the list */
	short temp; 
	char *currRecPtr;/* pointer to the current record in the list */
	AM_LEAFHEADER head,*header;/* header of the page */
	int recSize; /* length of key,ptr pair for a leaf */
	int tempRec; /* holds the recId of the current record */
	int errVal; /* holds the return value of functions called within 
				                            this function */
	int i; /* loop index */


	/* check the parameters */
	if ((attrType != 'c') && (attrType != 'f') && (attrType != 'i'))
		{
		 AM_Errno = AME_INVALIDATTRTYPE;
		 return(AME_INVALIDATTRTYPE);
                }

	if (value == NULL) 
		{
		 AM_Errno = AME_INVALIDVALUE;
		 return(AME_INVALIDVALUE);
                }

	if (fileDesc < 0) 
		{
		 AM_Errno = AME_FD;
		 return(AME_FD);
                }

	/* initialise the header */
	header = &head;
	
	/* find the pagenumber and the index of the key to be deleted if it is
	there */
	status = AM_Search(fileDesc,attrType,attrLength,value,&pageNum,
			   &pageBuf,&index);
	
	/* check if return value is an error */
	if (status < 0) 
		{
		 AM_Errno = status;
		 return(status);
                }
	
	/* The key is not in the tree */
	if (status == AM_NOT_FOUND) 
		{
		 AM_Errno = AME_NOTFOUND;
		 return(AME_NOTFOUND);
                }
	
	bcopy(pageBuf,header,AM_sl);
	recSize = attrLength + AM_ss;
	currRecPtr = pageBuf + AM_sl + (index - 1)*recSize + attrLength;
	bcopy(currRecPtr,&nextRec,AM_ss);
	
	/* search the list for recId */
	while(nextRec != 0)
	{
		bcopy(pageBuf + nextRec,&tempRec,AM_si);
		
		/* found the recId to be deleted */
		if (recId == tempRec)
		{
			/* Delete recId */
			bcopy(pageBuf + nextRec + AM_si,currRecPtr,AM_ss);
			header->numinfreeList++;
			oldhead = header->freeListPtr;
			header->freeListPtr = nextRec;
			bcopy(&oldhead,pageBuf + nextRec + AM_si,AM_ss);
			break;
		}
		else 
	        {
			/* go over to the next item on the list */
			currRecPtr = pageBuf + nextRec + AM_si;
			bcopy(currRecPtr,&nextRec,AM_ss);
		}
	}
	
	/* if end of list reached then key not in tree */
	if (nextRec == AM_NULL)
		{
		 AM_Errno = AME_NOTFOUND;
		 return(AME_NOTFOUND);
                }
	
	/* check if list is empty */
	bcopy(pageBuf + AM_sl + (index - 1)*recSize + attrLength,&temp,AM_ss);
	if (temp == 0)
	{
		/* list is empty , so delete key from the list */
		for(i = index; i < (header->numKeys);i++)
			bcopy(pageBuf + AM_sl + i*recSize,pageBuf + AM_sl + 
				(i-1)*recSize,recSize);
		header->numKeys--;
		header->keyPtr = header->keyPtr - recSize;
	}
	
	/* copy the header onto the buffer */
	bcopy(header,pageBuf,AM_sl);
	
	errVal = PF_UnfixPage(fileDesc,pageNum,TRUE);
	
	/* empty the stack so that it is set for next amlayer call */
	AM_EmptyStack();
	  {
	   AM_Errno = AME_OK;
	   return(AME_OK);
          }
}








/* Inserts a value,recId pair into the tree */
AM_InsertEntry(int fileDesc,char attrType,int attrLength,char value,int recId)
{
	char *pageBuf; /* buffer to hold page */
	int pageNum; /* page number of the page in buffer */
	int index; /* index where key can be found or can be inserted */
	int status; /* whether key is old or new */
	int inserted; /* Whether key has been inserted into the leaf or 
	                      splitting is needed */
	int addtoparent; /* Whether key has to be added to the parent */ 
	int errVal; /* return value of functions within this function */
	char key[AM_MAXATTRLENGTH]; /* holds the attribute to be passed 
						  back to the parent */

	
	/* check the parameters */
	if ((attrType != 'c') && (attrType != 'f') && (attrType != 'i'))
		{
		 AM_Errno = AME_INVALIDATTRTYPE;
		 return(AME_INVALIDATTRTYPE);
                }

	if (value == NULL) 
		{
		 AM_Errno = AME_INVALIDVALUE;
		 return(AME_INVALIDVALUE);
                }

	if (fileDesc < 0) 
		{
		 AM_Errno = AME_FD;
		 return(AME_FD);
                }
	
	
	/* Search the leaf for the key */
	status = AM_Search(fileDesc,attrType,attrLength,value,&pageNum,
			   &pageBuf,&index);


	
	/* check if there is an error */
	if (status < 0) 
	{ 
		AM_EmptyStack();
		AM_Errno = status;
		return(status);
	}
	
	/* Insert into leaf the key,recId pair */
	inserted = AM_InsertintoLeaf(pageBuf,attrLength,value,recId,index,
				     status);

	/* if key has been inserted then done */
	if (inserted == TRUE) 
	{
		errVal = PF_UnfixPage(fileDesc,pageNum,TRUE);
		AM_Check;
		AM_EmptyStack();
		return(AME_OK);
	}
	
	/* check if there is any error */
	if (inserted < 0) 
	{
		AM_EmptyStack();
		AM_Errno = inserted;
		return(inserted);
	}
	
	/* if not inserted then have to split */
	if (inserted == FALSE)
	{
		/* Split the leaf page */
		addtoparent = AM_SplitLeaf(fileDesc,pageBuf,&pageNum,
			     attrLength,recId,value, status,index,key);
		
		/* check for errors */
		if (addtoparent < 0) 
		{
			AM_EmptyStack();
			{
			 AM_Errno = addtoparent;
			 return(addtoparent);
                        }
		}
		
		/* if key has to be added to the parent */
		if (addtoparent == TRUE)
		{
			errVal = AM_AddtoParent(fileDesc,pageNum,key,attrLength);
			if (errVal < 0)
			{
				AM_EmptyStack();
				AM_Errno = errVal;
				return(errVal);
			}
		}
	}
	AM_EmptyStack();
	return(AME_OK);
}


/* error messages */
static char *AMerrormsg[] = {
"No error",
"Invalid Attribute Length",
"Key Not Found in Tree",
"PF error",
"Internal error - contact database manager immediately",
"Invalid scan Descriptor",
"Invalid operator to OpenIndexScan",
"Scan Over",
"Scan Table is full",
"Invalid Attribute Type",
"Invalid file Descriptor",
"Invalid value to Delete or Insert Entry"
};


AM_PrintError(s)
char *s;

{
   fprintf(stderr,"%s",s);
   fprintf(stderr,"%s",AMerrormsg[-1*AM_Errno]);
   if (AM_Errno == AME_PF)
      /* print the PF error message */
      PF_PrintError(" ");
   else 
     fprintf(stderr,"\n");
}

