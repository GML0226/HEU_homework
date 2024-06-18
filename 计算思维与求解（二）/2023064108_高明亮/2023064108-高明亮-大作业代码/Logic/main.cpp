#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;

// 定义葡萄干数据结构
struct RaisinData {
    double area;
    double perimeter;
    double major_axis_length;
    double minor_axis_length;
    double eccentricity;
    double convex_area;
    double extent;
    int label;
};

// 定义逻辑回归模型类
class LogisticRegression {
private:
    vector<double> weights; // 权重向量

public:
    // 构造函数，初始化权重
    LogisticRegression(int num_features) {
         //随机初始化权重
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(-1, 1); // 随机初始化范围为[-1, 1]
        for (int i = 0; i < num_features; ++i) {
            double weight = dis(gen);
            weights.push_back(weight);
            cout << "Initial weight for feature " << i + 1 << ": " << weight << endl;
        }
    }

    // Sigmoid 函数
    double sigmoid(double z) {
        return 1 / (1 + exp(-z));
    }

    // 训练模型
    void train(vector<vector<double>>& X, vector<int>& y, double learning_rate, int num_iterations) {
        int m = X.size(); // 样本数量
        int n = weights.size(); // 特征数量

        for (int iter = 0; iter < num_iterations; iter++) {
            for (int i = 0; i < m; i++) {
                double z = 0.0;
                for (int j = 0; j < n; j++) {
                    z += weights[j] * X[i][j]; // 计算线性组合
                }
                double h = sigmoid(z); // 应用 Sigmoid 函数
                double error = h - y[i]; // 计算误差
                for (int j = 0; j < n; j++) {
                    weights[j] -= (learning_rate * error * X[i][j]) / m; // 更新权重
                }
            }
        }
    }

    // 预测概率
    double predict_proba(vector<double>& X) {
        double z = 0.0;
        for (size_t i = 0; i < X.size(); i++) {
            z += weights[i] * X[i]; // 计算线性组合
        }
        return sigmoid(z); // 应用 Sigmoid 函数
    }

    // 预测
    int predict(vector<double>& X) {
        return (predict_proba(X) >= 0.5) ? 1 : 0; // 阈值为0.5
    }
};

// 加载葡萄干数据集
vector<RaisinData> loadRaisinDataset(const string& filename) {
    vector<RaisinData> dataset;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        RaisinData data;
        char comma;
        string label;
        ss >> data.area >> comma >> data.perimeter >> comma >>
            data.major_axis_length >> comma >> data.minor_axis_length >> comma >>
            data.eccentricity >> comma >> data.convex_area >> comma >>
            data.extent >> comma >> label;
        data.label = (label == "Kecimen") ? 1 : 0; // 假设我们分类Kecimen和Besni
        dataset.push_back(data);
    }
    file.close();
    return dataset;
}

// 分割数据集为训练集和测试集
void splitDataset(vector<RaisinData>& dataset, vector<vector<double>>& X_train, vector<int>& y_train, vector<vector<double>>& X_test, vector<int>& y_test, double train_size = 0.82) {
    //random_device rd;
    //mt19937 g(rd());
    //shuffle(dataset.begin(), dataset.end(), g); // 打乱数据集

    size_t train_size_index = static_cast<size_t>(dataset.size() * train_size); // 训练集大小
    for (size_t i = 0; i < dataset.size(); ++i) {
        vector<double> features = { dataset[i].area, dataset[i].perimeter, dataset[i].major_axis_length, dataset[i].minor_axis_length, dataset[i].eccentricity, dataset[i].convex_area, dataset[i].extent };
        if (i < train_size_index) {
            X_train.push_back(features);
            y_train.push_back(dataset[i].label);
        }
        else {
            X_test.push_back(features);
            y_test.push_back(dataset[i].label);
        }
    }

    cout << "Total data points: " << dataset.size() << endl;
    cout << "Training set size: " << X_train.size() << endl;
    cout << "Test set size: " << X_test.size() << endl << endl;
}

// 计算评估指标
// 计算 AUC
double calculateAUC(const vector<pair<double, double>>& roc_points) {
    double auc = 0.0;
    double prev_fpr = 0.0;
    double prev_tpr = 0.0;

    for (const auto& point : roc_points) {
        double fpr = point.first;
        double tpr = point.second;

        auc += (fpr - prev_fpr) * (tpr + prev_tpr) / 2.0;

        prev_fpr = fpr;
        prev_tpr = tpr;
    }

    return auc;
}

