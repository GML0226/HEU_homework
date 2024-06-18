#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <string>
#include <sstream>
using namespace std;

// 定义数据点结构
struct DataPoint {
    vector<double> features;
    int label;
};

// 读取CSV格式的数据集
vector<DataPoint> readData(const string& filename) {
    ifstream file(filename);
    string line;
    vector<DataPoint> dataset;

    while (getline(file, line)) {
        istringstream ss(line);
        DataPoint dp;
        string value;

        while (getline(ss, value, ',')) {
            dp.features.push_back(stod(value));
        }
        if (dp.features.size() > 0) { // 确保特征不为空
            dp.label = static_cast<int>(dp.features.back());
            dp.features.pop_back();
            dataset.push_back(dp);
        }
    }
    return dataset;
}

// 随机划分数据集
void splitData(const vector<DataPoint>& dataset, vector<DataPoint>& trainSet, vector<DataPoint>& testSet, double trainRatio) {
    // vector<DataPoint> shuffled = dataset;
    // random_device rd;
    // mt19937 g(rd());
    // shuffle(shuffled.begin(), shuffled.end(), g);

    size_t trainSize = static_cast<size_t>(trainRatio * dataset.size());
    trainSet.assign(dataset.begin(), dataset.begin() + trainSize);
    testSet.assign(dataset.begin() + trainSize, dataset.end());
}

