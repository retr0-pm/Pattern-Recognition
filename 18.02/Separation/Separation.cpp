#include <opencv2/opencv.hpp>

using namespace cv;

void setScalar(int i, Mat& region) {
    switch (i % 4) {
    case 0:  // Покраска
        region.setTo(Scalar{ 255, 0, 0 });
        break;
    case 1:  // Инвертирование
        bitwise_not(region, region);
        break;
    case 2:  // Размытие
        GaussianBlur(region, region, Size(15, 15), 0);
        break;
    case 3:  // Сдвиг яркости
        region += Scalar(50, 50, 50);
        break;
    }
}

int main() {
    auto pathToImage = "pic.jpg";
    Mat image = imread(pathToImage);

    if (image.empty()) {
        std::cerr << "Error: Failed to load image!" << std::endl;
        return -1;
    }

    int N = 2;
    int w = image.cols / N;
    int h = image.rows / N;

    Mat result = image.clone();

    int counter = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Rect roi(i * w, j * h, w, h);
            Mat subRegion = result(roi);

            setScalar(counter++, subRegion);
        }
    }

    imshow("Processed", result);
    waitKey(0);

    return 0;
}
