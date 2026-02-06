#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

// 递归实现的 DFS（与图片中的 DFS2 思路一致）
// G: 邻接矩阵（0/1），i: 当前访问顶点，parent: 父节点（-1 表示无）
// dfn: 深度优先编号数组，visited: 访问标记，cnt: 编号计数器（引用传递）
// graphTracer, tableTracer, logger 用于可视化
void DFSRec(const std::vector<std::vector<int>> &G, int i, int parent,
            std::vector<int> &dfn, std::vector<bool> &visited, int &cnt,
            GraphTracer &graphTracer, Array2DTracer &tableTracer, LogTracer &logger)
{
    // 访问顶点 i
    visited[i] = true;
    dfn[i] = ++cnt;

    // 可视化：标注节点（高亮访问）并在表格中填写编号
    if (parent >= 0) {
        graphTracer.visit(i, parent);
    } else {
        graphTracer.visit(i);
    }
    // 在 tableTracer 第二行（索引 1）第 i+1 列写入 dfn 值（第一列为表头）
    tableTracer.patch(1, i + 1, dfn[i]);
    Tracer::delay();
    tableTracer.depatch(1, i + 1);
    Tracer::delay();

    // 遍历邻接顶点
    for (int j = 0; j < (int)G.size(); ++j) {
        if (G[i][j] && !visited[j]) {
            // 可视化当前遍历的边
            graphTracer.visit(i, j);
            Tracer::delay();
            DFSRec(G, j, i, dfn, visited, cnt, graphTracer, tableTracer, logger);
        }
    }
}

// 对外的 DFS 函数：按照图片的 DFS2 风格（以顶点 i 为起点进行深度优先）
void DFS(const std::vector<std::vector<int>> &G, int start,
         GraphTracer &graphTracer, Array2DTracer &tableTracer, LogTracer &logger)
{
    int n = (int)G.size();
    std::vector<bool> visited(n, false);
    std::vector<int> dfn(n, 0);
    int cnt = 0;
    logger.println("从顶点 " + std::to_string(start) + " 开始深度优先遍历");
    Tracer::delay();

    // 初始化表格：第一列为表头，第一行为顶点编号，第二行为 DFN（初始为 "-"）


    // 从 start 开始 DFS
    DFSRec(G, start, -1, dfn, visited, cnt, graphTracer, tableTracer, logger);
    // 输出连通性判断
    bool connected = true;
    for (int i = 0; i < n; ++i) if (!visited[i]) { connected = false; break; }
    if (connected) logger.println("所有顶点都已遍历，该图连通");
    else logger.println("存在未遍历的顶点，该图不连通");
    Tracer::delay();

    // 若图不连通，可尝试对剩余未访问顶点再调用DFS，用于其余目的（保证遍历全部连通分量）
    // for (int i = 0; i < n; ++i) {
    //     if (!visited[i]) {
    //         DFSRec(G, i, -1, dfn, visited, cnt, graphTracer, tableTracer, logger);
    //     }
    // }


}

int main()
{
    GraphTracer graphTracer = GraphTracer("二叉树的深度优先遍历").directed(false);
    Array2DTracer tableTracer = Array2DTracer("深度优先编号");
    LogTracer logger = LogTracer("运行日志");
    Layout::setRoot(VerticalLayout({graphTracer, tableTracer, logger}));

    // 构造固定的二叉树示例（无向图的邻接矩阵表示）
    // 使用完备二叉树的结构：对于节点 i，左孩子 2*i+1，右孩子 2*i+2（若存在）
    int N = 11; // 示例：7 个节点，索引 0..6
    json Gjson = json::array();
    for (int i = 0; i < N; ++i) {
        json row = json::array();
        for (int j = 0; j < N; ++j) row.push_back(0);
        Gjson.push_back(row);
    }
    auto addEdge = [&](int a, int b){ if (a>=0 && a<N && b>=0 && b<N) { Gjson[a][b]=1; Gjson[b][a]=1; } };
    for (int i = 0; i < N; ++i) {
        int l = 2*i + 1;
        int r = 2*i + 2;
        addEdge(i, l);
        addEdge(i, r);
    }

    graphTracer.set(Gjson);
    graphTracer.layoutTree();

    json row0 = json::array();
    json row1 = json::array();
    row0.push_back("顶点");
    row1.push_back("深度优先编号");
    for (int i = 0; i < N; ++i) {
        row0.push_back(std::to_string(i));
        row1.push_back(std::string("-"));
    }
    json table = json::array();
    table.push_back(row0);
    table.push_back(row1);
    tableTracer.set(table);

    Tracer::delay();

    // 把邻接矩阵转成 std::vector<std::vector<int>>
    std::vector<std::vector<int>> G;
    for (size_t i = 0; i < Gjson.size(); ++i) {
        std::vector<int> row;
        for (size_t j = 0; j < Gjson[i].size(); ++j) row.push_back((int)Gjson[i][j]);
        G.push_back(row);
    }

    // 从树根（0）开始深度优先遍历
    DFS(G, 0, graphTracer, tableTracer, logger);

    return 0;
}
