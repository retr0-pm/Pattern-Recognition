#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    string pathToImage = "pic.jpg";
    Mat image = imread(pathToImage);

    // Проверка на ошибки при загрузке изображения
    if (image.empty()) {
        cerr << "Error: Failed to load image!" << endl;
        return -1;
    }

    // Показываем исходное изображение
    imshow("Original Image", image);

    // Преобразуем изображение в оттенки серого
    Mat grayImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);

    // Применяем размытие Гаусса
    Mat blurredImage;
    GaussianBlur(image, blurredImage, Size(15, 15), 0);

    // Отображаем изображения для каждого эффекта
    imshow("Gray Image", grayImage);
    imshow("Gaussian Blur Image", blurredImage);

    // Сохраняем результаты
    imwrite("gray_image.png", grayImage);
    imwrite("gaussian_blur_image.png", blurredImage);

    waitKey(0);
    return 0;
}
