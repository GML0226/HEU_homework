#define _CRT_SECURE_NO_WARNINGS
#include <graphics.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip> // �������ڸ�ʽ�������ͷ�ļ�
#include <tchar.h> // ���� TCHAR ��ͷ�ļ�

using namespace std;

// �ṹ���ʾһ����
struct Point {
    double x;
    double y;
};

// ��ȡCSV�ļ������ذ������е������
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

// ��ȡTXT�ļ��еĸ�����ֵ
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

// ��ȡRESULT.txt�ļ��е��ĸ�������
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

// ����������Ϳ̶�
void drawAxesSeparate(int offsetX, int offsetY, int width, int height) {
    setcolor(WHITE); // ������ɫΪ��ɫ
    // ����X��
    line(offsetX, offsetY, offsetX + width, offsetY);
    // ����Y��
    line(offsetX, offsetY, offsetX, offsetY - height);

    // ����X��̶Ⱥ���ֵ
    for (int i = 0; i <= 10; ++i) {
        int x = offsetX + i * (width / 10);
        line(x, offsetY, x, offsetY + 5);

        // ������ֵ��ת��Ϊ�ַ���
        double value = i * 0.1;
        stringstream ss;
        ss << fixed << setprecision(1) << value;
        string strValue = ss.str();

        // ת�� std::string Ϊ TCHAR*
        size_t size = strValue.size() + 1;
        TCHAR* tcharValue = new TCHAR[size];
        mbstowcs(tcharValue, strValue.c_str(), size);

        // ������ֵ
        outtextxy(x - textwidth(tcharValue) / 2, offsetY + 10, tcharValue);
        delete[] tcharValue;
    }

    // ����Y��̶Ⱥ���ֵ
    for (int i = 0; i <= 5; ++i) {
        int y = offsetY - i * (height / 5);
        line(offsetX, y, offsetX - 10, y);

        // ������ֵ��ת��Ϊ�ַ���
        double value = i * 0.2;
        stringstream ss;
        ss << fixed << setprecision(1) << value;
        string strValue = ss.str();

        // ת�� std::string Ϊ TCHAR*
        size_t size = strValue.size() + 1;
        TCHAR* tcharValue = new TCHAR[size];
        mbstowcs(tcharValue, strValue.c_str(), size);

        // ������ֵ
        outtextxy(offsetX - textwidth(tcharValue) - 10, y - textheight(tcharValue) / 2, tcharValue);
        delete[] tcharValue;
    }
}

// ��������
void drawCurveSeparate(const vector<Point>& points, int offsetX, int offsetY, int width, int height, COLORREF color) {
    if (points.empty()) {
        cerr << "No points to draw." << endl;
        return;
    }

    setcolor(color); // ����������ɫ
    moveto(offsetX + points[0].x * width, offsetY - points[0].y * height);
    for (const auto& point : points) {
        lineto(offsetX + point.x * width, offsetY - point.y * height);
    }
}

// ��ÿ�������·��������ֱ�ע
void drawLabelSeparate(int offsetX, int offsetY, int width, const string& label, double auc) {
    settextstyle(20, 0, _T("Arial"));

    // ��������AUC��ֵ�ı�ǩ
    stringstream ss;
    ss << label << " AUC=" << fixed << setprecision(6) << auc;
    string fullLabel = ss.str();

    // �� std::string ת��Ϊ TCHAR*
    size_t size = fullLabel.size() + 1;
    TCHAR* tcharLabel = new TCHAR[size];
    mbstowcs(tcharLabel, fullLabel.c_str(), size);

    outtextxy(offsetX + width / 2 - textwidth(tcharLabel) / 2, offsetY + 10, tcharLabel);

    delete[] tcharLabel;
}

void drawBarChart(const vector<vector<double>>& results, int offsetX, int offsetY, int width, int height) {
    int numBars = results.size();
    int numGroups = results[0].size();
    int totalSpacing = (numBars + 1) * numGroups; // �ܼ����
    int barWidth = width / (numBars * numGroups + totalSpacing); // ���¼��������
    int barSpacing = barWidth; // ������֮��ļ�����������

    // �����ǩ����
    const char* labels[] = { "Accuracy", "Precision", "Recall", "F1 Score" };

    // ������ɫ����
    int colors[] = { LIGHTMAGENTA, GREEN, YELLOW, LIGHTCYAN, BROWN };

    // ��������ǩ����
    const char* barLabels[] = { "bpNET", "KNN", "Logic", "Rtrees", "SVM" };

    for (int i = 0; i < numGroups; ++i) {
        for (int j = 0; j < numBars; ++j) {
            int x = offsetX + (i * (numBars + 1) + j) * (barWidth + barSpacing);
            int barHeight = static_cast<int>(results[j][i] * height);
            setfillcolor(colors[j % 5]); // ʹ�ò�ͬ����ɫ
            bar(x, offsetY - barHeight, x + barWidth, offsetY);

            // ���������϶˱����ֵ
            stringstream ss;
            ss << fixed << setprecision(2) << results[j][i];
            string valueStr = ss.str();

            // �� std::string ת��Ϊ TCHAR*
            size_t size = valueStr.size() + 1;
            TCHAR* tcharValue = new TCHAR[size];
            mbstowcs(tcharValue, valueStr.c_str(), size);

            outtextxy(x + barWidth / 2 - textwidth(tcharValue) / 2, offsetY - barHeight - textheight(tcharValue) - 5, tcharValue);
            delete[] tcharValue;

            // �������ڲ�ͬһ�߶ȱ����ǩ
            const char* barLabel = barLabels[j];
            size = strlen(barLabel) + 1;
            TCHAR* tcharBarLabel = new TCHAR[size];
            mbstowcs(tcharBarLabel, barLabel, size);

            outtextxy(x + barWidth / 2 - textwidth(tcharBarLabel) / 2, 750, tcharBarLabel);
            delete[] tcharBarLabel;
        }

        // �������ǩ
        const char* label = labels[i];
        size_t size = strlen(label) + 1;
        TCHAR* tcharLabel = new TCHAR[size];
        mbstowcs(tcharLabel, label, size);
        outtextxy(offsetX + (i * (numBars + 1) + numBars / 2) * (barWidth + barSpacing) - textwidth(tcharLabel) / 2, offsetY + 25, tcharLabel);
        delete[] tcharLabel;
    }
}

