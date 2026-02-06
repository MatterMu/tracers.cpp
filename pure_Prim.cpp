#include <bits/stdc++.h>
using namespace std;

const int INF = 1000000000;

// 根据图片的伪代码，保留变量名和逻辑实现 Prim 算法
// 使用 1-based 索引，C 为 (n+1)x(n+1) 的邻接矩阵，若无边则为 INF

void Prim(int n, const vector<vector<int>>& C)
{
    vector<int> LOWCOST(n+1);
    vector<int> CLOSSET(n+1);
    int i, j, k;
    int min;

    // 初始化 LOWCOST 和 CLOSSET
    for (i = 2; i <= n; i++) {
        LOWCOST[i] = C[1][i];
        CLOSSET[i] = 1;
    }

    // 执行 n-1 次
    for (i = 2; i <= n; i++) {
        // 在 LOWCOST 中选取最小边，记 CLOSET 中对应的顶点序号 k
        min = LOWCOST[i];
        k = i;
        for (j = 2; j <= n; j++) {
            if (LOWCOST[j] < min) {
                min = LOWCOST[j];
                k = j;
            }
        }

        // 输出顶点 k 和对应的权值（按图片中的格式）
        cout << "(" << k << "," << CLOSSET[k] << ") 权值=" << min << endl;

        // 把顶点 k 加入最小生成树中（设置为 INF 表示已加入）
        LOWCOST[k] = INF;

        // 调整数组 LOWCOST 和 CLOSSET
        for (j = 2; j <= n; j++) {
            if (C[k][j] < LOWCOST[j] && LOWCOST[j] != INF) {
                LOWCOST[j] = C[k][j];
                CLOSSET[j] = k;
            }
        }
    }
}

int main()
{
    // 示例图（1-based），节点数 n
    int n = 7;
    // 构建 (n+1)x(n+1) 的邻接矩阵（无向图），无边用 INF
    vector<vector<int>> C(n+1, vector<int>(n+1, INF));

    // 对角为 0
    for (int i = 1; i <= n; ++i) C[i][i] = 0;

    // 示例边（无向，互相赋值）
    auto add_edge = [&](int u, int v, int w) {
        C[u][v] = w;
        C[v][u] = w;
    };

    add_edge(1,2,7);
    add_edge(1,4,5);
    add_edge(2,3,8);
    add_edge(2,4,9);
    add_edge(2,5,7);
    add_edge(3,5,5);
    add_edge(4,5,15);
    add_edge(4,6,6);
    add_edge(5,6,8);
    add_edge(5,7,9);
    add_edge(6,7,11);

    cout << "Running Prim (示例图)，n=" << n << "\n";
    Prim(n, C);

    return 0;
}
