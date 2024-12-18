#ifndef CUTE_HEAP_H
#define CUTE_HEAP_H
#include "list.h"
#define HEAP_SIZE 2048
typedef struct HeapNode_t
{
    Node node;      // 内存节点
    uint8_t isUsed; // 节点是否被使用
    uint32_t size;  // 大小
} HeapNode;
#define NODE_SIZE (sizeof(HeapNode))
// 堆内存申请
uint8_t* heap_malloc(uint32_t wantSize);
// 内存释放
void heap_free(uint8_t *ptr);
#endif
