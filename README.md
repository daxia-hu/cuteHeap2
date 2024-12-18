<h2 id="UlCam">一、说明</h2>
> <font style="color:#000000;">动态内存管理指的是在程序需要内存时申请，在使用完成后释放，保证以比较少的内存实现更多的软件需求。与之相对的是静态内存，典型的静态内存就是全局变量：在程序启动时到整个生命空间内都占用空间。动态内存的特点就是实现内存的“按需使用”，在嵌入式开发环境中，节省ram实现更多更灵活的内存使用需求。c库中的malloc、free接口就是动态内存申请、释放的实现。在《深入理解计算机系统》书中针对c库中malloc的实现算法有一章的篇幅进行说明。</font>
>

V2是因为几年前写过一版动态内存管理，整体思路是：

使用链表管理已经申请的内存，申请时检查容器中未被申请的空间，在这个空间内创建节点放在链表上。内存释放时将节点中的内存释放出来并删除节点。这个管理方案中申请内存没有考虑最优方案，会产生过多的内存碎片导致空间利用率太低

[代码仓库](https://gitee.com/hu_daxia/cuteheap)

[hudaxia/cuteheap](https://gitee.com/hu_daxia/cuteheap)

<h2 id="pmf6X">二、软件设计</h2>
<h3 id="PSGcD">2.1 改动项目</h3>
新版本软件同样采用一个双向链表实现，相比于之前的链表只存放已申请节点增加了未申请节点，这样在内存申请已经释放合并时更加灵活。同时在申请处理中增加了查找最优申请节点的处理，极大的减少了内存碎片增加空间利用率。

链表节点包含三个属性：用于挂在链表上的节点，是否被申请属性，大小属性

![节点类图](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734427880231-171d61a3-64eb-4174-9218-70c21751c346.png)

<h3 id="kLWw8">2.2 场景举例</h3>
> 可视化举例说明内存申请释放，使用图片的形式说明内存在申请、释放时链表的动态变化
>

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734428506011-f7fccaba-4a35-4ef7-90ed-b9dfb2ebcfb4.png) 

1. 动态内存初始化完成

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734428485886-b4a8c1d2-cd9e-4dce-b04b-8047884aa9d4.png)

2. 申请1、2、3三个节点

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734430195332-1dc74ab3-5e98-4f18-8ea7-397c11c0766b.png)

3. 释放掉上述的2号节点

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734430286720-2dfb8933-e075-4404-bc48-1264ff9b629b.png)

4. 申请一块比2空间大的节点

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734430393353-02557a02-d5d9-418b-a217-a655276d2c53.png)

5. 释放上述的3号节点，2号与3号合并

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734430595042-4d8fe228-344f-44ba-9470-57a6ec15190b.png)

6. 申请一个小于2号节点的节点

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734487540822-64cb2155-d12c-43a4-9c05-4243d4c94c02.png)

整体场景：

![](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734487586907-deb597a2-76b0-4ecd-8ba1-05a9d11ea175.png)

![代码实践结果](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734487914802-54af82da-3736-45da-874d-834a66ee36da.png)

<h3 id="zETMS">2.3 内存申请软件流程</h3>
![内存申请流程图](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734433539570-5a5dafbc-975c-44cc-86ed-7cfec844ec0e.png)

<h3 id="gBOYl">2.4 内存释放软件流程</h3>
![内存释放流程](https://cdn.nlark.com/yuque/0/2024/png/1561832/1734434148504-9217ddf4-e7dc-49ba-85fe-6abea6554350.png)

<h2 id="ad6sQ">三、软件实现</h2>
[代码仓库](https://github.com/daxia-hu/cuteHeap2)

申请部分：

```c
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
        heapInit = 1;、
        heap_init();
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
```

释放部分：

```c
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
```

实测用例：

```c
int main()
{
    heap_init();
    heap_foreach();
    uint8_t *p1 = heap_malloc(50);
    uint8_t *p2 = heap_malloc(20);
    uint8_t *p3 = heap_malloc(20);
    heap_foreach();
    heap_free(p2);
    heap_foreach();
    uint8_t *p4 = heap_malloc(70);
    heap_foreach();
    heap_free(p3);
    heap_foreach();
    uint8_t *p5 = heap_malloc(30);
    heap_foreach();
}
```

运行效果：

![](https://cdn.nlark.com/yuque/0/2024/gif/1561832/1734488493554-22a1d7f1-5451-44ae-98ad-4e77a29eaf5b.gif)

<h2 id="EPKVr">四、待改善</h2>
1. 申请的和释放的时间不确定
2. 节点本身占用空间，节点太多导致可使用空间变少



