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

void Insert(HEAP &heap, ElemType elem)
{
    int i;

    std::vector<int> visitidx;
    if (!HeapFull(heap))
    {
        i = heap.n;
        int parentidx = (i - 1) / 2;
        heap.n++;
        heapGraph.addNode(elem);
        heapGraph.addEdge(heap.data[parentidx], elem);
        heapGraph.addEdge(elem, heap.data[parentidx]);
        heapTracer.patch(i, elem); // 先放在最后
        heapGraph.visit(elem);
        Tracer::delay();
        logger.println("将元素 " + std::to_string(elem) + " 放在堆的末尾位置，开始上浮");
        Tracer::delay();
        heapGraph.leave(elem);
        heapTracer.depatch(i);
        heapGraph.select(heap.data[parentidx]);
        heapTracer.select(parentidx);
        Tracer::delay();
        heapTracer.depatch(i);
        while ((i != 0) && (elem > heap.data[parentidx]))
        {
            logger.println("堆中元素 " + std::to_string(heap.data[parentidx]) + " 小于插入元素 " + std::to_string(elem) + "，进行下推");
            heap.data[i] = heap.data[parentidx]; // 下推
            heap.data[parentidx] = elem;         // 上浮
            // 可视化：修改堆树结构
            updateHeapGraph(heap);
            heapGraph.visit(elem);
            heapGraph.visit(heap.data[i]);
            // 可视化：修改堆数组内容
            heapTracer.patch(parentidx, elem);
            heapTracer.patch(i, heap.data[i]);
            heapTracer.deselect(parentidx);
            Tracer::delay();
            // visitidx.push_back(i);

            heapTracer.depatch(i);
            heapTracer.depatch(parentidx);
            heapGraph.leave(elem);
            heapGraph.leave(heap.data[i]);
            Tracer::delay();
            i = parentidx;
            parentidx = (parentidx - 1) / 2;
            if (heap.data[parentidx] != elem)
            {
                heapGraph.select(heap.data[parentidx]);
                heapTracer.select(parentidx);
                Tracer::delay();
            }
        }

        // heapTracer.patch(i, heap.data[i]);
        // visitidx.push_back(i);
        if (i == 0)
        {
            logger.println("已到达堆顶位置，元素 " + std::to_string(elem) + " 上浮完毕");
            heapTracer.deselect(0);
        }
        else
        {
            logger.println("元素 " + std::to_string(heap.data[parentidx]) + " 大于插入元素 " + std::to_string(elem) + "，元素上浮完毕");
            heapTracer.deselect(parentidx);
            heapGraph.deselect(heap.data[parentidx]);
            // heapGraph.visit(heap.data[parentidx]);
        }
        Tracer::delay();

        logger.println("成功插入元素 " + std::to_string(elem) + "，目前堆中元素个数：" + std::to_string(heap.n));
        Tracer::delay();
        return;
    }
    logger.println("堆已满, 无法插入元素: " + std::to_string(elem));
    Tracer::delay();
    std::cout << "堆已满, 无法插入元素: " << elem << std::endl;
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

    logger.println("试图插入元素88");
    Tracer::delay();
    Insert(heap, 88);
    logger.println("试图插入元素100");
    Tracer::delay();
    Insert(heap, 100);
    logger.println("试图插入元素42");
    Tracer::delay();
    Insert(heap, 42);

    return 0;
}
