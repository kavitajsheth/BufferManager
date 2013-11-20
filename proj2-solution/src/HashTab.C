/*****************************************************************************/
/*************** Implementation of the Hash Table ******************/

/*****************************************************************************/
#include "buf.h"
//#include "HashTab.h"


HashTab::HashTab()
{
  size = HTSIZE;
  table = new Node*[size];
  for ( int i = 0; i < size; i++ ) {
    table[i] = NULL;
  }
}

HashTab::~HashTab()
{
   for(int i=0; i<size; i++)
   {
      Node* temp = table[i];
      while(temp != NULL)
      {
         Node* next = temp->next;
         delete temp;
         temp = next;
      }
   }
   size = 0;
   delete[] table;
}

void HashTab::insert(PageId PageNum,int FrameNum){
	int a = 5, b=10;
    int location = (a*PageNum+b) % size;
    Node* toInsert = new Node;
    toInsert->next= NULL;
    toInsert->PageNum = PageNum;
    toInsert->FrameNum = FrameNum;

    if(table[location]==NULL){
       table[location] = toInsert;
    }
    else{
        Node *runner = table[location];
        while(runner->next != NULL){
             runner = runner->next;
        }
        runner->next = toInsert;
    }
}
void HashTab::remove(PageId PageNum){
	int a = 5, b=10;
	int location = (a*PageNum+b) % size;
	if(table[location]!=NULL){
		Node *runner = table[location];
		if(runner->PageNum == PageNum){
			table[location] = runner->next;
			delete runner;
		}else{
			Node* prev = runner;
			runner=runner->next;
			while(runner->next != NULL){
				if(runner->PageNum == PageNum){
					prev->next = runner->next;
					delete runner;
					break;
				}
				prev = runner;
				runner = runner->next;
			}
			if(runner->PageNum == PageNum){
				prev->next=NULL;
				delete runner;
			}
		}
	}
}
int HashTab::lookup(PageId PageNum){
	int a = 5, b=10;
	int location = (a*PageNum+b) % size;
	if(table[location]==NULL){
//		cout << "BufferManager does not contain Page NUmber :: " << PageNum << endl;
		return -1;
	}else{
		Node *runner = table[location];
						do{
							if(runner->PageNum == PageNum){
//								cout << "Page Found , Frame Number :: " << runner->FrameNum;
								return runner->FrameNum;
							}
							runner = runner->next;
						}while(runner != NULL);
//						cout << "BufferManager does not contain Page NUmber :: " << PageNum << endl;
						return -1;
	}
}

void HashTab::print(){
	int i;
	for(i=0;i<size;i++){
		cout << "Bucket :: " << i+1 << " ";
		if(table[i]!= NULL){
			Node *runner = table[i];
				do{
					cout << "Page No :: "<< runner->PageNum << " && Frame No ::" << runner->FrameNum ;
					 runner = runner->next;
					 if(runner != NULL){
						 cout << " --> ";
					 }
				}while(runner != NULL);
				cout << endl;
		}else{
			cout << "Empty" << endl;
		}
	}
}