// 计算点积
double dotProduct(const vector<double>& a, const vector<double>& b) {
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

// SVM训练
// 该函数使用梯度下降法训练支持向量机（SVM），返回权重向量。
// 参数：
// trainSet: 训练集，包含若干DataPoint对象，每个对象具有特征向量和标签
// learningRate: 学习率，用于控制权重更新的步长
// lambda: 正则化惩罚参数，用于控制过拟合
// epochs: 训练的轮数
vector<double> trainSVM(const vector<DataPoint>& trainSet, double learningRate, double lambda, size_t epochs) {
    if (trainSet.empty()) { // 检查训练集是否为空
        cerr << "Training set is empty!" << endl; // 输出错误信息
        return vector<double>(); // 返回空向量
    }

    size_t featureSize = trainSet[0].features.size(); // 获取特征向量的长度
    vector<double> weights(featureSize, 0.0); // 初始化权重向量为0

    // 进行训练
    for (size_t epoch = 0; epoch < epochs; ++epoch) { // 遍历每个训练轮次
        for (const auto& point : trainSet) { // 遍历训练集中的每个数据点
            double y = point.label == 1 ? 1 : -1; // 将标签转换为+1或-1
            double condition = y * dotProduct(weights, point.features); // 计算y乘以权重和特征的点积
            if (condition < 1) { // 如果条件小于1，更新权重
                for (size_t i = 0; i < featureSize; ++i) {
                    weights[i] += learningRate * ((y * point.features[i]) - (2 * lambda * weights[i]));
                }
            }
            else { // 否则，只应用正则化项更新权重
                for (size_t i = 0; i < featureSize; ++i) {
                    weights[i] += learningRate * (-2 * lambda * weights[i]);
                }
            }
        }
    }
    return weights; // 返回训练好的权重向量
}

// SVM预测（返回决策值）
double predictDecision(const vector<double>& weights, const vector<double>& features) {
    return dotProduct(weights, features);
}

// SVM预测（返回标签）
int predict(const vector<double>& weights, const vector<double>& features) {
    double result = predictDecision(weights, features);
    return result >= 0 ? 1 : 0;
}

// 计算准确率
double calculateAccuracy(const vector<DataPoint>& testSet, const vector<double>& weights) {
    if (testSet.empty()) {
        cerr << "Test set is empty!" << endl;
        return 0.0;
    }

    size_t correct = 0;
    for (const auto& point : testSet) {
        int prediction = predict(weights, point.features);
        if (prediction == point.label) {
            ++correct;
        }
    }
    return static_cast<double>(correct) / testSet.size();
}

// 计算精度，召回率，F1分数
void calculateMetrics(const vector<DataPoint>& testSet, const vector<double>& weights, double& precision, double& recall, double& f1Score) {
    size_t tp = 0, fp = 0, fn = 0, tn = 0;

    for (const auto& point : testSet) {
        int prediction = predict(weights, point.features);
        if (prediction == 1 && point.label == 1) {
            ++tp;
        }
        else if (prediction == 1 && point.label == 0) {
            ++fp;
        }
        else if (prediction == 0 && point.label == 1) {
            ++fn;
        }
        else if (prediction == 0 && point.label == 0) {
            ++tn;
        }
    }

    precision = tp + fp > 0 ? static_cast<double>(tp) / (tp + fp) : 0;
    recall = tp + fn > 0 ? static_cast<double>(tp) / (tp + fn) : 0;
    f1Score = precision + recall > 0 ? 2 * (precision * recall) / (precision + recall) : 0;
}

// 计算AUC和生成ROC曲线点
double calculateAUC(const vector<DataPoint>& testSet, const vector<double>& weights, const string& rocFilename) {
    vector<pair<double, int>> scores;
    for (const auto& point : testSet) {
        double decisionValue = predictDecision(weights, point.features);
        scores.push_back({ decisionValue, point.label });
    }

    sort(scores.begin(), scores.end(), greater<pair<double, int>>());

    size_t tp = 0, fp = 0, fn = 0, tn = 0;
    for (const auto& point : testSet) {
        if (point.label == 1) {
            ++fn;
        }
        else {
            ++tn;
        }
    }

    ofstream rocFile(rocFilename);
    if (!rocFile.is_open()) {
        cerr << "Failed to open file for ROC curve!" << endl;
        return -1;
    }

    double prevFPR = 0.0, prevTPR = 0.0, auc = 0.0;
    //rocFile << "FPR,TPR\n";
    rocFile << "0.0,0.0\n";

    for (const auto& score : scores) {
        if (score.second == 1) {
            ++tp;
            --fn;
        }
        else {
            ++fp;
            --tn;
        }

        double tpr = tp + fn > 0 ? static_cast<double>(tp) / (tp + fn) : 0;
        double fpr = fp + tn > 0 ? static_cast<double>(fp) / (fp + tn) : 0;

        auc += (fpr - prevFPR) * (tpr + prevTPR) / 2.0;

        rocFile << fpr << "," << tpr << "\n";

        prevFPR = fpr;
        prevTPR = tpr;
    }

    rocFile.close();
    return auc;
}



// 主函数
int main() {
    string filename = "Raisin_Dataset 2.csv";  // 确保文件路径正确  （Heart Attack 2.csv）(iris3.csv)
    vector<DataPoint> dataset = readData(filename);

    // Debug: 输出读取的数据点数
    cout << "Total data points: " << dataset.size() << endl;

    if (dataset.empty()) {
        cerr << "Dataset is empty!" << endl;
        return -1;
    }

    // Check if all data points have the same number of features
    size_t expectedFeatureSize = dataset[0].features.size();
    for (const auto& dp : dataset) {
        if (dp.features.size() != expectedFeatureSize) {
            cerr << "Inconsistent feature sizes in the dataset!" << endl;
            return -1;
        }
    }

    vector<DataPoint> trainSet, testSet;
    splitData(dataset, trainSet, testSet, 0.82);

    // Debug: 输出训练集和测试集数据点数
    cout << "Training set size: " << trainSet.size() << endl;
    cout << "Test set size: " << testSet.size() << endl << endl;

    if (trainSet.empty() || testSet.empty()) {
        cerr << "Train or Test set is empty after split!" << endl;
        return -1;
    }

    double learningRate = 0.0001;
    double lambda = 0.0001;
    size_t epochs = 30000;

    vector<double> weights = trainSVM(trainSet, learningRate, lambda, epochs);

    // Debug: 检查训练是否成功
    if (weights.empty()) {
        cerr << "Training failed, weights vector is empty!" << endl;
        return -1;
    }

    // 输出验证集每个样本的分类结果
    cout << "\nValidation Set Predictions:\n";
    int sampleIndex = 1;
    for (const auto& point : testSet) {
        int prediction = predict(weights, point.features);
        cout << "Sample" << sampleIndex << " - True Label: " << point.label << ", Predicted Label: " << prediction << "     ";
        ++sampleIndex;
        if (point.label == prediction) {
            cout << "true";
        }
        else {
            cout << "false";
        }
        cout << endl;
    }
    cout << endl;

    double accuracy = calculateAccuracy(testSet, weights);
    cout << "Accuracy: " << accuracy << endl;

    // 计算精度，召回率和F1分数
    double precision, recall, f1Score;
    calculateMetrics(testSet, weights, precision, recall, f1Score);
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1Score << endl;

    // 计算AUC和生成ROC曲线点
    string rocFilename = "roc_curve.csv";
    double auc = calculateAUC(testSet, weights, rocFilename);
    cout << "AUC: " << auc << endl;
    //保存AUC的值
    ofstream outFile;
    outFile.open("AUC.txt");
    outFile << auc;
    outFile.close();
    //保存其余指标
    ofstream outFile2;
    outFile2.open("RESULT.txt");
    outFile2 << accuracy << " " << precision << " " << recall << " " << f1Score;
    outFile2.close();

    return 0;
}
