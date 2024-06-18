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
    vector<Raisin> shuffled = dataSet;
    //random_device rd;
    //mt19937 g(rd());
    //shuffle(shuffled.begin(), shuffled.end(), g);

    size_t trainSize = static_cast<size_t>(trainRatio * shuffled.size());
    trainSet.assign(shuffled.begin(), shuffled.begin() + trainSize); // ����ѵ����
    testSet.assign(shuffled.begin() + trainSize, shuffled.end());    // ������Լ�
}

// ����ŷ����þ���
// �ú���������������������a��b�����㲢��������֮���ŷ����þ��롣
float euclideanDistance(const vector<float>& a, const vector<float>& b) {
    float sum = 0.0; // ���ڴ洢�����ƽ����
    for (size_t i = 0; i < a.size(); ++i) { // ����������ÿ��Ԫ��
        sum += pow(a[i] - b[i], 2); // ����ÿ����ӦԪ�ز��ƽ�������ۼӵ�sum
    }
    return sqrt(sum); // ���ؾ��룬��ƽ���͵�ƽ����
}

// KNN��������
// �ú���ʹ��k�����㷨�Ը�������ʵ�����з��࣬�����ظ�ʵ������"Kecimen"��ı�����
// ������
// trainSet: ѵ��������������Raisin����ÿ������������������ͱ�ǩ
// testInstance: ����ʵ������������
// k: KNN�㷨�е�kֵ����ѡ������ڵ�����
float knnClassifyScore(const vector<Raisin>& trainSet, const vector<float>& testInstance, int k) {
    vector<pair<float, string>> distances; // ���ڴ洢ÿ��ѵ��ʵ��������ʵ���ľ��뼰���ǩ

    // ����ÿ��ѵ��ʵ��������ʵ����ŷ����þ���
    for (const auto& instance : trainSet) {
        float distance = euclideanDistance(instance.features, testInstance); // �������
        distances.push_back(make_pair(distance, instance.label)); // ������ͱ�ǩ��������
    }

    // ��������������
    sort(distances.begin(), distances.end());

    // ͳ��ǰk������ڵı�ǩ���ִ���
    map<string, int> labelCounts; // ���ڴ洢��ǩ������ִ���
    for (int i = 0; i < k; ++i) { // ����ǰk�������
        labelCounts[distances[i].second]++; // ������Ӧ��ǩ
    }

    // ����ǰk�������������"Kecimen"��ı���
    return static_cast<float>(labelCounts["Kecimen"]) / k;
}


void evaluateModel(const vector<Raisin>& testSet, const vector<Raisin>& trainSet, int k, const string& rocFileName) {
    int tp = 0, tn = 0, fp = 0, fn = 0;
    vector<pair<double, int>> scores;
    vector<string> predictions;

    // ����ÿ������������Ԥ�����������¼ʵ�ʱ�ǩ
    for (const auto& instance : testSet) {
        float score = knnClassifyScore(trainSet, instance.features, k);
        scores.push_back(make_pair(score, instance.label == "Kecimen" ? 1 : 0));

        string predictedLabel = score >= 0.5 ? "Kecimen" : "Besni";
        predictions.push_back(predictedLabel);

        // �����������Ԫ��
        if (predictedLabel == "Kecimen" && instance.label == "Kecimen") tp++;
        else if (predictedLabel == "Besni" && instance.label == "Besni") tn++;
        else if (predictedLabel == "Kecimen" && instance.label == "Besni") fp++;
        else if (predictedLabel == "Besni" && instance.label == "Kecimen") fn++;
    }

    // ���ÿ��������ʵ�ʱ�ǩ��Ԥ���ǩ
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

    // ��������ָ��
    double accuracy = static_cast<double>(tp + tn) / testSet.size();
    double precision = tp + fp ? static_cast<double>(tp) / (tp + fp) : 0;
    double recall = tp + fn ? static_cast<double>(tp) / (tp + fn) : 0;
    double f1 = precision + recall ? 2 * (precision * recall) / (precision + recall) : 0;

    cout << endl;
    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    // ����������������Ӹߵ�������
    sort(scores.begin(), scores.end(), greater<>());

    // �����ܵ�����͸���������
    int posCount = 0, negCount = 0;
    for (const auto& score : scores) {
        if (score.second == 1) posCount++;
        else negCount++;
    }

    // ����ROC����
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

        // ����FPR�����仯ʱ��¼��
        if (fpr != prevFpr) {
            rocPoints.push_back(make_pair(fpr, tpr));
            prevFpr = fpr;
        }
    }

    // ����AUC
    double auc = 0.0;
    for (size_t i = 1; i < rocPoints.size(); ++i) {
        double x1 = rocPoints[i - 1].first;
        double x2 = rocPoints[i].first;
        double y1 = rocPoints[i - 1].second;
        double y2 = rocPoints[i].second;
        auc += (x2 - x1) * (y1 + y2) / 2.0;
    }

    cout << "AUC: " << auc << endl;

    // ����AUC��ֵ
    ofstream outFile("AUC.txt");
    outFile << auc;
    outFile.close();

    // ��������ָ��
    ofstream outFile2("RESULT.txt");
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

    int k = 7; // KNN����K

    cout << "Total samples: " << dataSet.size() << endl;
    cout << "Training samples: " << trainSet.size() << endl;
    cout << "Testing samples: " << testSet.size() << endl << endl;

    evaluateModel(testSet, trainSet, k, rocFileName); // ����ģ�Ͳ�����ROC���ߵ�

    return 0;
}
