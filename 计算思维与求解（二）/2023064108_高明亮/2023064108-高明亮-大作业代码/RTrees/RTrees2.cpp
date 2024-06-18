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

// �������Ѹ����ݽṹ
struct Raisin {
    vector<float> features; // ��������
    string label;           // ��ǩ
};

// �������ݼ�����
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
                raisin.features.push_back(stof(item)); // ��������ӵ�������
            }
            catch (const invalid_argument&) {
                raisin.label = item; // ����ǩ����Ϊ�ַ���
            }
        }
        dataSet.push_back(raisin);
    }

    return dataSet;
}

// �������ݼ�����
void splitDataSet(const vector<Raisin>& dataSet, vector<Raisin>& trainSet, vector<Raisin>& testSet, float trainRatio = 0.82) {
    // random_device rd;
    // mt19937 g(rd());
    // vector<Raisin> shuffledDataSet = dataSet;
    // shuffle(shuffledDataSet.begin(), shuffledDataSet.end(), g); // ����������ݼ�

    size_t trainSize = static_cast<size_t>(trainRatio * dataSet.size());
    trainSet.assign(dataSet.begin(), dataSet.begin() + trainSize); // ����ѵ����
    testSet.assign(dataSet.begin() + trainSize, dataSet.end());    // ������Լ�
}

// ѵ�����ɭ��ģ�ͺ���
void trainRandomForest(const vector<Raisin>& trainSet, Ptr<RTrees>& model) {
    // ����һ��ѵ�����ݾ�������Ϊѵ������С������Ϊ������
    Mat trainData(trainSet.size(), trainSet[0].features.size(), CV_32F);
    // ����һ����ǩ��������Ϊѵ������С������Ϊ1
    Mat trainLabels(trainSet.size(), 1, CV_32S);

    // ���ѵ�����ݺͱ�ǩ
    for (size_t i = 0; i < trainSet.size(); ++i) {
        for (size_t j = 0; j < trainSet[i].features.size(); ++j) {
            // ���������ݸ��Ƶ�ѵ�����ݾ�����
            trainData.at<float>(i, j) = trainSet[i].features[j];
        }
        // ����ǩת��Ϊ���������Ƶ���ǩ������
        trainLabels.at<int>(i, 0) = (trainSet[i].label == "Kecimen") ? 1 : 0;
    }

    // �������ɭ��ģ��
    model = RTrees::create();
    // ����ģ�Ͳ���
    model->setMaxDepth(10); // ��������������
    model->setMinSampleCount(2); // ���ýڵ���ѵ���С������
    model->setRegressionAccuracy(0); // ���ûع龫�ȣ�������������Ϊ0��
    model->setUseSurrogates(false); // ��ʹ���������
    model->setMaxCategories(2); // �������
    model->setPriors(Mat()); // ��ʹ������������
    model->setCalculateVarImportance(true); // ����������Ҫ��
    model->setActiveVarCount(4); // ÿ�η���ʱ���ǵ���������
    // ������ֹ����������������Ϊ1000������Ϊ0.001
    model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 1000, 0.001));

    // ʹ��ѵ�����ݺͱ�ǩѵ�����ɭ��ģ��
    model->train(trainData, ROW_SAMPLE, trainLabels);
}


// ����ģ�ͺ���
void evaluateModel(const vector<Raisin>& testSet, Ptr<RTrees>& model, const string& rocFileName) {
    int tp = 0, tn = 0, fp = 0, fn = 0;
    vector<pair<double, int>> scores;

    for (size_t i = 0; i < testSet.size(); ++i) {
        const auto& instance = testSet[i];
        Mat sampleMat(1, instance.features.size(), CV_32F);
        for (size_t j = 0; j < instance.features.size(); ++j) {
            sampleMat.at<float>(0, j) = instance.features[j]; // �����������
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

        // �����������Ԫ��
        if (response == "Kecimen" && instance.label == "Kecimen") tp++;
        else if (response == "Besni" && instance.label == "Besni") tn++;
        else if (response == "Kecimen" && instance.label == "Besni") fp++;
        else if (response == "Besni" && instance.label == "Kecimen") fn++;

        float score = model->predict(sampleMat, noArray(), StatModel::RAW_OUTPUT); // ��ȡԤ�����
        scores.push_back(make_pair(score, instance.label == "Kecimen" ? 1 : 0));
    }
    cout << endl;

    // ��������ָ��
    double accuracy = static_cast<double>(tp + tn) / testSet.size();
    double precision = static_cast<double>(tp) / (tp + fp);
    double recall = static_cast<double>(tp) / (tp + fn);
    double f1 = 2 * (precision * recall) / (precision + recall);

    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    // ����AUC��ROC����
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
        rocPoints.push_back(make_pair(fpr, tpr)); // ����ROC���ߵ�
    }

    auc /= posCount * negCount;
    cout << "AUC: " << auc << endl;

    //����AUC��ֵ
    ofstream outFile;
    outFile.open("AUC.txt");
    outFile << auc;
    outFile.close();
    //��������ָ��
    ofstream outFile2;
    outFile2.open("RESULT.txt");
    outFile2 << accuracy << " " << precision << " " << recall << " " << f1;
    outFile2.close();

    // ��ROC���ߵ㱣�浽CSV�ļ�
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
    string fileName = "Raisin_Dataset.csv"; // ���ݼ�·��
    string rocFileName = "roc_points.csv"; // ROC���ߵ㱣��·��
    vector<Raisin> dataSet = loadDataSet(fileName);

    if (dataSet.empty()) {
        cerr << "Error: No data loaded from file." << endl;
        return 1;
    }

    vector<Raisin> trainSet, testSet;
    splitDataSet(dataSet, trainSet, testSet); // �������ݼ�

    cout << "Total samples: " << dataSet.size() << endl;
    cout << "Training samples: " << trainSet.size() << endl;
    cout << "Testing samples: " << testSet.size() << endl << endl;

    Ptr<RTrees> model;
    trainRandomForest(trainSet, model); // ѵ�����ɭ��ģ��


    evaluateModel(testSet, model, rocFileName); // ����ģ�Ͳ�����ROC���ߵ�

    return 0;
}