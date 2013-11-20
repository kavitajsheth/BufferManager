#ifndef HASHTAB_H
#define HASHTAB_H


class Node {
public:
  PageId PageNum;
  int FrameNum;
  Node * next;
};

class HashTab {
private:
  int size;
  Node ** table;
public:
  HashTab();
  ~HashTab();
  void insert(PageId PageNum,int FrameNum);

  void remove(PageId PageNum);
  int lookup(PageId PageNum);
  void print(void);
};
#endif
