#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

// 单独实现 PreOrder 函数（中序：左 - 根 - 右）
void PreOrder(int root, int parent,
             const std::vector<std::array<int,2>>& T,
             GraphTracer &treeTracer, Array1DTracer &arrayTracer, LogTracer &logger,
             int &index)
{
    if (root == -1) {
        logger.println("该结点不存在，回溯中...");
        Tracer::delay();
        return;
    }

    // 输出访问信息
    logger.println(std::string("到达结点：") + std::to_string(root));
    treeTracer.visit(root, parent);
    Tracer::delay();

    logger.println(std::string("打印结点：") + std::to_string(root));
    treeTracer.leave(root, parent);
    arrayTracer.patch(index++, root);
    Tracer::delay();

    // 访问左子树
    logger.println(std::string("访问 ") + std::to_string(root) + " 的左子树");
    Tracer::delay();
    PreOrder(T[root][0], root, T, treeTracer, arrayTracer, logger, index);

    // 访问右子树
    logger.println(std::string("访问 ") + std::to_string(root) + " 的右子树");
    Tracer::delay();
    PreOrder(T[root][1], root, T, treeTracer, arrayTracer, logger, index);
}

int main()
{
    // 邻接矩阵表示图 G（用于展示）
    std::vector<std::vector<int>> G = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {1,0,1,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,0,0,0,0,0,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    // T[i] = {left, right}
    std::vector<std::array<int,2>> T = {
        std::array<int,2>{-1,-1},
        std::array<int,2>{0,2},
        std::array<int,2>{-1,-1},
        std::array<int,2>{1,4},
        std::array<int,2>{-1,-1},
        std::array<int,2>{3,9},
        std::array<int,2>{-1,-1},
        std::array<int,2>{6,8},
        std::array<int,2>{-1,-1},
        std::array<int,2>{7,11},
        std::array<int,2>{-1,-1},
        std::array<int,2>{10,12},
        std::array<int,2>{-1,-1}
    };

    // Tracers
    GraphTracer treeTracer = GraphTracer("先序遍历");
    Array1DTracer arrayTracer = Array1DTracer("先序遍历结果打印");
    LogTracer logger = LogTracer("运行日志");

    Layout::setRoot(VerticalLayout({treeTracer, arrayTracer, logger}));

    // convert G to json array2d
    json Gjson = json::array();
    for (auto &row : G) {
        json r = json::array();
        for (int v : row) r.push_back(v);
        Gjson.push_back(r);
    }
    treeTracer.set(Gjson);
    treeTracer.layoutTree(5);

    // initialize arrayTracer with '-' entries
    int n = (int)T.size();
    std::vector<json> adisp(n);
    for (int i = 0; i < n; ++i) adisp[i] = "-";
    arrayTracer.set(adisp);
    Tracer::delay();

    int index = 0;

    // start traversal from node 5
    PreOrder(5, -1, T, treeTracer, arrayTracer, logger, index);

    logger.println("先序遍历结束");

    return 0;
}
