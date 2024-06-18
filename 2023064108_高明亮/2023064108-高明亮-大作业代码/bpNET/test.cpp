#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <numeric>
#include <iomanip>

using namespace std;

struct RaisinData {
    vector<double> features;
    vector<int> label; // �����Ʊ�ǩ 0 �� 1
};

// ��ȡ����
void readData(const string& filename, vector<RaisinData>& data) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        RaisinData raisin;
        string temp;

        for (int i = 0; i < 7; ++i) {
            getline(ss, temp, ',');
            raisin.features.push_back(stod(temp));
        }

        getline(ss, temp, ',');
        if (temp == "Kecimen") {      //positive
            raisin.label = { 1, 0 }; // ʹ�� one-hot ����
        }
        else if (temp == "Besni") {    //negative
            raisin.label = { 0, 1 }; // ʹ�� one-hot ����
        }

        data.push_back(raisin);
    }
}

// ���ݹ�һ��
void normalizeData(vector<RaisinData>& data) {
    int numFeatures = data[0].features.size();
    vector<double> minVals(numFeatures, numeric_limits<double>::max());
    vector<double> maxVals(numFeatures, numeric_limits<double>::lowest());

    // �ҵ�ÿ����������Сֵ�����ֵ
    for (const auto& raisin : data) {
        for (int i = 0; i < numFeatures; ++i) {
            if (raisin.features[i] < minVals[i]) minVals[i] = raisin.features[i];
            if (raisin.features[i] > maxVals[i]) maxVals[i] = raisin.features[i];
        }
    }

    // ��һ������
    for (auto& raisin : data) {
        for (int i = 0; i < numFeatures; ++i) {
            raisin.features[i] = (raisin.features[i] - minVals[i]) / (maxVals[i] - minVals[i]);
        }
    }
}

class NeuralNetwork {
private:
    int inputSize, hiddenSize, outputSize;
    vector<double> hiddenLayer, outputLayer;
    vector<vector<double>> weightsInputHidden, weightsHiddenOutput;
    vector<double> biasHidden, biasOutput;
    double learningRate;

public:
    NeuralNetwork(int inputSize, int hiddenSize, int outputSize, double learningRate)
        : inputSize(inputSize), hiddenSize(hiddenSize), outputSize(outputSize), learningRate(learningRate) {

        hiddenLayer.resize(hiddenSize);
        outputLayer.resize(outputSize);
        weightsInputHidden.resize(inputSize, vector<double>(hiddenSize));
        weightsHiddenOutput.resize(hiddenSize, vector<double>(outputSize));
        biasHidden.resize(hiddenSize);
        biasOutput.resize(outputSize);

        initializeWeights();
    }

    // ��ʼ��Ȩ��
    void initializeWeights() {
        srand(time(nullptr));
        auto randWeight = []() { return (rand() % 1000 / 1000.0) - 0.5; };

        for (auto& row : weightsInputHidden)
            for (auto& weight : row)
                weight = randWeight();

        for (auto& row : weightsHiddenOutput)
            for (auto& weight : row)
                weight = randWeight();

        for (auto& bias : biasHidden)
            bias = randWeight();

        for (auto& bias : biasOutput)
            bias = randWeight();
    }

    // ǰ�򴫲�
    vector<double> forward(const vector<double>& input) {
        // ���ز�
        for (int j = 0; j < hiddenSize; ++j) {
            hiddenLayer[j] = 0.0;
            for (int i = 0; i < inputSize; ++i)
                hiddenLayer[j] += input[i] * weightsInputHidden[i][j];
            hiddenLayer[j] += biasHidden[j];
            hiddenLayer[j] = sigmoid(hiddenLayer[j]);
        }

        // �����
        for (int k = 0; k < outputSize; ++k) {
            outputLayer[k] = 0.0;
            for (int j = 0; j < hiddenSize; ++j)
                outputLayer[k] += hiddenLayer[j] * weightsHiddenOutput[j][k];
            outputLayer[k] += biasOutput[k];
            outputLayer[k] = sigmoid(outputLayer[k]);
        }

        return outputLayer;
    }

    // ���򴫲�
    void backward(const vector<double>& input, const vector<int>& target) {
        // ������������
        vector<double> outputError(outputSize);
        for (int k = 0; k < outputSize; ++k) {
            outputError[k] = (outputLayer[k] - target[k]) * sigmoidDerivative(outputLayer[k]);
        }

        // �������ز����
        vector<double> hiddenError(hiddenSize);
        for (int j = 0; j < hiddenSize; ++j) {
            hiddenError[j] = 0.0;
            for (int k = 0; k < outputSize; ++k)
                hiddenError[j] += outputError[k] * weightsHiddenOutput[j][k];
            hiddenError[j] *= sigmoidDerivative(hiddenLayer[j]);
        }

        // �������ز㵽������Ȩ�غ�ƫ��
        for (int j = 0; j < hiddenSize; ++j) {
            for (int k = 0; k < outputSize; ++k) {
                weightsHiddenOutput[j][k] -= learningRate * outputError[k] * hiddenLayer[j];
            }
        }

        for (int k = 0; k < outputSize; ++k) {
            biasOutput[k] -= learningRate * outputError[k];
        }

        // ��������㵽���ز��Ȩ�غ�ƫ��
        for (int i = 0; i < inputSize; ++i) {
            for (int j = 0; j < hiddenSize; ++j) {
                weightsInputHidden[i][j] -= learningRate * hiddenError[j] * input[i];
            }
        }

        for (int j = 0; j < hiddenSize; ++j) {
            biasHidden[j] -= learningRate * hiddenError[j];
        }
    }