// 计算 ROC 曲线的指标
vector<pair<double, double>> calculateROC(vector<double>& y_probs, vector<int>& y_test) {
    vector<pair<double, double>> roc_points;

    vector<pair<double, int>> sorted_probs;
    for (size_t i = 0; i < y_probs.size(); ++i) {
        sorted_probs.push_back(make_pair(y_probs[i], y_test[i]));
    }
    sort(sorted_probs.begin(), sorted_probs.end(), greater<pair<double, int>>()); // 按概率降序排序

    int num_positive = count(y_test.begin(), y_test.end(), 1); // 正样本数量
    int num_negative = y_test.size() - num_positive; // 负样本数量

    int tp = 0, fp = 0;

    for (const auto& prob_label : sorted_probs) {
        if (prob_label.second == 1) {
            tp++;
        }
        else {
            fp++;
        }

        double tpr_value = static_cast<double>(tp) / num_positive; // 计算 TPR
        double fpr_value = static_cast<double>(fp) / num_negative; // 计算 FPR

        roc_points.push_back(make_pair(fpr_value, tpr_value)); // 保存ROC点
    }

    return roc_points;
}

// 评估模型
vector<double> evaluateModel(LogisticRegression& model, vector<vector<double>>& X_test, vector<int>& y_test) {
    int true_positive = 0, true_negative = 0, false_positive = 0, false_negative = 0;
    vector<double> y_probs;

    for (size_t i = 0; i < X_test.size(); i++) {
        int prediction = model.predict(X_test[i]);
        double proba = model.predict_proba(X_test[i]);
        y_probs.push_back(proba);

        // 显示每个样本的分类结果
        cout << "Sample " << i + 1 << " - Actual: " << y_test[i] << ", Predicted: " << prediction << ", Probability: " << proba << endl;

        if (prediction == 1 && y_test[i] == 1) true_positive++;
        if (prediction == 0 && y_test[i] == 0) true_negative++;
        if (prediction == 1 && y_test[i] == 0) false_positive++;
        if (prediction == 0 && y_test[i] == 1) false_negative++;
    }

    double accuracy = (true_positive + true_negative) / static_cast<double>(X_test.size());
    double precision = true_positive / static_cast<double>(true_positive + false_positive);
    double recall = true_positive / static_cast<double>(true_positive + false_negative);
    double f1 = 2 * (precision * recall) / (precision + recall);
    cout << endl;
    cout << "Accuracy: " << accuracy << endl;
    cout << "Precision: " << precision << endl;
    cout << "Recall: " << recall << endl;
    cout << "F1 Score: " << f1 << endl;

    vector<pair<double, double>> roc_points = calculateROC(y_probs, y_test);

    // 计算 AUC
    double auc = calculateAUC(roc_points);
    cout << "AUC: " << auc << endl;
    cout << endl;

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

    return y_probs;
}

// 保存 ROC 曲线上的点到 CSV 文件
void saveROCCurvePoints(const string& filename, const vector<pair<double, double>>& roc_points) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to create file: " << filename << endl;
        return;
    }

    file << "FPR,TPR" << endl;
    for (const auto& point : roc_points) {
        file << point.first << "," << point.second << endl;
    }

    file.close();
    cout << "ROC curve points saved to file: " << filename << endl;
}

int main() {
    // 加载数据集
    vector<RaisinData> raisin_data = loadRaisinDataset("Raisin_Dataset.csv");

    // 划分数据集
    vector<vector<double>> X_train, X_test;
    vector<int> y_train, y_test;
    splitDataset(raisin_data, X_train, y_train, X_test, y_test);

    // 初始化并训练逻辑回归模型
    LogisticRegression model(7);
    model.train(X_train, y_train, 0.00001, 40000); //学习率 迭代次数

    // 评估模型
    vector<double> y_probs = evaluateModel(model, X_test, y_test);

    // 计算 ROC 曲线
    vector<pair<double, double>> roc_points = calculateROC(y_probs, y_test);

    // 保存 ROC 曲线上的点到 CSV 文件
    saveROCCurvePoints("roc_curve_points.csv", roc_points);

    return 0;
}
