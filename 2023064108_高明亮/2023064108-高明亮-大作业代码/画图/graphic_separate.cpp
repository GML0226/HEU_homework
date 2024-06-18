#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip> // 包含用于格式化输出的头文件
#include <tchar.h> // 包含 TCHAR 的头文件

using namespace std;

// 结构体表示一个点
struct Point {
    double x;
    double y;
};

// 读取CSV文件，返回包含所有点的向量
vector<Point> readCSV(const string& filename) {
    vector<Point> points;
    ifstream file(filename);
    string line, x, y;

    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream ss(line);
            if (getline(ss, x, ',') && getline(ss, y, ',')) {
                try {
                    points.push_back({ stod(x), stod(y) });
                }
                catch (const invalid_argument& e) {
                    cerr << "Invalid number in file " << filename << ": " << line << endl;
                }
            }
            else {
                cerr << "Invalid line format in file " << filename << ": " << line << endl;
            }
        }
        file.close();
    }
    else {
        cerr << "Unable to open file " << filename << endl;
    }
    return points;
}

// 读取TXT文件中的浮点数值
double readAUC(const string& filename) {
    ifstream file(filename);
    double value;

    if (file.is_open()) {
        file >> value;
        file.close();
    }
    else {
        cerr << "Unable to open file " << filename << endl;
        value = 0.0;
    }

    return value;
}

// 读取RESULT.txt文件中的四个浮点数
vector<double> readResults(const string& filename) {
    vector<double> results(4, 0.0);
    ifstream file(filename);

    if (file.is_open()) {
        for (int i = 0; i < 4; ++i) {
            if (!(file >> results[i])) {
                cerr << "Failed to read value " << i + 1 << " from file " << filename << endl;
                break;
            }
        }
        file.close();
    }
    else {
        cerr << "Unable to open file " << filename << endl;
    }

    return results;
}

// 绘制坐标轴和刻度
void drawAxesSeparate(int offsetX, int offsetY, int width, int height) {
    setcolor(WHITE); // 设置颜色为白色
    // 绘制X轴
    line(offsetX, offsetY, offsetX + width, offsetY);
    // 绘制Y轴
    line(offsetX, offsetY, offsetX, offsetY - height);

    // 绘制X轴刻度和数值
    for (int i = 0; i <= 10; ++i) {
        int x = offsetX + i * (width / 10);
        line(x, offsetY, x, offsetY + 5);

        // 计算数值并转换为字符串
        double value = i * 0.1;
        stringstream ss;
        ss << fixed << setprecision(1) << value;
        string strValue = ss.str();

        // 转换 std::string 为 TCHAR*
        size_t size = strValue.size() + 1;
        TCHAR* tcharValue = new TCHAR[size];
        mbstowcs(tcharValue, strValue.c_str(), size);

        // 绘制数值
        outtextxy(x - textwidth(tcharValue) / 2, offsetY + 10, tcharValue);
        delete[] tcharValue;
    }

    // 绘制Y轴刻度和数值
    for (int i = 0; i <= 5; ++i) {
        int y = offsetY - i * (height / 5);
        line(offsetX, y, offsetX - 10, y);

        // 计算数值并转换为字符串
        double value = i * 0.2;
        stringstream ss;
        ss << fixed << setprecision(1) << value;
        string strValue = ss.str();

        // 转换 std::string 为 TCHAR*
        size_t size = strValue.size() + 1;
        TCHAR* tcharValue = new TCHAR[size];
        mbstowcs(tcharValue, strValue.c_str(), size);

        // 绘制数值
        outtextxy(offsetX - textwidth(tcharValue) - 10, y - textheight(tcharValue) / 2, tcharValue);
        delete[] tcharValue;
    }
}

