#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

struct Edge
{
    int begin;
    int end;
    int cost;
};

using EdgeSet = std::vector<Edge>;

struct Vertex
{
    char data;
};

// 路径压缩并查集（保持原样）
int Find(int x, std::vector<int> &parents)
{
    if (parents[x] == -1 || parents[x] == x)
        return x;
    return parents[x] = Find(parents[x], parents);
}

// Kruskal with visualization (keeps original logic, only adds tracer calls)
// 增加 parentsTracer 参数用于同步展示 parents 数组
void Kruskal_Min_Tree(EdgeSet &edges, int vexnum, int arcnum, const std::vector<Vertex> &vertices,
                      GraphTracer &tracer, LogTracer &logger, Array2DTracer &edgeTable,
                      Array1DTracer &parentsTracer)
{
    std::vector<int> parents(vexnum, -1);
    logger.println("初始化各结点的parent为自身");
    Tracer::delay();
    for (int i = 0; i < vexnum; ++i) {
        parents[i] = i;
        parentsTracer.patch(i, i);
        Tracer::delay();
        parentsTracer.depatch(i);
        Tracer::delay();
    }
    int bnf, edf;
    int taken = 0;
    int total_cost = 0;
    logger.println("按序遍历边表，输出最小生成树的边");
    Tracer::delay();

    for (int i = 0; i < arcnum && taken < vexnum - 1; ++i)
    {
        if (i > 0) {
            logger.println("寻找下一条最短边");
            Tracer::delay();
        }
        // 可视化，高亮边
        tracer.visit(edges[i].begin, edges[i].end);
        tracer.visit(edges[i].end, edges[i].begin);
        // 可视化，高亮边表
        edgeTable.selectCol(i + 1, 0, 2);
        Tracer::delay();
        // 可视化：闪烁
        tracer.leave(edges[i].begin, edges[i].end);
        tracer.leave(edges[i].end, edges[i].begin);
        edgeTable.deselectCol(i + 1, 0, 2);
        Tracer::delay();

        logger.println("调用Find函数获取parent信息，并进行路径压缩");
        Tracer::delay();

        bnf = Find(edges[i].begin, parents);
        edf = Find(edges[i].end, parents);

        // 每次调用 Find 或合并后，同步更新 parentsTracer 的显示
        parentsTracer.patch(edges[i].begin, bnf);
        parentsTracer.patch(edges[i].end, edf);
        Tracer::delay();
        parentsTracer.depatch(edges[i].begin);
        parentsTracer.depatch(edges[i].end);
        Tracer::delay();

        parentsTracer.select(edges[i].begin);
        parentsTracer.select(edges[i].end);
        Tracer::delay();
        if (bnf != edf)
        {
            logger.println("两者属于不同的连通分量，合并");
            Tracer::delay();
            // 合并集合
            parents[edf] = bnf;
            ++taken;
            total_cost += edges[i].cost;
            // 更新 parents 显示：只更新被修改的索引 edf（也可以整体更新，上面已有整体更新）
            
            parentsTracer.patch(edf, bnf);
            Tracer::delay();
            parentsTracer.depatch(edf);
            parentsTracer.deselect(edges[i].begin);
            parentsTracer.deselect(edges[i].end);
            Tracer::delay();

            // 双向选中边：视觉上持久高亮并输出日志
            tracer.visit(edges[i].begin, edges[i].end);
            tracer.visit(edges[i].end, edges[i].begin);
            // 若该边被选中，则在表中保持高亮状态（保持列选中）
            edgeTable.selectCol(i + 1, 0, 2);
            logger.println(std::string("选中: (") + vertices[edges[i].begin].data + "," + vertices[edges[i].end].data + ") 权值=" + std::to_string(edges[i].cost));
            Tracer::delay();
        }
        else
        {
            logger.println("两者属于同一连通分量");
            Tracer::delay();
            // 取消表格高亮（取消列选中）
            edgeTable.deselectCol(i + 1, 0, 2);
            parentsTracer.deselect(edges[i].begin);
            parentsTracer.deselect(edges[i].end);
            Tracer::delay();
        }
    }
    logger.println(std::string("Kruskal算法运行结束，生成树中边的总权值: ") + std::to_string(total_cost));
}

