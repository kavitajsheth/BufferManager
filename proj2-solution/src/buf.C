/*****************************************************************************/
/*************** Implementation of the Buffer Manager Layer ******************/
/*****************************************************************************/


#include "buf.h"
//#include "HashTable.h"

// Define buffer manager error messages here
//enum bufErrCodes  {...};

// Define error message here
static const char* bufErrMsgs[] = { 
  // error message strings go here
		"Error in MRU/LRU List insert",
		"Error in MRU/LRU List lookup"
};

// Create a static "error_string_table" object and register the error messages
// with minibase system 
static error_string_table bufTable(BUFMGR,bufErrMsgs);

BufMgr::BufMgr (int numbuf, Replacer *replacer) {
  // put your code here
	// allocating "numbuf" pages(frames) in the pool in main memory.
	numbuffer = numbuf;
 	bufPool = new Page[numbuf];
 	bufDescr = new Descriptor[numbuf];
 	FrameNumber=0;
 	for ( int i = 0; i < numbuffer; ++i)
 	{
		bufDescr[i].PageNumber = INVALID_PAGE;
		bufDescr[i].pin_count	= 0;
		bufDescr[i].dirtybit = FALSE;
 	}
 	//h = new HashTab();

}

Status BufMgr::pinPage(PageId PageId_in_a_DB, Page*& page, int emptyPage) {
  // put your code here
	int result = h.lookup(PageId_in_a_DB);
	// Page is not in buffer pool
	if(result==-1){
		int freefrm = -1;
		int frmno = -1;
		for ( int i = 0; i < numbuffer; ++i )
		{
			if ( (bufDescr[i].PageNumber == INVALID_PAGE) )
			{
			frmno = i;
			break;
			}
		}
		if ( (frmno != -1) && (frmno < numbuffer) )
		{
			freefrm = frmno;
		}else{
//				for ( int i = 0; i < numbuffer; ++i )
//					{
//						if ( (bufDescr[i].pin_count == 0) )
//						{
//						frmno = i;
//						break;
//						}
//					}
			Status st;
			st = blist.lookup(&freefrm);
				if(st!=OK){
						return MINIBASE_FIRST_ERROR(BUFMGR,LISTSEARCH_ERROR);

				}
				if ( (frmno != -1) && (frmno < numbuffer) )
					{
						freefrm = frmno;
					}
				Page *pageptr = &bufPool[freefrm];
				//Status st;
				if(bufDescr[freefrm].dirtybit){
					st = MINIBASE_DB->write_page(bufDescr[freefrm].PageNumber, pageptr);
				}
				if(st != OK){
					return FAIL;
				}
				if(bufDescr[freefrm].pin_count>0){
					return FAIL;
				}
				h.remove(bufDescr[freefrm].PageNumber);
		}

		page = &bufPool[freefrm];
		Status st;
		st = MINIBASE_DB->read_page(PageId_in_a_DB,page);
		if(st !=OK){
			return FAIL;
		}
		//bufPool[freefrm]=*page;
		h.insert(PageId_in_a_DB,freefrm);

		bufDescr[freefrm].PageNumber = PageId_in_a_DB;
		(bufDescr[freefrm].pin_count)++;
		bufDescr[freefrm].dirtybit = false;

	}else{ // Page is in buffer pool
		++bufDescr[result].pin_count;
		int pno = bufDescr[result].PageNumber;
		Page *pptr = &bufPool[result];
			if(bufDescr[result].dirtybit == true){
				MINIBASE_DB->write_page(pno, pptr);
			}
		bufDescr[result].dirtybit = 0;
		page = &bufPool[result];
	}
  return OK;
}//end pinPage


Status BufMgr::newPage(PageId& firstPageId, Page*& firstpage, int howmany) {
	Status st;
	st =MINIBASE_DB->allocate_page(firstPageId, howmany);
	if(st==OK){
		BufMgr::pinPage(firstPageId, firstpage, 0);
		return OK;
	}
	return FAIL;
}

Status BufMgr::flushPage(PageId pageid) {
  // put your code here
	Status st;
	int framein = h.lookup(pageid);
	if(framein !=-1){
		if(bufDescr[framein].dirtybit){
		st =MINIBASE_DB->write_page(pageid, (Page*)&bufPool[framein]);
			if(st==OK){
				return OK;
			}
		}
		return OK;
	}
  return FAIL;
}
    
	  
//*************************************************************
//** This is the implementation of ~BufMgr
//************************************************************
BufMgr::~BufMgr(){
  // put your code here for flush all dirty pages in the pool to disk before
	//deallocating buffer pool in main memory
//	bufPool = NULL;
//	free(bufPool);
	flushAllPages();
	delete[] bufPool;
	delete[] bufDescr;
}


//*************************************************************
//** This is the implementation of unpinPage
//************************************************************

Status BufMgr::unpinPage(PageId page_num, int dirty=FALSE, int hate = FALSE){
  // put your code here
	void *qptr;
	Status st;
	int framein = h.lookup(page_num);
	//cout << "framein :: "<< framein << endl;
	if(framein !=-1){
	// set dirty bit fro fram if its true
	if(dirty==TRUE){
		bufDescr[framein].dirtybit = dirty;
		st =MINIBASE_DB->write_page(page_num, &bufPool[framein]);
		if(st!=OK){
			return MINIBASE_CHAIN_ERROR(BUFMGR, st);
		}
	}
	// decrement pin count for frame if its greater than 0
	if(bufDescr[framein].pin_count>0){
		--(bufDescr[framein].pin_count);
	}else{
		return FAIL;
	}
	st = blist.insert(!hate, page_num, framein);
		if(st!=OK){
			MINIBASE_FIRST_ERROR(BUFMGR,LISTINSERT_ERROR);
		}return OK;
	}
	return FAIL;
}

//*************************************************************
//** This is the implementation of freePage
//************************************************************

Status BufMgr::freePage(PageId globalPageId){
	Status st;
	int framein = h.lookup(globalPageId);
	if(framein!=-1 && bufDescr[framein].pin_count==0){
		if ( bufDescr[framein].dirtybit ){
			st =MINIBASE_DB->write_page(globalPageId,(Page*) &bufDescr[framein]);
			if(st!=OK){
				return MINIBASE_CHAIN_ERROR(BUFMGR, st);
			}
		}
		bufDescr[framein].PageNumber = INVALID_PAGE;
		bufDescr[framein].pin_count = 0;
		bufDescr[framein].dirtybit = FALSE;

		h.remove(globalPageId);
		//blist.remove()

		st = MINIBASE_DB->deallocate_page(globalPageId,1);
		if(st!=OK){
			return MINIBASE_CHAIN_ERROR(BUFMGR, st);
		}
		return OK;
	}
		return FAIL;

}

Status BufMgr::flushAllPages(){
  //put your code here
	int i,framein; Status st;
	for(i=0;i<numbuffer;i++){
		framein = i;
		if(bufDescr[framein].dirtybit){
			st =MINIBASE_DB->write_page(bufDescr[framein].PageNumber, (Page*)&bufPool[framein]);
			if(st!=OK){
				return MINIBASE_CHAIN_ERROR(BUFMGR, st);
			}
		}
	}
  return OK;
}
