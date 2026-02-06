#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

struct HEAP
{
    std::vector<int> data; // 1-based: data[0] unused
    int n;
    int capacity;
    HEAP(int cap = 32) : data(cap + 1, 0), n(0), capacity(cap) {}
};

typedef int ElemType;

Array1DTracer heapTracer = Array1DTracer("堆的数组形式");
LogTracer logger = LogTracer("运行日志");
GraphTracer heapGraph = GraphTracer("最大堆").directed(false);
bool HeapFull(HEAP &heap)
{
    return (heap.n == heap.capacity);
}

// 更新图形化的堆树视图（binary heap visual）
// 根据数组构建完全二叉树：对第 i 个节点（0-based），左孩子为 2*i+1，右孩子为 2*i+2
void updateHeapGraph(HEAP &heap)
{
    int n = heap.n;
    GraphTracer heapGraph_new = GraphTracer("最大堆").directed(false);
    heapGraph = heapGraph_new;
    Layout::setRoot(VerticalLayout({heapGraph, heapTracer, logger}));
    // 添加节点：使用连续的数值 id (0..n-1)，并把堆值作为 weight 显示
    for (int i = 0; i < n; ++i)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        heapGraph.addNode(heap.data[i]);
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
}

void DeleteMax(HEAP &heap)
{
    if (heap.n == 0)
    {
        logger.println("堆为空，无法删除元素");
        Tracer::delay();
        return;
    }
    int maxElem = heap.data[0];
    logger.println("删除最大元素: " + std::to_string(maxElem) + "，并替换堆顶元素");
    heapTracer.select(0);
    heapGraph.select(heap.data[0]);
    heapGraph.select(heap.data[heap.n-1]);
    Tracer::delay();

    heap.data[0] = heap.data[heap.n - 1];
    heap.n--;
    heapTracer.deselect(0);
    heapTracer.patch(0, heap.data[0]);
    heapTracer.patch(heap.n, "-");
    updateHeapGraph(heap);
    heapGraph.select(heap.data[0]);
    Tracer::delay();
    heapTracer.depatch(0);
    heapTracer.depatch(heap.n);
    heapGraph.deselect(heap.data[0]);
    Tracer::delay();
    logger.println("堆顶元素被最后一个元素替换，开始下滤过程");
    Tracer::delay();
    // 下滤过程
    int i = 0;
    while (true)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int largest = i;
        // 找到最大子结点
        if (left < heap.n && heap.data[left] > heap.data[largest])
            largest = left;
        if (right < heap.n && heap.data[right] > heap.data[largest])
            largest = right;

        if (largest != i)
        {
            // 可视化
            logger.println("交换元素 " + std::to_string(heap.data[i]) + " 和 " + std::to_string(heap.data[largest]));
            heapGraph.select(heap.data[largest]);
            heapGraph.select(heap.data[i]);
            heapTracer.select(i);
            heapTracer.select(largest);
            Tracer::delay();

            std::swap(heap.data[i], heap.data[largest]);
            // 可视化
            heapTracer.patch(i, heap.data[i]);
            heapTracer.patch(largest, heap.data[largest]);
            heapTracer.deselect(i);
            heapTracer.deselect(largest);
            updateHeapGraph(heap);
            heapGraph.select(heap.data[largest]);
            heapGraph.select(heap.data[i]);
            Tracer::delay();
            heapTracer.depatch(i);
            heapTracer.depatch(largest);
            heapGraph.deselect(heap.data[largest]);
            heapGraph.deselect(heap.data[i]);
            Tracer::delay();

            i = largest;
        }
        else
        {
            break;
        }
    }
    updateHeapGraph(heap);
    logger.println("删除完成，目前堆中元素个数：" + std::to_string(heap.n));
    Tracer::delay();
}

int main()
{
    Layout::setRoot(VerticalLayout({heapGraph, heapTracer, logger}));

    // 创建一个示例堆（max-heap）
    HEAP heap(10);
    // 初始元素：索引从0开始
    std::vector<int> init = {95, 87, 53, 45, 78, 9, 31, 17};
    // 初始化堆内容
    heap.n = (int)init.size();
    for (int i = 0; i < heap.n; ++i)
        heap.data[i] = init[i];

    // 可视化初始堆数组
    json arr = json::array();
    for (int i = 0; i < heap.capacity; ++i)
    {
        if (i < heap.n)
            arr.push_back(heap.data[i]);
        else
            arr.push_back(std::string("-"));
    }
    heapTracer.set(arr);

    for (int i = 0; i < heap.n; ++i)
    {
        heapGraph.addNode(heap.data[i]);
    }
    for (int i = 0; i < heap.n; ++i)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        if (left < heap.n)
        {
            heapGraph.addEdge(heap.data[i], heap.data[left]);
            heapGraph.addEdge(heap.data[left], heap.data[i]);
        }
        if (right < heap.n)
        {
            heapGraph.addEdge(heap.data[i], heap.data[right]);
            heapGraph.addEdge(heap.data[right], heap.data[i]);
        }
    }
    heapGraph.layoutTree(heap.data[0]);
    Tracer::delay();
    logger.println("试图删除最大元素");
    Tracer::delay();
    DeleteMax(heap);
    logger.println("试图删除最大元素");
    Tracer::delay();
    DeleteMax(heap);

    return 0;
}
