#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <cstring>

using json = nlohmann::json;

Array2DTracer pattern_tracer = Array2DTracer("模式"); // row0: Next, row1: pattern chars, row2: pointer/markers
Array1DTracer string_tracer = Array1DTracer("主串");
LogTracer logger = LogTracer("运行日志");

// KMP 算法逻辑
void GetNext(char* T, int Next[])
{
    int lenT = std::strlen(T); // lenT为模式串T的长度
    int j = 0, k = -1;
    if (lenT <= 0) return;
    Next[0] = -1;

    // 可视化初始化：row0 Next (空)，row1 pattern chars，row2 pointers
    std::vector<json> row0(lenT), row1(lenT), row2(lenT);
    for (int idx = 0; idx < lenT; ++idx) {
        row0[idx] = nullptr;
        char tmp[2] = {T[idx], '\0'};
        row1[idx] = std::string(tmp);
        row2[idx] = std::string(" ");
    }
    json arr = json::array();
    arr.push_back(row0);
    arr.push_back(row1);
    arr.push_back(row2);
    pattern_tracer.set(arr);

    logger.println(std::string("开始计算 Next 数组，lenT=") + std::to_string(lenT));
    Tracer::delay();

    while (j < lenT - 1) {
        // 高亮当前比较位 j 和 k（在第1行为字符，高亮第2行用于指针）
        if (k >= 0) pattern_tracer.select(2, k);
        pattern_tracer.select(1, j);
        Tracer::delay();

        if (k == -1 || T[j] == T[k]) {
            ++j; ++k;
            Next[j] = k;
            // 更新可视化 Next
            pattern_tracer.patch(0, j, k);
            logger.println(std::string("Next[") + std::to_string(j) + "] = " + std::to_string(k));
            Tracer::delay();
            // 取消高亮 j (字符行)
            pattern_tracer.deselect(1, j-1);
        } else {
            // 不匹配时可视化回退
            logger.println(std::string("不匹配，k 从 ") + std::to_string(k) + " 回退到 Next[k] = " + std::to_string(Next[k]));
            // 高亮 Next[k] 的位置
            if (k - 1 >= 0) {
                pattern_tracer.select(0, k - 1);
                Tracer::delay();
                pattern_tracer.deselect(0, k - 1);
            }
            k = Next[k];
            Tracer::delay();
        }
    }
    // 结束时去掉所有选择
    for (int i = 0; i < lenT; ++i) {
        pattern_tracer.deselect(0, i);
        pattern_tracer.deselect(1, i);
        pattern_tracer.deselect(2, i);
    }
    Tracer::delay();
}

int StrMatch_KMP(char* S, char* T, int pos = 0)
{
    int lenS = std::strlen(S);
    int lenT = std::strlen(T);

    if (lenT == 0) return pos;
    if (pos < 0) pos = 0;
    if (pos >= lenS) return -1;

    int* Next = new int[lenT];
    // 先计算 Next（内部含可视化）
    GetNext(T, Next);

    // 初始化主串与 pattern_tracer 显示（确保 Next 行显示计算结果）
    std::vector<json> Sdisplay(lenS);
    for (int i = 0; i < lenS; ++i) {
        char tmp[2] = {S[i], '\0'};
        Sdisplay[i] = std::string(tmp);
    }
    string_tracer.set(Sdisplay);

    // 重新设置 pattern_tracer 行以反映计算好的 Next
    std::vector<json> row0(lenT), row1(lenT), row2(lenT);
    for (int i = 0; i < lenT; ++i) {
        row0[i] = Next[i];
        char tmp[2] = {T[i], '\0'};
        row1[i] = std::string(tmp);
        row2[i] = std::string(" ");
    }
    json arr = json::array();
    arr.push_back(row0);
    arr.push_back(row1);
    arr.push_back(row2);
    pattern_tracer.set(arr);
    Tracer::delay();

    int i = pos;
    int j = 0;

    logger.println(std::string("开始匹配，主串长度=") + std::to_string(lenS) + ", 模式长度=" + std::to_string(lenT));
    Tracer::delay();

    while (i < lenS && j < lenT) {
        // 高亮主串与模式当前比较位置
        string_tracer.select(i);
        pattern_tracer.select(1, j);
        Tracer::delay();

        if (j == -1 || S[i] == T[j]) {
            ++i; ++j;
            logger.println(std::string("匹配: S[") + std::to_string(i-1) + "] == T[" + std::to_string(j-1) + "]");
            Tracer::delay();
        } else {
            logger.println(std::string("不匹配: S[") + std::to_string(i) + "] != T[" + std::to_string(j) + "]，回退到 Next[j] = " + std::to_string(Next[j]));
            // 可视化回退
            if (j - 1 >= 0) {
                pattern_tracer.select(0, j - 1);
                Tracer::delay();
                pattern_tracer.deselect(0, j - 1);
            }
            int newj = Next[j];
            pattern_tracer.deselect(1, j);
            if (newj >= 0) {
                pattern_tracer.select(1, newj);
                Tracer::delay();
                pattern_tracer.deselect(1, newj);
            }
            j = newj;
            Tracer::delay();
        }
        // 去掉当前选择
        string_tracer.deselect(i-1 >= 0 ? i-1 : 0);
        if (j >= 0 && j < lenT) pattern_tracer.deselect(1, j);
    }

    delete[] Next;

    if (j == lenT) {
        int res = i - lenT;
        logger.println(std::string("匹配成功，起始下标 = ") + std::to_string(res));
        return res;
    } else {
        logger.println(std::string("匹配不成功"));
        return -1;
    }
}

int main()
{
    const char S[] = "AAAABAABAAAABAAABAAAA";
    const char T[] = "AAAABAAA";

    // 布局
    Layout::setRoot(VerticalLayout({pattern_tracer, string_tracer, logger}));

    // 初始显示
    std::vector<json> Sdisplay(std::strlen(S));
    for (size_t i = 0; i < std::strlen(S); ++i) {
        char tmp[2] = {S[i], '\0'};
        Sdisplay[i] = std::string(tmp);
    }
    string_tracer.set(Sdisplay);

    // 初始化 pattern_tracer with empty Next and pattern chars
    int lt = std::strlen(T);
    std::vector<json> row0(lt), row1(lt), row2(lt);
    for (int i = 0; i < lt; ++i) {
        row0[i] = nullptr;
        char tmp[2] = {T[i], '\0'};
        row1[i] = std::string(tmp);
        row2[i] = std::string(" ");
    }
    json arr = json::array(); arr.push_back(row0); arr.push_back(row1); arr.push_back(row2);
    pattern_tracer.set(arr);
    Tracer::delay();

    // 将 const away for our function signatures
    char* Sbuf = const_cast<char*>(S);
    char* Tbuf = const_cast<char*>(T);

    int pos = StrMatch_KMP(Sbuf, Tbuf, 0);
    std::cout << "匹配结果位置: " << pos << std::endl;

    return 0;
}
