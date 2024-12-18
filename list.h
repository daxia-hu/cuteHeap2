#ifndef LIST_H
#define LIST_H
#include "stdint.h"
typedef struct Node_t Node;
typedef struct Node_t
{
    Node *pPrev;
    Node *pNext;
}Node;
typedef struct List_t
{
    Node pHead;
    Node pTail;
}List;
// 链表初始化
void list_init(List *l);
// 在当前节点后面插入节点
void list_insert(Node *c,Node *n);
// 删除节点
void list_delete(Node *n);
#endif
