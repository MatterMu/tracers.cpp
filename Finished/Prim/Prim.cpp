#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>

using json = nlohmann::json;
int INF = std::numeric_limits<int>::max();
// Prim 算法：在单独函数中实现，保留变量命名和算法逻辑
// G: 邻接矩阵（0 表示无边），start: 起始结点，默认为 0
int Prim(std::vector<std::vector<int>> &C, int start,
         GraphTracer &tracer, LogTracer &logger, Array2DTracer &arraytracer)
{
    int n = (int)C.size();
    std::vector<int> CLOSEST(n, 0); // 记录U中的顶点
    std::vector<int> LOWCOST(n, 0); // 最小边权值
    CLOSEST[start] = 0;
    tracer.visit(start); // 标记起始结点
    logger.println("下面根据起始节点初始化 LOWCOST 和 CLOSEST 数组，第一行为CLOSEST数组，第二行为LOWCOST数组");
    Tracer::delay();
    arraytracer.patch(2, start+1, 0);
    arraytracer.patch(1, start+1, 0);
    Tracer::delay();
    arraytracer.depatch(2, start+1);
    arraytracer.depatch(1, start+1);
    Tracer::delay();

    int i = 0, j = 0, k = 0;
    for (i = 1; i < n; i++)
    {
        if (C[start][i] == 0)
        {
            LOWCOST[i] = INF;
            CLOSEST[i] = -1; // 表示无连接
            arraytracer.patch(2, i+1, "INF");
            arraytracer.patch(1, i+1, "-");
            Tracer::delay();
            arraytracer.depatch(2, i+1);
            arraytracer.depatch(1, i+1);
            Tracer::delay();
            continue;
        }
        tracer.visit(start, i);
        CLOSEST[i] = start;
        LOWCOST[i] = C[start][i];
        arraytracer.patch(2, i+1, LOWCOST[i]);
        arraytracer.patch(1, i+1, CLOSEST[i]);
        Tracer::delay();
        tracer.leave(start, i);
        arraytracer.depatch(2, i+1);
        arraytracer.depatch(1, i+1);
        Tracer::delay();
    }
    LOWCOST[start] = 0; // 标记加入最小生成树

    logger.println("下面开始执行 Prim 算法，遍历LOWCOST和CLOSEST数组以选择最小边");
    Tracer::delay();
    int sum = 0;
    for (i = 1; i < n; i++)
    {
        int minweight = INF;
        k = i;
        // 遍历已在树中的结点和未加入的结点寻找最小边
        for (j = 1; j < n; j++)
        {
            if (LOWCOST[j] == INF || LOWCOST[j] == 0)
                continue;
            std::cout << "(CLOSEST[j], j) : (" << CLOSEST[j] << "," << j << ") 权值=" << LOWCOST[j] << std::endl;
            tracer.visit(j, CLOSEST[j]);
            arraytracer.select(2, j+1);
            arraytracer.select(1, j+1);
            Tracer::delay();
            if (LOWCOST[j] < minweight)
            {
                minweight = LOWCOST[j];
                k = j;
            }
            tracer.leave(j, CLOSEST[j]);
            arraytracer.deselect(2, j+1);
            arraytracer.deselect(1, j+1);
            Tracer::delay();
        }

        tracer.visit(k, CLOSEST[k]);
        arraytracer.select(2, k+1);
        arraytracer.select(1, k+1);
        Tracer::delay();
        arraytracer.deselect(2, k+1);
        arraytracer.deselect(1, k+1);
        tracer.leave(k, CLOSEST[k]);
        Tracer::delay();
        tracer.visit(k, CLOSEST[k]);
        arraytracer.select(2, k+1);
        arraytracer.select(1, k+1);
        logger.println(std::string("选中边: (") + std::to_string(CLOSEST[k]) + "," + std::to_string(k) + ") 权值=" + std::to_string(C[k][CLOSEST[k]]));
        std::cout << "(" << CLOSEST[k] << "," << k << ") 权值=" << LOWCOST[k] << std::endl;
        Tracer::delay();

        sum += C[k][CLOSEST[k]];
        // 标记加入最小生成树
        C[k][CLOSEST[k]] = 0;
        C[CLOSEST[k]][k] = 0;
        LOWCOST[k] = 0;
        logger.println("下面根据新加入的结点更新LOWCOST和CLOSEST数组");
        Tracer::delay();
        arraytracer.deselect(2, k+1);
        arraytracer.deselect(1, k+1);
        arraytracer.patch(1, k+1, CLOSEST[k]);
        arraytracer.patch(2, k+1, 0);
        Tracer::delay();
        arraytracer.depatch(1, k+1);
        arraytracer.depatch(2, k+1);
        Tracer::delay();
        for (j = 1; j < n; j++)
        {
            if (C[k][j] != 0 && C[k][j] < LOWCOST[j])
            {
                LOWCOST[j] = C[k][j];
                CLOSEST[j] = k;
                tracer.visit(k, j);
                arraytracer.patch(1, j+1, CLOSEST[j]);
                arraytracer.patch(2, j+1, LOWCOST[j]);
                Tracer::delay();
                std::cout << "更新结点 " << j << " 的最小边为 (" << k << "," << j << ") 权值=" << C[k][j] << std::endl;
                tracer.leave(k, j);
                arraytracer.depatch(1, j+1);
                arraytracer.depatch(2, j+1);
                Tracer::delay();
            }
        }
        logger.println("数组更新完毕，继续寻找当前距离最小的边");
        Tracer::delay();
    }

    logger.println(std::string("算法执行完毕，生成树中的边权值之和: ") + std::to_string(sum));
    Tracer::delay();
    return sum;
}

