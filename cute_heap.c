#include "cute_heap.h"
#include "stdio.h"
uint8_t gHeapSpace[HEAP_SIZE];
List heapList;
void heap_foreach(void);
// 初始化
void heap_init(void)
{
    list_init(&heapList);
    HeapNode *n = NULL;
    n = (HeapNode *)gHeapSpace;
    n->isUsed = 0;
    n->size = (uint32_t)(HEAP_SIZE - NODE_SIZE);
    list_insert(&(heapList.pHead), (Node *)(n));
}
// 堆内存申请
uint8_t *heap_malloc(uint32_t wantSize)
{
    static uint8_t heapInit = 0;
    uint8_t nodeState;
    uint32_t currentSize;
    uint32_t tempSize = HEAP_SIZE - NODE_SIZE;
    HeapNode *nNode = NULL;
    Node *iNode = NULL;
    if (!heapInit)
    {
        heapInit = 1;
    }
    if (wantSize > HEAP_SIZE - NODE_SIZE)
    {
        return NULL;
    }
    for (iNode = heapList.pHead.pNext; NULL != iNode->pNext; iNode = iNode->pNext)
    {
        nodeState = ((HeapNode *)iNode)->isUsed;
        if (!nodeState)
        {
            currentSize = ((HeapNode *)iNode)->size;
            if (currentSize < wantSize)
            {
                continue;
            }
            else if (currentSize > wantSize + NODE_SIZE)
            {
                if (currentSize <= tempSize)
                {
                    tempSize = currentSize;
                    nNode = (HeapNode *)iNode;
                }
            }
            else 
            {
                nNode = (HeapNode *)iNode;
                nNode->isUsed = 1;
                return (uint8_t *)((uint8_t *)nNode + NODE_SIZE);
            }
        }
    }
    if (NULL != nNode)
    {
        HeapNode *tNode = (HeapNode *)((uint8_t *)nNode + NODE_SIZE + wantSize);
        tNode->isUsed = 0;
        tNode->size = nNode->size - wantSize - NODE_SIZE;
        list_insert((Node *)nNode,(Node *)tNode);
        nNode->size = wantSize;
        nNode->isUsed = 1;
        return (uint8_t *)((uint8_t *)nNode + NODE_SIZE);
    }
    else
    {
        return NULL;
    }
}
// 内存释放
void heap_free(uint8_t *ptr)
{
    if ((ptr > (uint8_t *)(gHeapSpace + HEAP_SIZE - NODE_SIZE)) ||
        (ptr < (uint8_t *)gHeapSpace))
    {
        return;
    }
    Node *iNode = heapList.pHead.pNext;
    HeapNode *nNode = NULL;
    for (; NULL != iNode->pNext; iNode = iNode->pNext)
    {
        if ((ptr - NODE_SIZE) == (uint8_t *)iNode)
        {
            nNode = (HeapNode *)iNode;
            nNode->isUsed = 0;
            break;
        }
    }
    if (nNode != NULL)
    {
        if (0 == ((HeapNode *)(iNode->pNext))->isUsed)
        {
            nNode->size = nNode->size + NODE_SIZE + ((HeapNode *)(iNode->pNext))->size;
            list_delete(iNode->pNext);
        }
        if (0 == ((HeapNode *)(iNode->pPrev))->isUsed)
        {
            HeapNode *tNode = (HeapNode *)(iNode->pPrev);
            tNode->size = tNode->size + NODE_SIZE + nNode->size;
            list_delete(iNode);
        }
    }
}
void heap_foreach(void)
{
    Node *iNode = NULL;
    HeapNode *nNode = NULL;
    static int xx = 0;
    printf("--------------%d---------------\r\n",++xx);
    int index = 0;
    for (iNode = heapList.pHead.pNext; NULL != iNode->pNext; iNode = iNode->pNext)
    {
        HeapNode *nNode = (HeapNode *)iNode;
        printf("->%d:%04d,%04d,%02d",++index,(uint8_t *)nNode-gHeapSpace,nNode->size,nNode->isUsed);
        printf("\r\n");
    }
}