// 绘制曲线
void drawCurveSeparate(const vector<Point>& points, int offsetX, int offsetY, int width, int height, COLORREF color) {
    if (points.empty()) {
        cerr << "No points to draw." << endl;
        return;
    }

    setcolor(color); // 设置曲线颜色
    moveto(offsetX + points[0].x * width, offsetY - points[0].y * height);
    for (const auto& point : points) {
        lineto(offsetX + point.x * width, offsetY - point.y * height);
    }
}

// 在每个曲线下方绘制文字标注
void drawLabelSeparate(int offsetX, int offsetY, int width, const string& label, double auc) {
    settextstyle(20, 0, _T("Arial"));

    // 创建带有AUC数值的标签
    stringstream ss;
    ss << label << " AUC=" << fixed << setprecision(6) << auc;
    string fullLabel = ss.str();

    // 将 std::string 转换为 TCHAR*
    size_t size = fullLabel.size() + 1;
    TCHAR* tcharLabel = new TCHAR[size];
    mbstowcs(tcharLabel, fullLabel.c_str(), size);

    outtextxy(offsetX + width / 2 - textwidth(tcharLabel) / 2, offsetY + 10, tcharLabel);

    delete[] tcharLabel;
}

void drawBarChart(const vector<vector<double>>& results, int offsetX, int offsetY, int width, int height) {
    int numBars = results.size();
    int numGroups = results[0].size();
    int totalSpacing = (numBars + 1) * numGroups; // 总间隔数
    int barWidth = width / (numBars * numGroups + totalSpacing); // 重新计算柱宽度
    int barSpacing = barWidth; // 设置柱之间的间隔等于柱宽度

    // 定义标签数组
    const char* labels[] = { "Accuracy", "Precision", "Recall", "F1 Score" };

    // 定义颜色数组
    int colors[] = { LIGHTMAGENTA, GREEN, YELLOW, LIGHTCYAN, BROWN };

    // 定义柱标签数组
    const char* barLabels[] = { "bpNET", "KNN", "Logic", "Rtrees", "SVM" };

    for (int i = 0; i < numGroups; ++i) {
        for (int j = 0; j < numBars; ++j) {
            int x = offsetX + (i * (numBars + 1) + j) * (barWidth + barSpacing);
            int barHeight = static_cast<int>(results[j][i] * height);
            setfillcolor(colors[j % 5]); // 使用不同的颜色
            bar(x, offsetY - barHeight, x + barWidth, offsetY);

            // 在柱的最上端标出数值
            stringstream ss;
            ss << fixed << setprecision(2) << results[j][i];
            string valueStr = ss.str();

            // 将 std::string 转换为 TCHAR*
            size_t size = valueStr.size() + 1;
            TCHAR* tcharValue = new TCHAR[size];
            mbstowcs(tcharValue, valueStr.c_str(), size);

            outtextxy(x + barWidth / 2 - textwidth(tcharValue) / 2, offsetY - barHeight - textheight(tcharValue) - 5, tcharValue);
            delete[] tcharValue;

            // 在柱的内部同一高度标出标签
            const char* barLabel = barLabels[j];
            size = strlen(barLabel) + 1;
            TCHAR* tcharBarLabel = new TCHAR[size];
            mbstowcs(tcharBarLabel, barLabel, size);

            outtextxy(x + barWidth / 2 - textwidth(tcharBarLabel) / 2, 750, tcharBarLabel);
            delete[] tcharBarLabel;
        }

        // 绘制组标签
        const char* label = labels[i];
        size_t size = strlen(label) + 1;
        TCHAR* tcharLabel = new TCHAR[size];
        mbstowcs(tcharLabel, label, size);
        outtextxy(offsetX + (i * (numBars + 1) + numBars / 2) * (barWidth + barSpacing) - textwidth(tcharLabel) / 2, offsetY + 25, tcharLabel);
        delete[] tcharLabel;
    }
}