int main() {
    // ��ʼ��EasyXͼ�δ���
    int winWidth = 1000;
    int winHeight = 800;
    initgraph(winWidth, winHeight);

    // ��ȡCSV�ļ��е�����
    vector<Point> points1 = readCSV("../../bpNET2/bpNET2/roc_curve.csv");
    vector<Point> points2 = readCSV("../../knn2/knn2/roc_points.csv");
    vector<Point> points3 = readCSV("../../logic2/logic2/roc_curve_points.csv");
    vector<Point> points4 = readCSV("../../RTrees2/RTrees2/roc_points.csv");
    vector<Point> points5 = readCSV("../../svm/svm/roc_curve.csv");

    // ��ȡTXT�ļ��е�AUCֵ
    double auc1 = readAUC("../../bpNET2/bpNET2/AUC.txt");
    double auc2 = readAUC("../../knn2/knn2/AUC.txt");
    double auc3 = readAUC("../../logic2/logic2/AUC.txt");
    double auc4 = readAUC("../../RTrees2/RTrees2/AUC.txt");
    double auc5 = readAUC("../../svm/svm/AUC.txt");

    // ����Ƿ�ɹ���ȡ����
    if (points1.empty() || points2.empty() || points3.empty() || points4.empty() || points5.empty()) {
        cerr << "Failed to read one or more CSV files." << endl;
        closegraph();
        return 1;
    }

    // ����ÿ�������ڲ�ͬ������
    int margin = 50;
    int graphWidth = (winWidth - 3 * margin) / 2;
    int graphHeight = (winHeight - 4 * margin) / 3;


    // ��1������ (����)
    drawAxesSeparate(margin, margin + graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points1, margin, margin + graphHeight, graphWidth, graphHeight, LIGHTMAGENTA);
    drawLabelSeparate(margin, margin + graphHeight - 75, graphWidth, "bpNET", auc1);

    // ��2������ (����)
    drawAxesSeparate(2 * margin + graphWidth, margin + graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points2, 2 * margin + graphWidth, margin + graphHeight, graphWidth, graphHeight, GREEN);
    drawLabelSeparate(2 * margin + graphWidth, margin + graphHeight - 75, graphWidth, "KNN", auc2);

    // ��3������ (����)
    drawAxesSeparate(margin, 2 * margin + 2 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points3, margin, 2 * margin + 2 * graphHeight, graphWidth, graphHeight, YELLOW);
    drawLabelSeparate(margin, 2 * margin + 2 * graphHeight - 75, graphWidth, "Logic", auc3);

    // ��4������ (����)
    drawAxesSeparate(2 * margin + graphWidth, 2 * margin + 2 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points4, 2 * margin + graphWidth, 2 * margin + 2 * graphHeight, graphWidth, graphHeight, LIGHTCYAN);
    drawLabelSeparate(2 * margin + graphWidth, 2 * margin + 2 * graphHeight - 75, graphWidth, "RTrees", auc4);

    // ��5������ (�ײ�����)
    drawAxesSeparate((winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight, graphWidth, graphHeight);
    drawCurveSeparate(points5, (winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight, graphWidth, graphHeight, BROWN);
    drawLabelSeparate((winWidth - graphWidth) / 2, 3 * margin + 3 * graphHeight - 75, graphWidth, "SVM", auc5);

    // ��������˳�
    getchar();
    closegraph();

    // ��ȡ���RESULT.txt�ļ��е�����
    vector<vector<double>> results;
    results.push_back(readResults("../../bpNET2/bpNET2/RESULT.txt"));
    results.push_back(readResults("../../knn2/knn2/RESULT.txt"));
    results.push_back(readResults("../../logic2/logic2/RESULT.txt"));
    results.push_back(readResults("../../RTrees2/RTrees2/RESULT.txt"));
    results.push_back(readResults("../../svm/svm/RESULT.txt"));

    // ����Ƿ�ɹ���ȡ����
    for (const auto& result : results) {
        if (result.empty()) {
            cerr << "Failed to read one or more RESULT.txt files." << endl;
            return 1;
        }
    }

    // ��ʼ���µĻ�ͼ����
    initgraph(winWidth, winHeight);

    // ������״ͼ
    drawBarChart(results, margin, winHeight - margin, winWidth - 2 * margin, winHeight - 2 * margin);

    // ��������˳�
    getchar();
    closegraph();

    return 0;
}