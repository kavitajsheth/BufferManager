#include "buf.h"

BufList::BufList()
{
	ll = NULL;
	hl = NULL;
}

BufList::~BufList()
{
	ll = NULL;
	hl = NULL;
}

Status BufList::insert(int flag,PageId pageno, int frameno)
{
	// flag = 0 means Hate List
	// flag = 1 means Love List
	if(flag == 0){
		//while(hl!=NULL){
			//hl = hl->next;
		//}
		hateList *tmp = new hateList;
		tmp->pageNo = pageno;
		tmp->frameNo = frameno;


		if(hl == NULL){
			hl = tmp;
			hl->prev = hl;
			hl->next = hl;
			//*lptr = hl;
			return OK;
		}
		tmp->next = hl;
		tmp->prev = hl->prev;
		hl->prev->next = tmp;
		hl->prev = tmp;


		// new head
		hl = tmp;
//		*lptr = hl;
		return OK;
	}else{

				loveList *tmp = new loveList;
				tmp->pageNo = pageno;
				tmp->frameNo = frameno;

				if(ll == NULL){
					ll = tmp;
					ll->prev = ll;
					ll->next = ll;
		//			*lptr = ll;
					return OK;
				}
				tmp->next = ll;
				tmp->prev = ll->prev;
				ll->prev->next = tmp;
				ll->prev = tmp;

				// new head
				ll = tmp;
	//			*lptr = ll;
				return OK;
	}
	return FAIL;
}
int BufList::remove(int flag, void **lptr){
	if(flag == 0){
        hateList *tmp;
        tmp = (hateList*)(*lptr);
        if ( hl == tmp )
             hl = NULL;

		if ( tmp->prev->next == tmp )
		{
		 delete tmp;
		 return 1;
		}

		if ( tmp == NULL )
		 return 0;

		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp;

		delete tmp;
	    return 1;
	}else{
		loveList *tmp;
		tmp = (loveList*)(*lptr);
		if ( ll == tmp )
			 ll = NULL;

		if ( tmp->prev->next == tmp )
		{
		 delete tmp;
		 return 1;
		}

		if ( tmp == NULL )
		 return 0;

		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp;

		delete tmp;
		return 1;
	}
	return 0;
}
Status BufList::lookup(int *freeframe){
	int freefrm;
	// check hatelist first
	if(hl!=NULL){
		freefrm = hl->frameNo;
		hateList *tmp = hl;
		hl = tmp->next;
		if ( hl == tmp )
		   hl = NULL;

	    // Update only if we have new head.
		if ( hl )
        {
			// removing first node
		   tmp->prev->next = hl;
		   hl->prev = tmp->prev;
	    }

	  delete tmp;
	  *freeframe = freefrm;
	  return OK;
	}else{
		if ( ll == NULL )
		    return FAIL;

		loveList *rear = ll->prev;
		freefrm = rear->frameNo;

		if ( rear == ll )
		{
				delete rear;
				ll = NULL;
				*freeframe = freefrm;
				return OK;
		}
// removing last node
		rear->prev->next = rear->next;
		rear->next->prev = rear->prev;

		delete rear;
		*freeframe = freefrm;
		return OK;
	}
return FAIL;
}
void BufList::print(void){
	hateList *tmp = new hateList;
	tmp = hl;
	if ( tmp )
			hl = hl->next;
	while ( hl != tmp )
	{
			cout << hl->pageNo << endl;
			hl = hl->next;
	}
}