int main() {
    // 初始化EasyX图形窗口
    int winWidth = 1000;
    int winHeight = 800;
    initgraph(winWidth, winHeight);

    // 读取CSV文件中的数据
    vector<Point> points1 = readCSV("../../bpNET2/bpNET2/roc_curve.csv");
    vector<Point> points2 = readCSV("../../knn2/knn2/roc_points.csv");
    vector<Point> points3 = readCSV("../../logic2/logic2/roc_curve_points.csv");
    vector<Point> points4 = readCSV("../../RTrees2/RTrees2/roc_points.csv");
    vector<Point> points5 = readCSV("../../svm/svm/roc_curve.csv");

    // 读取TXT文件中的AUC值
    double auc1 = readAUC("../../bpNET2/bpNET2/AUC.txt");
    double auc2 = readAUC("../../knn2/knn2/AUC.txt");
    double auc3 = readAUC("../../logic2/logic2/AUC.txt");
    double auc4 = readAUC("../../RTrees2/RTrees2/AUC.txt");
    double auc5 = readAUC("../../svm/svm/AUC.txt");

    // 检查是否成功读取数据
    if (points1.empty() || points2.empty() || points3.empty() || points4.empty() || points5.empty()) {
        cerr << "Failed to read one or more CSV files." << endl;
        closegraph();
        return 1;
    }

    // 绘制每条曲线在不同的区域
    int margin = 50;
    int graphWidth = (winWidth - 3 * margin) / 2;
    int graphHeight = (winHeight - 4 * margin) / 3;


    // 第1条曲线 (左上)
    drawAxesSeparate(margin, margin + graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points1, margin, margin + graphHeight, graphWidth, graphHeight, LIGHTMAGENTA);
    drawLabelSeparate(margin, margin + graphHeight - 75, graphWidth, "bpNET", auc1);

    // 第2条曲线 (右上)
    drawAxesSeparate(2 * margin + graphWidth, margin + graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points2, 2 * margin + graphWidth, margin + graphHeight, graphWidth, graphHeight, GREEN);
    drawLabelSeparate(2 * margin + graphWidth, margin + graphHeight - 75, graphWidth, "KNN", auc2);

    // 第3条曲线 (左中)
    drawAxesSeparate(margin, 2 * margin + 2 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points3, margin, 2 * margin + 2 * graphHeight, graphWidth, graphHeight, YELLOW);
    drawLabelSeparate(margin, 2 * margin + 2 * graphHeight - 75, graphWidth, "Logic", auc3);

    // 第4条曲线 (右中)
    drawAxesSeparate(2 * margin + graphWidth, 2 * margin + 2 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points4, 2 * margin + graphWidth, 2 * margin + 2 * graphHeight, graphWidth, graphHeight, LIGHTCYAN);
    drawLabelSeparate(2 * margin + graphWidth, 2 * margin + 2 * graphHeight - 75, graphWidth, "RTrees", auc4);

    // 第5条曲线 (底部中央)
    drawAxesSeparate((winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points5, (winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight, graphWidth, graphHeight, BROWN);
    drawLabelSeparate((winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight - 75, graphWidth, "SVM", auc5);

    // 按任意键退出
    getchar();
    closegraph();

    // 读取五个RESULT.txt文件中的数据
    vector<vector<double>> results;
    results.push_back(readResults("../../bpNET2/bpNET2/RESULT.txt"));
    results.push_back(readResults("../../knn2/knn2/RESULT.txt"));
    results.push_back(readResults("../../logic2/logic2/RESULT.txt"));
    results.push_back(readResults("../../RTrees2/RTrees2/RESULT.txt"));
    results.push_back(readResults("../../svm/svm/RESULT.txt"));

    // 检查是否成功读取数据
    for (const auto& result : results) {
        if (result.empty()) {
            cerr << "Failed to read one or more RESULT.txt files." << endl;
            return 1;
        }
    }

    // 初始化新的绘图窗口
    initgraph(winWidth, winHeight);

    // 绘制柱状图
    drawBarChart(results, margin, winHeight - margin, winWidth - 2 * margin, winHeight - 2 * margin);

    // 按任意键退出
    getchar();
    closegraph();

    return 0;
}