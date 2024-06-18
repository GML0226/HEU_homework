#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>

using namespace std;

// 定义葡萄干数据结构
struct Raisin {
    vector<float> features; // 特征向量
    string label;           // 标签
};

// 加载数据集函数
vector<Raisin> loadDataSet(const string& fileName) {
    ifstream file(fileName);
    string line;
    vector<Raisin> dataSet;

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << fileName << endl;
        return dataSet;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        Raisin raisin;
        while (getline(ss, item, ',')) {
            try {
                raisin.features.push_back(stof(item)); // 将特征添加到向量中
            }
            catch (const invalid_argument&) {
                raisin.label = item; // 将标签保留为字符串
            }
        }
        dataSet.push_back(raisin);
    }

    return dataSet;
}

// 划分数据集函数
void splitDataSet(const vector<Raisin>& dataSet, vector<Raisin>& trainSet, vector<Raisin>& testSet, float trainRatio = 0.82) {
    vector<Raisin> shuffled = dataSet;
    //random_device rd;
    //mt19937 g(rd());
    //shuffle(shuffled.begin(), shuffled.end(), g);

    size_t trainSize = static_cast<size_t>(trainRatio * shuffled.size());
    trainSet.assign(shuffled.begin(), shuffled.begin() + trainSize); // 分配训练集
    testSet.assign(shuffled.begin() + trainSize, shuffled.end());    // 分配测试集
}

// 计算欧几里得距离
// 该函数接收两个浮点数向量a和b，计算并返回它们之间的欧几里得距离。
float euclideanDistance(const vector<float>& a, const vector<float>& b) {
    float sum = 0.0; // 用于存储距离的平方和
    for (size_t i = 0; i < a.size(); ++i) { // 遍历向量的每个元素
        sum += pow(a[i] - b[i], 2); // 计算每个对应元素差的平方，并累加到sum
    }
    return sqrt(sum); // 返回距离，即平方和的平方根
}

// KNN分类评分
// 该函数使用k近邻算法对给定测试实例进行分类，并返回该实例属于"Kecimen"类的比例。
// 参数：
// trainSet: 训练集，包含若干Raisin对象，每个对象具有特征向量和标签
// testInstance: 测试实例的特征向量
// k: KNN算法中的k值，即选择最近邻的数量
float knnClassifyScore(const vector<Raisin>& trainSet, const vector<float>& testInstance, int k) {
    vector<pair<float, string>> distances; // 用于存储每个训练实例到测试实例的距离及其标签

    // 计算每个训练实例到测试实例的欧几里得距离
    for (const auto& instance : trainSet) {
        float distance = euclideanDistance(instance.features, testInstance); // 计算距离
        distances.push_back(make_pair(distance, instance.label)); // 将距离和标签存入向量
    }

    // 按距离升序排序
    sort(distances.begin(), distances.end());

    // 统计前k个最近邻的标签出现次数
    map<string, int> labelCounts; // 用于存储标签及其出现次数
    for (int i = 0; i < k; ++i) { // 遍历前k个最近邻
        labelCounts[distances[i].second]++; // 计数对应标签
    }

    // 返回前k个最近邻中属于"Kecimen"类的比例
    return static_cast<float>(labelCounts["Kecimen"]) / k;
}


