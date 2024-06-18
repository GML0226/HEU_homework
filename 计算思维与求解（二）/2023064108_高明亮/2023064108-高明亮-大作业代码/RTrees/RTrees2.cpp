#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>
#include <random>
#include <algorithm>

using namespace std;
using namespace cv;
using namespace cv::ml;

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
    // random_device rd;
    // mt19937 g(rd());
    // vector<Raisin> shuffledDataSet = dataSet;
    // shuffle(shuffledDataSet.begin(), shuffledDataSet.end(), g); // 随机打乱数据集

    size_t trainSize = static_cast<size_t>(trainRatio * dataSet.size());
    trainSet.assign(dataSet.begin(), dataSet.begin() + trainSize); // 分配训练集
    testSet.assign(dataSet.begin() + trainSize, dataSet.end());    // 分配测试集
}

// 训练随机森林模型函数
void trainRandomForest(const vector<Raisin>& trainSet, Ptr<RTrees>& model) {
    // 创建一个训练数据矩阵，行数为训练集大小，列数为特征数
    Mat trainData(trainSet.size(), trainSet[0].features.size(), CV_32F);
    // 创建一个标签矩阵，行数为训练集大小，列数为1
    Mat trainLabels(trainSet.size(), 1, CV_32S);

    // 填充训练数据和标签
    for (size_t i = 0; i < trainSet.size(); ++i) {
        for (size_t j = 0; j < trainSet[i].features.size(); ++j) {
            // 将特征数据复制到训练数据矩阵中
            trainData.at<float>(i, j) = trainSet[i].features[j];
        }
        // 将标签转换为整数并复制到标签矩阵中
        trainLabels.at<int>(i, 0) = (trainSet[i].label == "Kecimen") ? 1 : 0;
    }

    // 创建随机森林模型
    model = RTrees::create();
    // 设置模型参数
    model->setMaxDepth(10); // 设置树的最大深度
    model->setMinSampleCount(2); // 设置节点分裂的最小样本数
    model->setRegressionAccuracy(0); // 设置回归精度（分类问题设置为0）
    model->setUseSurrogates(false); // 不使用替代分裂
    model->setMaxCategories(2); // 最大类数
    model->setPriors(Mat()); // 不使用先验分类概率
    model->setCalculateVarImportance(true); // 计算特征重要性
    model->setActiveVarCount(4); // 每次分裂时考虑的特征数量
    // 设置终止条件：最大迭代次数为1000，精度为0.001
    model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.001));

    // 使用训练数据和标签训练随机森林模型
    model->train(trainData, ROW_SAMPLE, trainLabels);
}


// 评估模型函数
void evaluateModel(const vector<Raisin>& testSet, Ptr<RTrees>& model, const string& rocFileName) {
    int tp = 0, tn = 0, fp = 0, fn = 0;
    vector<pair<double, int>> scores;

    for (size_t i = 0; i < testSet.size(); ++i) {
        const auto& instance = testSet[i];
        Mat sampleMat(1, instance.features.size(), CV_32F);
        for (size_t j = 0; j < instance.features.size(); ++j) {
            sampleMat.at<float>(0, j) = instance.features[j]; // 填充样本数据
        }

        int responseInt = static_cast<int>(model->predict(sampleMat));
        string response = responseInt == 1 ? "Kecimen" : "Besni";
        cout << "Sample" << i + 1 << setw(3) << setfill(' ') << " - Actual: " << setw(7) << setfill(' ') << instance.label << ", Predicted: " << setw(7) << setfill(' ') << response << "       ";
        if (instance.label == response) {
            cout << "true";
        }
        else {
            cout << "false";
        }
        cout << endl;

        // 计算混淆矩阵元素
        if (response == "Kecimen" && instance.label == "Kecimen") tp++;
        else if (response == "Besni" && instance.label == "Besni") tn++;
        else if (response == "Kecimen" && instance.label == "Besni") fp++;
        else if (response == "Besni" && instance.label == "Kecimen") fn++;

        float score = model->predict(sampleMat, noArray(), StatModel::RAW_OUTPUT); // 获取预测分数
        scores.push_back(make_pair(score, instance.label == "Kecimen" ? 1 : 0));
    }
    cout << endl;

    // 计算评估指标
    double accuracy = static_cast<double>(tp + tn) / testSet.size();
    double precision = static_cast<double>(tp) / (tp + fp);
    double recall = static_cast<double>(tp) / (tp + fn);
    double f1 = 2 * (precision * recall) / (precision + recall);

    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    // 计算AUC和ROC曲线
    sort(scores.begin(), scores.end(), greater<>());

    double auc = 0.0;
    int pos = 0, neg = 0, posCount = 0, negCount = 0;
    for (const auto& score : scores) {
        if (score.second == 1) posCount++;
        else negCount++;
    }

    vector<pair<double, double>> rocPoints;
    for (const auto& score : scores) {
        if (score.second == 1) pos++;
        else {
            neg++;
            auc += pos;
        }
        double tpr = static_cast<double>(pos) / posCount;
        double fpr = static_cast<double>(neg) / negCount;
        rocPoints.push_back(make_pair(fpr, tpr)); // 保存ROC曲线点
    }

    auc /= posCount * negCount;
    cout << "AUC: " << auc << endl;

    //保存AUC的值
    ofstream outFile;
    outFile.open("AUC.txt");
    outFile << auc;
    outFile.close();
    //保存其余指标
    ofstream outFile2;
    outFile2.open("RESULT.txt");
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

    cout << "Total samples: " << dataSet.size() << endl;
    cout << "Training samples: " << trainSet.size() << endl;
    cout << "Testing samples: " << testSet.size() << endl << endl;

    Ptr<RTrees> model;
    trainRandomForest(trainSet, model); // 训练随机森林模型


    evaluateModel(testSet, model, rocFileName); // 评估模型并保存ROC曲线点

    return 0;
}