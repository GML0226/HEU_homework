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

// �������ݵ�ṹ
struct DataPoint {
    vector<double> features;
    int label;
};

// ��ȡCSV��ʽ�����ݼ�
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
        if (dp.features.size() > 0) { // ȷ��������Ϊ��
            dp.label = static_cast<int>(dp.features.back());
            dp.features.pop_back();
            dataset.push_back(dp);
        }
    }
    return dataset;
}

// ����������ݼ�
void splitData(const vector<DataPoint>& dataset, vector<DataPoint>& trainSet, vector<DataPoint>& testSet, double trainRatio) {
    // vector<DataPoint> shuffled = dataset;
    // random_device rd;
    // mt19937 g(rd());
    // shuffle(shuffled.begin(), shuffled.end(), g);

    size_t trainSize = static_cast<size_t>(trainRatio * dataset.size());
    trainSet.assign(dataset.begin(), dataset.begin() + trainSize);
    testSet.assign(dataset.begin() + trainSize, dataset.end());
}

// ������
double dotProduct(const vector<double>& a, const vector<double>& b) {
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

// SVMѵ��
// �ú���ʹ���ݶ��½���ѵ��֧����������SVM��������Ȩ��������
// ������
// trainSet: ѵ��������������DataPoint����ÿ������������������ͱ�ǩ
// learningRate: ѧϰ�ʣ����ڿ���Ȩ�ظ��µĲ���
// lambda: ���򻯳ͷ����������ڿ��ƹ����
// epochs: ѵ��������
vector<double> trainSVM(const vector<DataPoint>& trainSet, double learningRate, double lambda, size_t epochs) {
    if (trainSet.empty()) { // ���ѵ�����Ƿ�Ϊ��
        cerr << "Training set is empty!" << endl; // ���������Ϣ
        return vector<double>(); // ���ؿ�����
    }

    size_t featureSize = trainSet[0].features.size(); // ��ȡ���������ĳ���
    vector<double> weights(featureSize, 0.0); // ��ʼ��Ȩ������Ϊ0

    // ����ѵ��
    for (size_t epoch = 0; epoch < epochs; ++epoch) { // ����ÿ��ѵ���ִ�
        for (const auto& point : trainSet) { // ����ѵ�����е�ÿ�����ݵ�
            double y = point.label == 1 ? 1 : -1; // ����ǩת��Ϊ+1��-1
            double condition = y * dotProduct(weights, point.features); // ����y����Ȩ�غ������ĵ��
            if (condition < 1) { // �������С��1������Ȩ��
                for (size_t i = 0; i < featureSize; ++i) {
                    weights[i] += learningRate * ((y * point.features[i]) - (2 * lambda * weights[i]));
                }
            }
            else { // ����ֻӦ�����������Ȩ��
                for (size_t i = 0; i < featureSize; ++i) {
                    weights[i] += learningRate * (-2 * lambda * weights[i]);
                }
            }
        }
    }
    return weights; // ����ѵ���õ�Ȩ������
}

// SVMԤ�⣨���ؾ���ֵ��
double predictDecision(const vector<double>& weights, const vector<double>& features) {
    return dotProduct(weights, features);
}

// SVMԤ�⣨���ر�ǩ��
int predict(const vector<double>& weights, const vector<double>& features) {
    double result = predictDecision(weights, features);
    return result >= 0 ? 1 : 0;
}

// ����׼ȷ��
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

// ���㾫�ȣ��ٻ��ʣ�F1����
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

// ����AUC������ROC���ߵ�
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



// ������
int main() {
    string filename = "Raisin_Dataset 2.csv";  // ȷ���ļ�·����ȷ  ��Heart Attack 2.csv��(iris3.csv)
    vector<DataPoint> dataset = readData(filename);

    // Debug: �����ȡ�����ݵ���
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

    // Debug: ���ѵ�����Ͳ��Լ����ݵ���
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

    // Debug: ���ѵ���Ƿ�ɹ�
    if (weights.empty()) {
        cerr << "Training failed, weights vector is empty!" << endl;
        return -1;
    }

    // �����֤��ÿ�������ķ�����
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

    // ���㾫�ȣ��ٻ��ʺ�F1����
    double precision, recall, f1Score;
    calculateMetrics(testSet, weights, precision, recall, f1Score);
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1Score << endl;

    // ����AUC������ROC���ߵ�
    string rocFilename = "roc_curve.csv";
    double auc = calculateAUC(testSet, weights, rocFilename);
    cout << "AUC: " << auc << endl;
    //����AUC��ֵ
    ofstream outFile;
    outFile.open("AUC.txt");
    outFile << auc;
    outFile.close();
    //��������ָ��
    ofstream outFile2;
    outFile2.open("RESULT.txt");
    outFile2 << accuracy << " " << precision << " " << recall << " " << f1Score;
    outFile2.close();

    return 0;
}