void evaluateModel(const vector<Raisin>& testSet, const vector<Raisin>& trainSet, int k, const string& rocFileName) {
    int tp = 0, tn = 0, fp = 0, fn = 0;
    vector<pair<double, int>> scores;
    vector<string> predictions;

    // 计算每个测试样本的预测分数，并记录实际标签
    for (const auto& instance : testSet) {
        float score = knnClassifyScore(trainSet, instance.features, k);
        scores.push_back(make_pair(score, instance.label == "Kecimen" ? 1 : 0));

        string predictedLabel = score >= 0.5 ? "Kecimen" : "Besni";
        predictions.push_back(predictedLabel);

        // 计算混淆矩阵元素
        if (predictedLabel == "Kecimen" && instance.label == "Kecimen") tp++;
        else if (predictedLabel == "Besni" && instance.label == "Besni") tn++;
        else if (predictedLabel == "Kecimen" && instance.label == "Besni") fp++;
        else if (predictedLabel == "Besni" && instance.label == "Kecimen") fn++;
    }

    // 输出每个样本的实际标签和预测标签
    for (size_t i = 0; i < testSet.size(); ++i) {
        cout << "Sample " << i + 1 << setw(3) << setfill(' ') << " - Actual: " << setw(7) << setfill(' ') << testSet[i].label << ", Predicted: " << setw(7) << setfill(' ') << predictions[i] << "     ";

        if (testSet[i].label == predictions[i]) {
            cout << "true";
        }
        else {
            cout << "false";
        }
        cout << endl;
    }

    // 计算评估指标
    double accuracy = static_cast<double>(tp + tn) / testSet.size();
    double precision = tp + fp ? static_cast<double>(tp) / (tp + fp) : 0;
    double recall = tp + fn ? static_cast<double>(tp) / (tp + fn) : 0;
    double f1 = precision + recall ? 2 * (precision * recall) / (precision + recall) : 0;

    cout << endl;
    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    // 排序分数，按分数从高到低排序
    sort(scores.begin(), scores.end(), greater<>());

    // 计算总的正类和负类样本数
    int posCount = 0, negCount = 0;
    for (const auto& score : scores) {
        if (score.second == 1) posCount++;
        else negCount++;
    }

    // 计算ROC曲线
    vector<pair<double, double>> rocPoints;
    int tpROC = 0, fpROC = 0;
    double prevFpr = -1.0;

    for (const auto& score : scores) {
        if (score.second == 1) {
            tpROC++;
        }
        else {
            fpROC++;
        }

        double tpr = static_cast<double>(tpROC) / posCount;
        double fpr = static_cast<double>(fpROC) / negCount;

        // 仅在FPR发生变化时记录点
        if (fpr != prevFpr) {
            rocPoints.push_back(make_pair(fpr, tpr));
            prevFpr = fpr;
        }
    }

    // 计算AUC
    double auc = 0.0;
    for (size_t i = 1; i < rocPoints.size(); ++i) {
        double x1 = rocPoints[i - 1].first;
        double x2 = rocPoints[i].first;
        double y1 = rocPoints[i - 1].second;
        double y2 = rocPoints[i].second;
        auc += (x2 - x1) * (y1 + y2) / 2.0;
    }

    cout << "AUC: " << auc << endl;

    // 保存AUC的值
    ofstream outFile("AUC.txt");
    outFile << auc;
    outFile.close();

    // 保存其余指标
    ofstream outFile2("RESULT.txt");
    outFile2 << accuracy << " " << precision << " " << recall << " " << f1;
    outFile2.close();

    // 将ROC曲线点保存到CSV文件
    ofstream rocFile(rocFileName);
    if (rocFile.is_open()) {
        rocFile << "FPR,TPR\n";
        for (const auto& point : rocPoints) {
            rocFile << point.first << "," << point.second << "\n";
        }
        rocFile.close();
        cout << "ROC points saved to " << rocFileName << endl;
    }
    else {
        cerr << "Error: Unable to open file " << rocFileName << endl;
    }
}

int main() {
    string fileName = "Raisin_Dataset.csv"; // 数据集路径
    string rocFileName = "roc_points.csv"; // ROC曲线点保存路径
    vector<Raisin> dataSet = loadDataSet(fileName);

    if (dataSet.empty()) {
        cerr << "Error: No data loaded from file." << endl;
        return 1;
    }

    vector<Raisin> trainSet, testSet;
    splitDataSet(dataSet, trainSet, testSet); // 划分数据集

    int k = 7; // KNN参数K

    cout << "Total samples: " << dataSet.size() << endl;
    cout << "Training samples: " << trainSet.size() << endl;
    cout << "Testing samples: " << testSet.size() << endl << endl;

    evaluateModel(testSet, trainSet, k, rocFileName); // 评估模型并保存ROC曲线点

    return 0;
}
