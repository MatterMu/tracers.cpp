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
    // 在 tableTracer 第二行（索引 1）第 i 列写入 dfn 值
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

// DFS函数（以顶点 i 为起点进行深度优先）
void DFS(const std::vector<std::vector<int>> &G, int start,
         GraphTracer &graphTracer, Array2DTracer &tableTracer, LogTracer &logger)
{
    int n = (int)G.size();
    std::vector<bool> visited(n, false);
    std::vector<int> dfn(n, 0);
    int cnt = 0;
    logger.println("从顶点 " + std::to_string(start) + " 开始深度优先遍历");
    Tracer::delay();
    // 从start开始 DFS
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
    GraphTracer graphTracer = GraphTracer("图的深度优先遍历").directed(false);
    Array2DTracer tableTracer = Array2DTracer("深度优先编号");
    LogTracer logger = LogTracer("运行日志");
    Layout::setRoot(VerticalLayout({graphTracer, tableTracer, logger}));

    // 使用固定示例邻接矩阵（确定性、便于测试）
    json Gjson = json::array();
    int N = 7;
    for (int i = 0; i < N; ++i) {
        json row = json::array();
        for (int j = 0; j < N; ++j) row.push_back(0);
        Gjson.push_back(row);
    }
    // 添加若干边（无向）
    auto addEdge = [&](int a, int b) {
        Gjson[a][b] = 1;
        Gjson[b][a] = 1;
    };
    addEdge(1,2); addEdge(1,3); addEdge(1,4); addEdge(2,4); addEdge(2,6);
    addEdge(3,4); addEdge(3,5); addEdge(4,5); addEdge(4,6); addEdge(5,6);
    graphTracer.set(Gjson);
    // 使用圆形布局避免节点在一条直线
    graphTracer.layoutCircle();

    // 初始化表格：第一行为顶点编号（0..n-1），第二行为 DFN（初始为 "-"）
    json row0 = json::array();
    json row1 = json::array();
    row0.push_back("顶点");
    row1.push_back("深度优先编号");
    for (int i = 1; i < N+1; ++i) {
        row0.push_back(std::to_string(i - 1));
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
        for (size_t j = 0; j < Gjson[i].size(); ++j) {
            row.push_back((int)Gjson[i][j]);
        }
        G.push_back(row);
    }

    // 进行深度优先遍历（从顶点 1 开始）
    DFS(G, 1, graphTracer, tableTracer, logger);

    return 0;
}