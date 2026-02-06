#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>
#include <queue>

using json = nlohmann::json;

// 从顶点 k 出发的一次 BFS
void BFSRec(const std::vector<std::vector<int>> &G, int k,
            std::vector<bool> &visited, std::vector<int> &dfn, int &count,
            GraphTracer &graphTracer, Array2DTracer &tableTracer, Array1DTracer &queueTracer, LogTracer &logger)
{
    std::queue<int> Q;
    // 访问顶点 k
    visited[k] = true;
    dfn[k] = count++;

    // 可视化：标记并在表格中写入编号
    graphTracer.visit(k);
    tableTracer.patch(1, k + 1, dfn[k]);
    Tracer::delay();

    tableTracer.depatch(1, k + 1);
    Tracer::delay();

    // 更新队列可视化（入队 k）
    std::vector<int> qview;
    Q.push(k);
    qview.push_back(k);
    // 显示队列中实际内容（不限定长度），并记录长度变化
    json qjson = json::array();
    for (int x : qview)
        qjson.push_back(x);
    queueTracer.set(qjson);
    logger.println(std::string("顶点 ") + std::to_string(k) + " 入队，队列大小= " + std::to_string((int)qview.size()));
    graphTracer.leave(k);
    graphTracer.select(k);
    Tracer::delay();

    while (!Q.empty())
    {
        int v = Q.front();
        Q.pop();

        queueTracer.select(0); // 高亮队首
        Tracer::delay();
        queueTracer.deselect(0);
        Tracer::delay();
        // 更新队列可视化（出队）: remove front from qview
        if (!qview.empty())
        {
            qview.erase(qview.begin());
        }
        json qjson2 = json::array();
        for (int x : qview)
            qjson2.push_back(x);
        queueTracer.set(qjson2);
        logger.println(std::string("顶点 ") + std::to_string(v) + " 出队，队列大小= " + std::to_string((int)qview.size()));
        Tracer::delay();
        // 可视化：高亮当前出队的节点
        graphTracer.visit(v);
        Tracer::delay();

        // 遍历 v 的所有邻接点
        for (int w = 0; w < (int)G.size(); ++w)
        {
            if (G[v][w])
            {
                if (visited[w])
                {
                    logger.println(std::string("顶点 ") + std::to_string(w) + " 已访问，跳过");
                    Tracer::delay();
                    continue;
                }
                // 可视化经过边 (v,w)
                graphTracer.visit(v, w);
                Tracer::delay();
                if (!visited[w])
                {
                    // 访问顶点 w
                    visited[w] = true;
                    dfn[w] = count++;
                    // 在表格里同步填写 dfn
                    tableTracer.patch(1, w + 1, dfn[w]);
                    Tracer::delay();
                    tableTracer.depatch(1, w + 1);
                    graphTracer.select(w);
                    Tracer::delay();

                    // 将 w 入队并在图上标记
                    Q.push(w);
                    qview.push_back(w);
                    // 更新队列显示（仅显示实际元素）并记录长度
                    json qjson3 = json::array();
                    for (int x : qview)
                        qjson3.push_back(x);
                    queueTracer.set(qjson3);
                    logger.println(std::string("顶点 ") + std::to_string(w) + " 入队，队列大小= " + std::to_string((int)qview.size()));
                    graphTracer.select(w);
                    Tracer::delay();
                    graphTracer.deselect(w);
                    Tracer::delay();
                }
                // 取消临时访问高亮
                graphTracer.leave(v, w);
                Tracer::delay();
            }
        }
        // 取消当前节点的持久选中
        graphTracer.leave(v);
        Tracer::delay();
    }
}

// 主算法
void BFSTraverse(const std::vector<std::vector<int>> &G,
                 GraphTracer &graphTracer, Array2DTracer &tableTracer, Array1DTracer &queueTracer, LogTracer &logger)
{
    int n = (int)G.size();
    std::vector<bool> visited(n, false);
    std::vector<int> dfn(n, 0);
    int count = 1;

    logger.println("从顶点 0 开始图的广度优先遍历");
    Tracer::delay();
    BFSRec(G, 0, visited, dfn, count, graphTracer, tableTracer, queueTracer, logger);

    // 输出连通性判断
    bool connected = true;
    for (int i = 0; i < n; ++i)
        if (!visited[i])
        {
            connected = false;
            break;
        }
    if (connected)
        logger.println("所有顶点都已遍历，该图连通");
    else
        logger.println("存在未遍历的顶点，该图不连通");
    Tracer::delay();

    // 若图不连通，可尝试对剩余未访问顶点再调用BFS，用于其余目的（保证遍历全部连通分量）
    // for (int i = 0; i < n; ++i) {
    //     if (!visited[i]) {
    //         BFSRec(G, i, visited, dfn, count, graphTracer, tableTracer, queueTracer, logger);
    //     }
    // }
}

int main()
{
    GraphTracer graphTracer = GraphTracer("BFS 树遍历").directed(false);
    Array2DTracer tableTracer = Array2DTracer("广度优先编号");
    Array1DTracer queueTracer = Array1DTracer("队列 Q");
    LogTracer logger = LogTracer("运行日志");
    Layout::setRoot(VerticalLayout({graphTracer, tableTracer, queueTracer, logger}));

    // 固定示例二叉树（无向）——便于测试和复现
    int N = 11; // 完备二叉树 11 个节点（0..10）
    json Gjson = json::array();
    for (int i = 0; i < N; ++i)
    {
        json row = json::array();
        for (int j = 0; j < N; ++j)
            row.push_back(0);
        Gjson.push_back(row);
    }
    auto addEdge = [&](int a, int b)
    { if (a>=0 && a<N && b>=0 && b<N) { Gjson[a][b]=1; Gjson[b][a]=1; } };
    for (int i = 0; i < N; ++i)
    {
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        addEdge(i, l);
        addEdge(i, r);
    }

    graphTracer.set(Gjson);
    graphTracer.layoutTree(); // 布局为树形

    // 可视化，展示广度优先编号，第一行为顶点，第二行为广度优先编号
    json row0 = json::array();
    json row1 = json::array();
    row0.push_back("顶点");
    row1.push_back("广度优先编号");
    for (int i = 0; i < N; ++i)
    {
        row0.push_back(std::to_string(i));
        row1.push_back(std::string("-"));
    }
    json table = json::array();
    table.push_back(row0);
    table.push_back(row1);
    tableTracer.set(table);
    Tracer::delay();

    // 将 json 转为邻接矩阵 vector<vector<int>>
    std::vector<std::vector<int>> G;
    for (size_t i = 0; i < Gjson.size(); ++i)
    {
        std::vector<int> row;
        for (size_t j = 0; j < Gjson[i].size(); ++j)
            row.push_back((int)Gjson[i][j]);
        G.push_back(row);
    }

    // 初始化队列可视化为空
    json qinit = json::array();
    queueTracer.set(qinit);
    Tracer::delay();

    // 调用 BFSTraverse
    BFSTraverse(G, graphTracer, tableTracer, queueTracer, logger);

    return 0;
}
