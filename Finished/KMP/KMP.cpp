#include "algorithm-visualizer.h"
#include <iostream>
#include <cstring>

using json = nlohmann::json;
Array2DTracer pattern_tracer = Array2DTracer("模式串");
Array1DTracer string_tracer = Array1DTracer("目标串");
LogTracer logger = LogTracer("运行日志");

void GetNext(char *T, int Next[])
{
    int lenT = std::strlen(T); // lenT为模式串T的长度
    int j = 0, k = -1;
    if (lenT <= 0)
        return;
    // 可视化
    logger.println("开始计算Next数组，初始化Next[0] = -1");
    Tracer::delay();
    Next[0] = -1;

    // 可视化
    pattern_tracer.patch(0, 1, -1);
    pattern_tracer.depatch(0, 1);
    Tracer::delay();

    // 在 j < lenT 时扩展 Next，避免写出界
    while (j < lenT)
    {
        if (k == -1 || T[j] == T[k])
        {
            // 需要向前分别跨越一个
            if (k == -1)
            {
                logger.println("k=-1，Next[" + std::to_string(j + 1) + "]设为0");
            }
            if (k != -1)
            {
                logger.println("模式字符匹配，Next[" + std::to_string(j + 1) + "]设为" + std::to_string(k + 1));
            }
            Tracer::delay();

            ++j;
            ++k;
            Next[j] = k;
            pattern_tracer.patch(0, j + 1, k);
            Tracer::delay();
            pattern_tracer.depatch(0, j + 1);
            pattern_tracer.deselect(1, j);
            pattern_tracer.deselect(2, k);
            Tracer::delay();
            if (j < lenT)
            {
                pattern_tracer.select(1, j + 1);
                pattern_tracer.select(2, k + 1);
                Tracer::delay();
            }
        }
        else
        {
            // 可视化
            logger.println(std::string("不匹配，k 从 ") + std::to_string(k) + " 回退到 Next[k] = " + std::to_string(Next[k]));
            if (Next[k] != -1)
            {
                pattern_tracer.select(0, k + 1);
                Tracer::delay();
                pattern_tracer.deselect(2, k + 1);
                pattern_tracer.select(2, Next[k] + 1);
                Tracer::delay();
                pattern_tracer.deselect(0, k + 1);
                Tracer::delay();
                k = Next[k];
            }
            else
            {
                logger.println("k 回退到 -1");
                Tracer::delay();
                pattern_tracer.select(0, k + 1);
                Tracer::delay();
                pattern_tracer.deselect(2, k + 1);
                Tracer::delay();
                pattern_tracer.deselect(0, k + 1);
                Tracer::delay();
                k = Next[k];
            }
        }
    }
    // 可视化
    pattern_tracer.deselect(2, k + 2);
    json row_index = json::array();
    row_index.push_back(std::string("索引"));
    for (int i = 0; i < lenT; ++i)
        row_index.push_back(i);
    row_index.push_back("-"); // 补齐列数
    json row_next = json::array();
    row_next.push_back(std::string("Next值字符"));
    for (int i = 0; i < lenT + 1; ++i)
        row_next.push_back(Next[i]);
    json row_pat = json::array();
    row_pat.push_back(std::string("模式字符"));
    for (int i = 0; i < lenT; ++i)
        row_pat.push_back(std::string(1, T[i]));
    row_pat.push_back("-"); // 补齐列数
    pattern_tracer.set(json::array({row_index, row_next, row_pat}));
    Tracer::delay();
}

int StrMatch_KMP(char *S, char *T, int pos = 0)
{
    // S 为主串, T 为模式, 长度分别为了 lenS 和 lenT; 串采用顺序存储结构
    int lenS = std::strlen(S);
    int lenT = std::strlen(T);
    std::vector<int> match_positions;

    if (lenT == 0)
        return pos; // 空模式视为在 pos 处匹配
    if (pos < 0)
        pos = 0;
    if (pos >= lenS)
        return -1;

    // 可视化

    json row_next = json::array();
    row_next.push_back(std::string("Next值字符"));
    for (int i = 0; i < lenT + 1; ++i)
        row_next.push_back("-");
    json row_patj = json::array();
    row_patj.push_back(std::string("模式字符j"));
    for (int i = 0; i < lenT; ++i)
        row_patj.push_back(std::string(1, T[i]));
    row_patj.push_back("-"); // 补齐列数
    json row_patk = json::array();
    row_patk.push_back(std::string("模式字符k"));
    for (int i = 0; i < lenT; ++i)
        row_patk.push_back(std::string(1, T[i]));
    row_patk.push_back("-"); // 补齐列数
    pattern_tracer.set(json::array({row_next, row_patj, row_patk}));
    Tracer::delay();

    // 为 Next 数组分配空间，大小为 lenT（足够放置 0..lenT-1 的 Next）
    int *Next = new int[lenT + 1];
    // 计算 Next
    GetNext(T, Next);

    logger.println("Next数组求解完毕，开始进行 KMP 模式匹配");
    Tracer::delay();
    int i = pos;
    int j = 0; // 从第一个位置开始比较

    string_tracer.select(i);
    pattern_tracer.select(2, j + 1);
    Tracer::delay();
    while (i < lenS && j < lenT)
    {
        if (j == 0 || S[i] == T[j])
        {
            ++i;
            ++j;
            // 可视化
            if (j == 1)
                logger.println("j=0, 将模式串和主串分别前移一位");
            else
                logger.println(std::string("字符匹配，i 和 j 分别移动到 ") + std::to_string(i) + ", " + std::to_string(j));
            Tracer::delay();
            string_tracer.deselect(i - 1);
            pattern_tracer.deselect(2, j);
            pattern_tracer.deselect(0, j);
            string_tracer.select(i);
            pattern_tracer.select(2, j + 1);
            Tracer::delay();
        }
        else
        {
            logger.println(std::string("字符不匹配，j 从 ") + std::to_string(j) + " 回退到 Next[j] = " + std::to_string(Next[j]));
            Tracer::delay();
            pattern_tracer.deselect(0, j + 1);
            pattern_tracer.select(1, j + 1); // j-1 -> column j
            Tracer::delay();
            pattern_tracer.deselect(2, j + 1);
            pattern_tracer.select(0, Next[j] + 1);
            Tracer::delay();
            pattern_tracer.select(2, Next[j] + 1);
            pattern_tracer.deselect(1, j + 1);
            j = Next[j];
        }
    }

    delete[] Next;

    if (j == lenT)
    {
        // 可视化
        logger.println("匹配成功，匹配起始下标为 " + std::to_string(i - lenT));
        pattern_tracer.deselect(2, j + 1);
        string_tracer.deselect(i);
        string_tracer.select(i - lenT, i - 1);
        Tracer::delay();
        string_tracer.deselect(i - lenT, i - 1);
        
        return i - lenT; // 返回匹配的起始下标
    }
    else
        return -1; // 匹配不成功
}

int main()
{
    // 示例测试
    char S[] = "AAAABAABAAAABAAABAAAA"; // 主串
    char T[] = "AAAABAAA";              // 模式串
    // 可视化
    Layout::setRoot(VerticalLayout({pattern_tracer, string_tracer, logger}));
    json srow = json::array();
    for (char c : S)
        srow.push_back(std::string(1, c));
    string_tracer.set(srow);

    int pos = StrMatch_KMP(S, T, 0);
    if (pos >= 0)
        std::cout << "匹配成功，起始下标 = " << pos << std::endl;
    else
        std::cout << "匹配失败" << std::endl;

    return 0;
}
