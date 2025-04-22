#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

struct SampleData {
    string name;
    Mat image;
    vector<KeyPoint> keypoints;
    Mat descriptors;
};

// Ratio test для фильтрации совпадений
vector<DMatch> ratio_test(const vector<vector<DMatch>>& matches12, double ratio) {
    vector<DMatch> good_matches;
    for (size_t i = 0; i < matches12.size(); i++) {
        if (matches12[i].size() >= 2 && matches12[i][0].distance < ratio * matches12[i][1].distance)
            good_matches.push_back(matches12[i][0]);
    }
    return good_matches;
}

int main() {
    vector<SampleData> samples;
    Ptr<SIFT> sift = SIFT::create();
    BFMatcher matcher;

    // Загружаем все образцы карт
    string path("./cards");
    for (auto& p : fs::recursive_directory_iterator(path)) {
        if (p.path().extension() == ".jpg") {
            string filePath = p.path().string();
            SampleData data;
            data.name = p.path().stem().string();
            data.image = imread(filePath);
            if (data.image.empty()) {
                cout << "Failed to parse sample: " << p.path().filename() << endl;
                continue;
            }
            sift->detectAndCompute(data.image, noArray(), data.keypoints, data.descriptors);
            samples.push_back(data);
            cout << "Parsed sample: " << p.path().filename() << endl;
        }
    }

    // Загружаем целевое изображение
    Mat target = imread("pic.jpg");
    if (target.empty()) {
        cout << "Failed to parse target" << endl;
        return -1;
    }

    Mat targetGray;
    cvtColor(target, targetGray, COLOR_BGR2GRAY);

    Mat blurred;
    GaussianBlur(targetGray, blurred, Size(5, 5), 2);

    Mat edges;
    Canny(blurred, edges, 50, 200);

    vector<vector<Point>> contours;
    findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat resultImg = target.clone();
    drawContours(resultImg, contours, -1, Scalar(0, 255, 0), 2);

    cout << "Found contours: " << contours.size() << endl;

    // Проходим по каждому контуру (карте)
    for (size_t i = 0; i < contours.size(); i++) {
        Rect roiRect = boundingRect(contours[i]);

        // Отбрасываем мелкие области
        if (roiRect.area() < 5000) continue;

        Mat cardROI = target(roiRect);

        vector<KeyPoint> cardKeypoints;
        Mat cardDescriptors;
        sift->detectAndCompute(cardROI, noArray(), cardKeypoints, cardDescriptors);

        if (cardKeypoints.empty() || cardDescriptors.empty()) continue;

        string bestMatchName = "Unknown";
        int bestMatchCount = 0;

        // Сопоставление с каждым сэмплом
        for (auto& sample : samples) {
            vector<vector<DMatch>> matches;
            matcher.knnMatch(cardDescriptors, sample.descriptors, matches, 2);

            auto good_matches = ratio_test(matches, 0.6);

            if (good_matches.size() > bestMatchCount) {
                bestMatchCount = (int)good_matches.size();
                bestMatchName = sample.name;
            }
        }

        // Подписываем найденное имя карты на центр карты
        Point textOrg(roiRect.x + roiRect.width / 2 - 50, roiRect.y + roiRect.height / 2);
        putText(resultImg, bestMatchName, textOrg, FONT_HERSHEY_SIMPLEX, 0.9, Scalar(0, 0, 255), 2);
        cout << "Card " << i + 1 << " detected as: " << bestMatchName << " with " << bestMatchCount << " good matches" << endl;
    }

    imshow("Detected Cards", resultImg);
    while ((waitKey() & 0xEFFFFF) != 27);

    return 0;
}