    // Sigmoid�����
    double sigmoid(double x) {
        return 1.0 / (1.0 + exp(-x));
    }

    // Sigmoid�����ĵ���
    double sigmoidDerivative(double x) {
        return x * (1.0 - x);
    }
};

// ѵ��������
void trainNetwork(NeuralNetwork& nn, vector<RaisinData>& trainData, int epochs) {
    for (int epoch = 0; epoch < epochs; ++epoch) {
        for (const auto& raisin : trainData) {
            nn.forward(raisin.features);
            nn.backward(raisin.features, raisin.label);
        }
    }
}

// ����AUC
double calculateAUC(const vector<double>& predictions, const vector<int>& targets) {
    vector<pair<double, int>> data;
    for (size_t i = 0; i < predictions.size(); ++i) {
        data.push_back(make_pair(predictions[i], targets[i]));
    }

    sort(data.begin(), data.end(), greater<>());

    int posCount = count(targets.begin(), targets.end(), 1);
    int negCount = targets.size() - posCount;

    if (posCount == 0 || negCount == 0) {
        cerr << "Error: One of the classes has zero instances." << endl;
        return 0.0;
    }

    double auc = 0.0;
    int cumPos = 0, cumNeg = 0;

    for (const auto& pair : data) {
        if (pair.second == 1) {
            cumPos++;
        }
        else {
            cumNeg++;
            auc += cumPos;
        }
    }

    auc /= static_cast<double>(posCount * negCount);
    return auc;
}

// ����ROC����
void saveROCCurve(const vector<double>& predictions, const vector<int>& targets, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    file << "FPR,TPR" << endl;

    vector<pair<double, int>> data;
    for (size_t i = 0; i < predictions.size(); ++i) {
        data.push_back(make_pair(predictions[i], targets[i]));
    }

    sort(data.begin(), data.end(), greater<>());

    int posCount = count(targets.begin(), targets.end(), 1);
    int negCount = targets.size() - posCount;

    if (posCount == 0 || negCount == 0) {
        cerr << "Error: One of the classes has zero instances." << endl;
        file.close();
        return;
    }

    int cumPos = 0, cumNeg = 0;

    for (const    auto& pair : data) {
        if (pair.second == 1) {
            cumPos++;
        }
        else {
            cumNeg++;
        }
        double tpr = static_cast<double>(cumPos) / posCount;
        double fpr = static_cast<double>(cumNeg) / negCount;
        file << setprecision(10) << fpr << "," << tpr << endl;
    }

    file.close();
}

// ��������������
void evaluateNetwork(NeuralNetwork& nn, vector<RaisinData>& testData) {
    int truePositives = 0, falsePositives = 0, falseNegatives = 0;
    double totalLoss = 0.0;

    vector<double> predictions;
    vector<int> targets;

    vector<string> classLabels = { "Kecimen", "Besni" };   //positive negative

    int sampleIndex = 1;
    for (const auto& raisin : testData) {
        auto output = nn.forward(raisin.features);
        int predicted = distance(output.begin(), max_element(output.begin(), output.end()));
        int actual = distance(raisin.label.begin(), max_element(raisin.label.begin(), raisin.label.end()));

        predictions.push_back(output[actual]);
        targets.push_back(actual);

        cout << "Sample " << sampleIndex++ << setw(3) << setfill(' ') << " - Actual: " << setw(7) << setfill(' ') << classLabels[actual] << ", Predicted: " << setw(7) << setfill(' ') << classLabels[predicted] << "  ";

        if (classLabels[actual] == classLabels[predicted]) {
            cout << "true";
        }
        else {
            cout << "false";
        }
        cout << endl;

        if (predicted == actual) {
            truePositives++;
        }
        else if (predicted != actual) {
            if (predicted == 1) falsePositives++;
            else falseNegatives++;
        }

        totalLoss += inner_product(output.begin(), output.end(), raisin.label.begin(), 0.0);
    }

    double accuracy = static_cast<double>(truePositives) / testData.size();
    double precision = static_cast<double>(truePositives) / (truePositives + falsePositives);
    double recall = static_cast<double>(truePositives) / (truePositives + falseNegatives);
    double f1 = 2 * (precision * recall) / (precision + recall);

    cout << endl;
    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    // AUC����
    double auc = calculateAUC(predictions, targets);
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

    // ����ROC�������굽�ļ�
    saveROCCurve(predictions, targets, "roc_curve.csv");
}

int main() {
    vector<RaisinData> data;
    readData("Raisin_Dataset.csv", data);  //Raisin_Dataset.csv
    normalizeData(data);

    // //����������ݼ�
    //srand(static_cast<unsigned>(time(nullptr)));
    //random_shuffle(data.begin(), data.end());

    // ����ѵ�����Ͳ��Լ���90%ѵ����10%���ԣ�
    int trainSize = static_cast<int>(data.size() * 0.82);
    vector<RaisinData> trainData(data.begin(), data.begin() + trainSize);
    vector<RaisinData> testData(data.begin() + trainSize, data.end());

    // ��������ݵ�����ѵ������С�Ͳ��Լ���С
    cout << "Total data points: " << data.size() << endl;
    cout << "Training set size: " << trainSize << endl;
    cout << "Test set size: " << data.size() - trainSize << endl << endl;

    NeuralNetwork nn(7, 10, 2, 0.1);
    trainNetwork(nn, trainData, 3000);
    evaluateNetwork(nn, testData);

    return 0;
}