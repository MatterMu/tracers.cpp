#include "algorithm-visualizer.h"
#include <iostream>
#include <cstring>
#include <vector>

// 全局可视化对象（按照 dijkstra.cpp 的风格）
Array2DTracer pattern_tracer = Array2DTracer("Pattern"); // row0: Next, row1: pattern chars, row2: pointer/markers
Array1DTracer string_tracer = Array1DTracer("String");
LogTracer logger = LogTracer("Console");
// 保持与纯算法相同的函数签名和变量命名，仅插入可视化语句
void GetNext(const std::string& Pattern, int Next[])
{
    int lenT = Pattern.length(); // lenT为模式串T的长度
    int j = 0, k = -1;
    if (lenT <= 0) return;
    Next[0] = -1;

    logger.println(std::string("开始计算 Next 数组，lenT=") + std::to_string(lenT));
    Tracer::delay();

    // 在 j < lenT 时扩展 Next，避免写出界
    while (j < lenT - 1) {
		// 高亮比较位置 k 和 j（在 pattern_tracer 的第 2 行显示为指针）
        // 显示当前 k 指针（在第三行展示为选中）
		pattern_tracer.select(1, j); // 显示j位置
		Tracer::delay();

        if (k == -1 || Pattern[j] == Pattern[k]) {
			pattern_tracer.deselect(1, j); // 去掉j位置选中
			pattern_tracer.deselect(2, k); // 去掉k指针选中
			++j; ++k;
            Next[j] = k;
			// 可视化：更新 Next[j]
			pattern_tracer.select(1, j); // 显示更新后的j位置
			pattern_tracer.select(2, k); // 显示更新后的k指针位置
			Tracer::delay();
			pattern_tracer.patch(0, j, k);
			Tracer::delay();
			pattern_tracer.depatch(0, j);
			Tracer::delay();
			logger.println(std::string("Next[") + std::to_string(j) + "] = " + std::to_string(k));

        } else {
            // 可视化：说明回退到 Next[k]
            logger.println(std::string("不匹配，k 从 ") + std::to_string(k) + " 回退到 Next[k] = " + std::to_string(Next[k]));
			k = Next[k];
			pattern_tracer.select(0, k-1);
			Tracer::delay();
			pattern_tracer.deselect(2, k);
			pattern_tracer.select(2, Next[k-1]);
			Tracer::delay();
			pattern_tracer.deselect(0, k-1);
            Tracer::delay();
        }
    }
}

int StrMatch_KMP(const std::string& String, const std::string& Pattern, int pos = 0)
{
    // String 为主串, Pattern 为模式, 长度分别为了 lenS 和 lenT; 串采用顺序存储结构
    int lenS = String.length();
    int lenT = Pattern.length();

    if (lenT == 0) return pos; // 空模式视为在 pos 处匹配
    if (pos < 0) pos = 0;
    if (pos >= lenS) return -1;

    // 为 Next 数组分配空间，大小为 lenT（足够放置 0..lenT-1 的 Next）
    int* Next = new int[lenT];

    // 计算 Next
    GetNext(Pattern, Next);

    // 可视化：确保 Next 数组完整显示（GetNext 在内部已更新 tracerNext）
    Tracer::delay();

    int i = pos; 
    int j = 0; // 从第一个位置开始比较

    logger.println(std::string("开始匹配，主串长度=") + std::to_string(lenS + 0) + ", 模式长度=" + std::to_string(lenT));
    Tracer::delay();

    while (i < lenS && j < lenT) {
        // 可视化：选中当前比较位置（主串与模式）
        string_tracer.select(i);
        pattern_tracer.select(1, j);
        Tracer::delay();

        if (j == -1 || String[i] == Pattern[j]) { ++i; ++j; 
            logger.println(std::string("匹配: String[") + std::to_string(i-1) + "] == Pattern[" + std::to_string(j-1) + "]");
            Tracer::delay();
        }
        else {
            logger.println(std::string("不匹配: String[") + std::to_string(i) + "] != Pattern[" + std::to_string(j) + "], j 将回退到 Next[j] = " + std::to_string(Next[j]));
            // 可视化回退：高亮 Next 的位置并移动指针行
            if (j - 1 >= 0) {
                pattern_tracer.select(0, j - 1);
                Tracer::delay();
                pattern_tracer.deselect(0, j - 1);
            }
            int newj = Next[j];
            pattern_tracer.select(2, (newj < 0 ? 0 : newj));
            Tracer::delay();
            pattern_tracer.deselect(2, (newj < 0 ? 0 : newj));
            j = newj;
            Tracer::delay();
        }
    }

    delete[] Next;

    if (j == lenT) {
        int res = i - lenT;
        logger.println(std::string("匹配成功，起始下标 = ") + std::to_string(res));
        return res; // 返回匹配的起始下标
    }
    else {
        logger.println(std::string("匹配不成功"));
        return -1; // 匹配不成功
    }
}

int main()
{
    // 示例测试
    const std::string String = "abxabcabcaby"; // 主串
    const std::string Pattern = "abcaby";       // 模式串
	
	// 布局：模式串、主串、日志（垂直）
	Layout::setRoot(VerticalLayout({pattern_tracer, string_tracer, logger}));
    // 初始展示：主串与模式（pattern_tracer 在 StrMatch_KMP 内会被完整设置）
    string_tracer.set(String);
    // 为 pattern_tracer 做一个简要初始化（Next 行为-1，pattern 行为字符，指针行为空格）
	int lt = Pattern.length();
    std::vector<nlohmann::json> row0(lt), row1(lt), row2(lt);
    for (int i = 0; i < lt; ++i) {
        row0[i] = -1;
        char tmp[2] = {Pattern[i], '\0'};
        row1[i] = std::string(tmp);
        row2[i] = std::string(tmp);
    }
    nlohmann::json arr = nlohmann::json::array();
    arr.push_back(row0);
    arr.push_back(row1);
    arr.push_back(row2);
    pattern_tracer.set(arr);

	Tracer::delay();

    int pos = StrMatch_KMP(String, Pattern, 0);
    if (pos >= 0) std::cout << "匹配成功，起始下标 = " << pos << std::endl;
    else std::cout << "匹配失败" << std::endl;

    // // 另外一个示例，测试多次匹配起始位置
    // char S2[] = "ababababca";
    // char T2[] = "abababca";
    // int pos2 = StrMatch_KMP(S2, T2);
    // std::cout << "示例2：" << (pos2>=0? std::to_string(pos2) : std::string("-1")) << std::endl;

    return 0;
}
