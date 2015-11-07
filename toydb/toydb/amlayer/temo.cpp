void (int * pages, int* values, int size) {
	
	if(size==1) {

		AM_RootPageNum=pages[i];

		return;
	}

	int* pagenNumNextLevel;
	int* valuesNextLevel;
	int globalindex[0];
    
	int previousValue;
	int haveToAllocate = 1;
	int recNum;
	int index;
	int first=1;
	int i;
	for(i=0; i<globalindex; ) {

		//printf("%d\n", recNum);
		int value = values[i];


		if(haveToAllocate ==1 && first==1){
			first=0;
			index=1;
			previousValue=-1;
			errVal = PF_AllocPage(fileDesc, &pageNum, &pageBuf);
			AM_Check;
			printf("MARKER :%d\n", pageNum);

			pagenNumNextLevel[globalindex[0]]=pageNum;
			valuesNextLevel[globalindex[0]]=values[i];
			globalindex[0]++;

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
			haveToAllocate=0;

			//Add the first key of the int node as pages[0]
			//i++

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
			index=1;
			errVal = PF_AllocPage(fileDesc,&newPageNum,&newPageBuf);
			AM_Check;

			printf("MARKER old, page:%d, %d\n", pageNum, newPageNum);
			bcopy(pageBuf,tempheader,AM_sl);
			tempheader->nextLeafPage = newPageNum;
			bcopy(tempheader,pageBuf,AM_sl);
			
			errVal = PF_UnfixPage(fileDesc, pageNum, TRUE);
			AM_Check;

			pageNum = newPageNum;
			pageBuf = newPageBuf;
			
			pagenNumNextLevel[globalindex[0]]=pageNum;
			valuesNextLevel[globalindex[0]]=value;
			globalindex[0]++;

		
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
			//Insert the page num as first pointer
			//i++
			// continue;

		}

		

		if (previousValue == value)
		/* key is already present */ 
		{

			if (header->freeListPtr == 0)
				if ((header->recIdPtr - header->keyPtr) <(AM_si + AM_ss))
				{
					/* no room for one more record */
					haveToAllocate=1;
					continue;
				}
			/* insert into leaf - no need to split */

			//AM_insertToIntNode	
			AM_InsertToLeafFound(pageBuf,recNum,index,header);
			help("do not happen yet");	
			bcopy(header,pageBuf,AM_sl);
			haveToAllocate=0;
			i++;
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
			//AM_insertToIntNode	
    			
    			AM_InsertToLeafNotFound(pageBuf,&value,recNum,index,header);
				header->numKeys++;
    			bcopy(header,pageBuf,AM_sl);
				haveToAllocate=0;
				i++;
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
				
				printf("We think this should never happen\n");

				bcopy(tempPage,pageBuf,PF_PAGE_SIZE);
				bcopy(pageBuf,header,AM_sl);
				/* Insert into leaf a new key - no need to split */
				AM_InsertToLeafNotFound(pageBuf,&value,recNum,index,header);
				header->numKeys++;
				bcopy(header,pageBuf,AM_sl);
				haveToAllocate=0;
				recNum++;
				previousValue=value;
				continue;

			}
			else{ /* there is not enough room in the page */
				haveToAllocate=1;
				recNum++;
				index++;
				previousValue=value;
			}
		}

	}



}