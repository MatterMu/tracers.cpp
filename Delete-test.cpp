#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

struct HEAP
{
    std::vector<int> data; // 0-based: data[0] is root
    int n; // number of elements
    int capacity;
    HEAP(int cap = 32) : data(cap, 0), n(0), capacity(cap) {}
};

typedef int ElemType;

Array1DTracer heapTracer = Array1DTracer("堆的数组形式");
LogTracer logger = LogTracer("运行日志");
GraphTracer heapGraph = GraphTracer("最大堆").directed(false);

bool HeapFull(HEAP &heap)
{
    return (heap.n >= heap.capacity);
}

// 更新图形化的堆树视图（binary heap visual）
// 使用连续的数值 id (0..n-1)，并把堆值作为 weight 显示
void updateHeapGraph(HEAP &heap)
{
    int n = heap.n;
    GraphTracer heapGraph_new = GraphTracer("最大堆").directed(false);
    heapGraph = heapGraph_new;
    Layout::setRoot(VerticalLayout({heapGraph, heapTracer, logger}));
    if (n <= 0) return;
    for (int i = 0; i < n; ++i)
    {
        heapGraph.addNode(heap.data[i]);
    }
    for (int i = 0; i < n; ++i)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < n)
        {
            heapGraph.addEdge(heap.data[i], heap.data[left]);
            heapGraph.addEdge(heap.data[left], heap.data[i]);
        }
        if (right < n)
        {
            heapGraph.addEdge(heap.data[i], heap.data[right]);
            heapGraph.addEdge(heap.data[right], heap.data[i]);
        }
    }
    heapGraph.layoutTree(heap.data[0]);
    Tracer::delay();
}

// DeleteMax (0-based indices) following image logic: parent=0, child=1
void DeleteMax(HEAP &heap)
{
    if (heap.n == 0)
    {
        logger.println("堆为空，无法删除元素");
        Tracer::delay();
        return;
    }

    ElemType elem = heap.data[0];
    ElemType tmp = heap.data[heap.n - 1];
    // 可视化
    logger.println("删除最大元素: " + std::to_string(elem) + "，并替换堆顶元素");
    heapTracer.select(0);
    heapGraph.select(heap.data[0]);
    // heapGraph.select(heap.data[heap.n - 1]);
    Tracer::delay();

    heap.n = heap.n - 1;

    // put last at root (will percolate down)
    heap.data[0] = tmp;
    heapTracer.deselect(0);
    heapTracer.patch(0, heap.data[0]);
    heapTracer.patch(heap.n, std::string("-"));
    updateHeapGraph(heap);
    heapGraph.select(heap.data[0]);
    Tracer::delay();
    
    
    heapTracer.depatch(0);
    heapTracer.depatch(heap.n);
    heapGraph.deselect(heap.data[0]);
    Tracer::delay();

    logger.println("堆顶元素被最后一个元素替换，开始下滤过程");
    Tracer::delay();

    int parent = 0;
    int child = 1; // left child of parent=0

    while (child <= heap.n - 1)
    {
        // 找到更大的子结点
        if (child < heap.n - 1 && heap.data[child] < heap.data[child + 1])
            child = child + 1;

        // 可视化
        heapGraph.select(heap.data[child]);
        heapTracer.select(child);
        Tracer::delay();

        if (tmp >= heap.data[child])
        {
            heapGraph.deselect(heap.data[child]);
            heapTracer.deselect(child);
            break;
        }

        // move child up
        heap.data[parent] = heap.data[child];
        heapTracer.patch(parent, heap.data[parent]);
        logger.println("下滤：将 " + std::to_string(heap.data[child]) + " 移到索引 " + std::to_string(parent));
        Tracer::delay();
        
        heapTracer.depatch(parent);
        heapGraph.deselect(child);
        heapTracer.deselect(child);

        // move down the tree
        parent = child;
        child = 2 * parent + 1;
    }

    // place tmp at its position
    heap.data[parent] = tmp;
    heapTracer.patch(parent, heap.data[parent]);
    Tracer::delay();

    // update graph and array visualization
    updateHeapGraph(heap);
    logger.println("删除完成: " + std::to_string(elem) + "，当前堆大小: " + std::to_string(heap.n));
    Tracer::delay();
}

int main()
{
    Layout::setRoot(VerticalLayout({heapGraph, heapTracer, logger}));

    HEAP heap(10);
    std::vector<int> init = {95, 87, 78, 65, 53, 31, 9, 45, 17, 23};
    heap.n = (int)init.size();
    for (int i = 0; i < heap.n; ++i) heap.data[i] = init[i];

    // array tracer (index 0..capacity-1)
    json arr = json::array();
    for (int i = 0; i < heap.capacity; ++i)
    {
        if (i < heap.n) arr.push_back(heap.data[i]);
        else arr.push_back(std::string("-"));
    }
    heapTracer.set(arr);

    updateHeapGraph(heap);
    heapGraph.layoutTree(heap.data[0]);

    Tracer::delay();

    logger.println("进行一次 DeleteMax"); Tracer::delay();
    DeleteMax(heap);

    logger.println("再执行一次 DeleteMax"); Tracer::delay();
    DeleteMax(heap);

    logger.println("结束"); Tracer::delay();

    return 0;
}
