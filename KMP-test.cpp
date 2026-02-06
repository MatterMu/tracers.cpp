#include "algorithm-visualizer.h"
#include <iostream>
#include <vector>
#include <string>

using json = nlohmann::json;

void GetNext(const std::string &pattern, std::vector<int> &_next, Array2DTracer &pattern_tracer, LogTracer &logger)
{
    int lenT = (int)pattern.size();
    int j = 1; // postfix pointer
    int k = 0; // prefix pointer

    // visualize (offset columns by 1: column 0 reserved for header)
    pattern_tracer.select(2, k + 1);
    Tracer::delay();
    while (j < lenT)
    {
        if (k == 0 || pattern[j] == pattern[k])
        {
            // visualize (column index = j+1)
            pattern_tracer.select(1, j + 1);
            Tracer::delay();

            ++j;
            ++k;
            _next[j - 1] = k;

            // visualize
            pattern_tracer.patch(0, j, k);
            Tracer::delay();
            pattern_tracer.depatch(0, j);
            Tracer::delay();
            pattern_tracer.deselect(1, j);
        }
        else
        {
            // visualize (column index = j+1)
            pattern_tracer.select(1, j + 1);
            Tracer::delay();

            // visualize backtrack
            logger.println(std::string("不匹配，k 从 ") + std::to_string(k) + " 回退到 Next[k-1] = " + std::to_string(_next[k - 1]));
            pattern_tracer.select(0, k); // k-1 -> column k
            Tracer::delay();
            pattern_tracer.deselect(2, k + 1);
            pattern_tracer.select(2, _next[k - 1] + 1);
            Tracer::delay();
            pattern_tracer.deselect(0, k);

            k = _next[k - 1];
        }
    }

    // for (; j < lenT; ++j)
    // {
    //     // visualize (column index = j+1)
    //     pattern_tracer.select(1, j + 1);
    //     Tracer::delay();

    //     while ((k > 0) && (pattern[j] != pattern[k]))
    //     {
    //         // visualize (adjust columns)
    //         pattern_tracer.select(0, k); // k-1 -> column k
    //         Tracer::delay();
    //         pattern_tracer.deselect(2, k + 1);
    //         pattern_tracer.select(2, _next[k - 1] + 1);
    //         Tracer::delay();
    //         pattern_tracer.deselect(0, k);

    //         k = _next[k - 1];
    //     }
    //     if (pattern[j] == pattern[k])
    //     {
    //         // visualize
    //         pattern_tracer.deselect(2, k + 1);
    //         pattern_tracer.select(2, k + 2);
    //         Tracer::delay();
    //         ++k;
    //     }
    //     // visualize
    //     pattern_tracer.patch(0, j + 1, k);
    //     Tracer::delay();
    //     pattern_tracer.depatch(0, j + 1);
    //     Tracer::delay();
    //     pattern_tracer.deselect(1, j + 1);

    //     _next[j] = k;
    // }

    // visualize final (deselect and set rows with header in column 0)
    pattern_tracer.deselect(2, k + 1);
    // set two rows: row_next and row_pat with header at column 0
    json row_next = json::array();
    row_next.push_back(std::string("Next值字符"));
    for (int i = 0; i < lenT; ++i) row_next.push_back(0);
    json row_patj = json::array();
    row_patj.push_back(std::string("模式字符j"));
    for (int i = 0; i < lenT; ++i) row_patj.push_back(std::string(1, pattern[i]));
    json row_patk = json::array();
    row_patk.push_back(std::string("模式字符k"));
    for (int i = 0; i < lenT; ++i) row_patk.push_back(std::string(1, pattern[i]));
    // duplicate pattern row for visualization (rows: next, pattern, pattern)
    pattern_tracer.set(json::array({row_next, row_patj, row_patk}));
    Tracer::delay();
}

void KMP(const std::string &str, const std::string &pattern, Array1DTracer &string_tracer, Array2DTracer &pattern_tracer, LogTracer &logger)
{
    std::vector<int> match_positions;
    int m = (int)pattern.size();

    std::vector<int> _next(m, 0);
    // initial pattern tracer with 3 rows: _next, pattern, pattern (add header column at index 0)
    json row_next = json::array();
    row_next.push_back(std::string("Next值字符"));
    for (int i = 0; i < m; ++i) row_next.push_back(0);
    json row_patj = json::array();
    row_patj.push_back(std::string("模式字符j"));
    for (int i = 0; i < m; ++i) row_patj.push_back(std::string(1, pattern[i]));
    json row_patk = json::array();
    row_patk.push_back(std::string("模式字符k"));
    for (int i = 0; i < m; ++i) row_patk.push_back(std::string(1, pattern[i]));
    // duplicate pattern row for visualization (rows: next, pattern, pattern)
    pattern_tracer.set(json::array({row_next, row_patj, row_patk}));
    Tracer::delay();

    GetNext(pattern, _next, pattern_tracer, logger);

    int i = 0; // string pointer
    int k = 0; // pattern pointer
    for (; i < (int)str.size(); ++i)
    {
        // visualize (column offset +1)
        string_tracer.select(i);
        pattern_tracer.select(1, k + 1);
        Tracer::delay();

        while ((k > 0) && (str[i] != pattern[k]))
        {
            // visualize (adjust columns)
            pattern_tracer.select(0, k); // k-1 -> column k
            Tracer::delay();
            pattern_tracer.deselect(1, k + 1);
            pattern_tracer.select(1, _next[k - 1] + 1);
            Tracer::delay();
            pattern_tracer.deselect(0, k);

            k = _next[k - 1];
        }
        if (str[i] == pattern[k])
        {
            ++k;
            if (k == m)
            {
                int match_start = i - m + 1;
                match_positions.push_back(match_start);
                // visualize match
                string_tracer.select(match_start, match_start + m - 1);
                Tracer::delay();
                string_tracer.deselect(match_start, match_start + m - 1);
                Tracer::delay();

                pattern_tracer.select(0, k);
                Tracer::delay();
                pattern_tracer.deselect(1, k);
                Tracer::delay();
                pattern_tracer.select(1, _next[k - 1] + 1);
                Tracer::delay();
                pattern_tracer.deselect(0, k);

                k = _next[k - 1];
            }
            else
            {
                pattern_tracer.deselect(1, k);
                pattern_tracer.select(1, k + 1);
                Tracer::delay();
            }
        }
        else
        {
            pattern_tracer.select(0, k + 1);
            Tracer::delay();
        }

        pattern_tracer.deselect(0, k + 1);
        pattern_tracer.deselect(1, k + 1);
        string_tracer.deselect(i);
    }

    // final highlight matches
    for (int pos : match_positions)
    {
        string_tracer.select(pos, pos + m - 1);
        Tracer::delay();
        string_tracer.deselect(pos, pos + m - 1);
    }
}

int main()
{
    const std::string str = "AAAABAABAAAABAAABAAAA";
    const std::string pattern = "AAAABAAA";

    Array2DTracer pattern_tracer = Array2DTracer("模式");
    Array1DTracer string_tracer = Array1DTracer("目标串");
    LogTracer logger = LogTracer("运行日志");
    Layout::setRoot(VerticalLayout({pattern_tracer, string_tracer, logger}));

    // prepare string tracer array
    json srow = json::array();
    for (char c : str) srow.push_back(std::string(1, c));
    string_tracer.set(srow);
    
    KMP(str, pattern, string_tracer, pattern_tracer, logger);
    Tracer::delay();

    return 0;
}
