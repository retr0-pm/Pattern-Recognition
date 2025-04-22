#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    string pathImg1 = "pic1.png";
    string pathImg2 = "pic2.png";

    Mat img1 = imread(pathImg1);
    Mat img2 = imread(pathImg2);

    if (img1.empty() || img2.empty()) {
        cerr << "Error: Failed to load images!" << endl;
        return -1;
    }

    // ORB детектор
    Ptr<ORB> orb = ORB::create(3000);

    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;

    orb->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    orb->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    imshow("Keypoints 1", img1);
    imshow("Keypoints 2", img2);

    // BFMatcher
    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    // Сортировка по расстоянию
    sort(matches.begin(), matches.end(), [](DMatch a, DMatch b) {
        return a.distance < b.distance;
    });

    // Оставим первые 100 совпадений
    const int numGoodMatches = 100;
    if (matches.size() > numGoodMatches) matches.resize(numGoodMatches);

    Mat imgMatches;
    drawMatches(img1, keypoints1, img2, keypoints2, matches, imgMatches);
    imshow("Matches", imgMatches);

    // Извлечение точек для гомографии
    vector<Point2f> points1, points2;
    for (size_t i = 0; i < matches.size(); i++) {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }

    // Вычисляем гомографию
    Mat H = findHomography(points2, points1, RANSAC);

    if (H.empty()) {
        cerr << "Error: Homography could not be computed!" << endl;
        return -1;
    }

    // Вычисляем размер итогового изображения
    Size panoramaSize(img1.cols + img2.cols, max(img1.rows, img2.rows));

    // Перспективное преобразование
    Mat result;
    warpPerspective(img2, result, H, panoramaSize);

    // Копируем первую картинку на результат
    Mat roi(result, Rect(0, 0, img1.cols, img1.rows));
    img1.copyTo(roi);

    imshow("Panorama", result);

    imwrite("panorama_result.jpg", result);
    cout << "Panorama saved to panorama_result.jpg" << endl;

    waitKey(0);
    return 0;
}