int main()
{
    // 示例测试（无向加权图，顶点用字母标识）
    std::vector<Vertex> vertices = {
        {'0'}, {'1'}, {'2'}, {'3'}, {'4'}, {'5'}, {'6'}};

    EdgeSet edges = {
        {0, 1, 7}, {0, 3, 5}, {1, 2, 8}, {1, 3, 9}, {1, 4, 7}, {2, 4, 5}, {3, 4, 15}, {3, 5, 6}, {4, 5, 8}, {4, 6, 9}, {5, 6, 11}};

    int vexnum = static_cast<int>(vertices.size());
    int arcnum = static_cast<int>(edges.size());

    // Tracers
    GraphTracer tracer = GraphTracer("Kruskal算法").directed(false).weighted();
    LogTracer logger = LogTracer("运行日志");
    Array2DTracer edgeTable = Array2DTracer("边表");
    Array1DTracer parentsTracer = Array1DTracer("parents数组"); // 新增用于显示 parents 数组
    Layout::setRoot(VerticalLayout({tracer, edgeTable, parentsTracer, logger}));

    // 为可视化准备邻接矩阵（GraphTracer接受 array2d）
    json Gjson = json::array();
    for (int i = 0; i < vexnum; ++i)
    {
        json r = json::array();
        for (int j = 0; j < vexnum; ++j)
            r.push_back(0);
        Gjson.push_back(r);
    }
    for (auto &e : edges)
    {
        Gjson[e.begin][e.end] = e.cost;
        Gjson[e.end][e.begin] = e.cost;
    }
    tracer.set(Gjson);
    // 使用圆形布局让点均匀分布
    tracer.layoutCircle();

    // 先对边集进行排序（不改变原始 edges 的顺序，创建一个副本 sortedEdges）
    EdgeSet sortedEdges = edges;
    std::sort(sortedEdges.begin(), sortedEdges.end(), [](const Edge &a, const Edge &b)
              { return a.cost < b.cost; });

    // 用 Array2DTracer 显示边表，横向：3 行 (costs, begin, end) × arcnum 列
    // 初始化表格为 3 行 arcnum+1列，填入占位符
    json table = json::array();
    for (int row = 0; row < 3; ++row)
    {
        json crow = json::array();
        for (int c = 0; c < arcnum + 1; ++c) crow.push_back(std::string("-"));
        table.push_back(crow);
    }
    table[0][0] = "cost";
    table[1][0] = "begin";
    table[2][0] = "end";
    edgeTable.set(table);

    // 初始化 parentsTracer 显示为 "-" * vexnum
    json pinit = json::array();
    for (int i = 0; i < vexnum; ++i) pinit.push_back(std::string("-"));
    parentsTracer.set(pinit);
    Tracer::delay();

    logger.println("遍历边集，按权值从小到大填充边表");
    Tracer::delay();
    // 按照排序后的顺序填充表格（按列填：第0行为cost，第1行为begin，第2行为end）
    for (int c = 0; c < arcnum; ++c)
    {
        const Edge &e = sortedEdges[c];
        // 填写edgeTable
        edgeTable.patch(0, c + 1, e.cost);
        edgeTable.patch(1, c + 1, e.begin);
        edgeTable.patch(2, c + 1, e.end);

        // 在填写时闪烁：高亮该列并在图上短暂高亮对应边
        tracer.visit(e.begin, e.end);
        tracer.visit(e.end, e.begin);
        edgeTable.selectCol(c + 1, 0, 2);
        Tracer::delay();
        tracer.leave(e.begin, e.end);
        tracer.leave(e.end, e.begin);
        edgeTable.deselectCol(c + 1, 0, 2);
        edgeTable.depatch(0, c + 1);
        edgeTable.depatch(1, c + 1);
        edgeTable.depatch(2, c + 1);
        Tracer::delay();
    }

    // 现在将排序后的边传入 Kruskal 算法并在运行时高亮表格对应列
    Kruskal_Min_Tree(sortedEdges, vexnum, arcnum, vertices, tracer, logger, edgeTable, parentsTracer);

    return 0;
}
