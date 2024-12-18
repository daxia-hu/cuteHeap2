#include "list.h"
// 链表初始化
void list_init(List *l)
{
    l->pHead.pPrev = NULL;
    l->pTail.pNext = NULL;
    l->pHead.pNext = &(l->pTail);
    l->pTail.pPrev = &(l->pHead);
}
// 在当前节点后面插入节点
void list_insert(Node *c,Node *n)
{
    c->pNext->pPrev = n;
    n->pNext = c->pNext;
    n->pPrev = c;
    c->pNext = n;
}
// 删除节点
void list_delete(Node *n)
{
    n->pNext->pPrev = n->pPrev;
    n->pPrev->pNext = n->pNext;
}