int main()
{
    // 使用固定的无向加权图作为测试样例（对称矩阵），规模为 8 个顶点
    // 节点: 0 1 2 3 4 5 6 7
    std::vector<std::vector<int>> C = {
        {0, 2, 0, 6, 0, 0, 3, 0},
        {2, 0, 3, 8, 5, 0, 0, 0},
        {0, 3, 0, 0, 7, 4, 0, 0},
        {6, 8, 0, 0, 9, 0, 0, 1},
        {0, 5, 7, 9, 0, 2, 0, 0},
        {0, 0, 4, 0, 2, 0, 6, 0},
        {3, 0, 0, 0, 0, 6, 0, 5},
        {0, 0, 0, 1, 0, 0, 5, 0}};

    // Tracers
    GraphTracer tracer = GraphTracer("Prim算法").directed(false).weighted();
    LogTracer logger = LogTracer("运行日志");
    Array2DTracer arraytracer = Array2DTracer("LOWCOST+CLOSEST");

    // 将 tracers 放到垂直布局
    Layout::setRoot(VerticalLayout({tracer, arraytracer, logger}));

    // 设置图到 tracer，并使用圆形布局以避免 tree 布局把节点排列在一条直线上
    json Cjson = json::array();
    for (auto &row : C)
    {
        json r = json::array();
        for (int v : row)
            r.push_back(v);
        Cjson.push_back(r);
    }
    tracer.set(Cjson);
    // 使用圆形布局让节点均匀分布
    tracer.layoutCircle();
    Tracer::delay();

    // 初始化 arraytracer 显示为 "-" / INF / values
    int n = (int)C.size();
    json row_index = json::array();
    row_index.push_back("节点值");
    json row0 = json::array(), row1 = json::array();
    row0.push_back("CLOSEST数组");
    row1.push_back("LOWCOST数组");
    for (int i = 0; i < n; ++i)
    {
        row_index.push_back(std::to_string(i));
        row0.push_back(std::string("-"));
        row1.push_back(std::string("-"));
    }
    json arr = json::array();
    arr.push_back(row_index);
    arr.push_back(row0);
    arr.push_back(row1);
    arraytracer.set(arr);
    Tracer::delay();

    // 显示初始信息
    logger.println("开始运行 Prim 算法，起始点 0");
    Tracer::delay();

    int total = Prim(C, 0, tracer, logger, arraytracer);

    logger.println(std::string("Prim 算法执行完毕，最小生成树总权值=") + std::to_string(total));

    // 同时把找到的最小生成树的边（在可视化中也已经被打印）再次输出为总结
    logger.println("最小生成树边列表已输出（每行格式: (u,v) 权值）");

    return 0;
}
