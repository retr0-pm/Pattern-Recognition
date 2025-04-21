#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    // Путь к изображению
    string pathToImage = "pic.png";
    Mat image = imread(pathToImage);

    // Проверка на ошибки при загрузке изображения
    if (image.empty()) {
        cerr << "Error: Failed to load image!" << endl;
        return -1;
    }

    // Преобразуем изображение в цветовую модель HSV
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // Маска для серого цвета (диапазон серых оттенков)
    Mat mask;
    inRange(hsvImage, Scalar(0, 0, 60), Scalar(180, 50, 200), mask);

    // Применяем маску
    Mat maskedImage;
    bitwise_and(image, image, maskedImage, mask);

    // Преобразуем изображение в оттенки серого для Хафа
    Mat grayImage;
    cvtColor(maskedImage, grayImage, COLOR_BGR2GRAY);

    // Применяем размытие Гаусса для уменьшения шумов
    GaussianBlur(grayImage, grayImage, Size(7, 7), 2, 2);

    // Отображаем изображение после размытия
    imshow("Blurred Image", grayImage);

    // Находим круги с помощью преобразования Хафа
    vector<Vec3f> circles;
    HoughCircles(grayImage, circles, HOUGH_GRADIENT, 1, grayImage.rows / 8, 100, 30, 10, 100);

    // Копируем изображение для отрисовки результатов
    Mat resultImage = image.clone();

    // Отрисовываем обнаруженные круги
    for (size_t i = 0; i < circles.size(); i++) {
        Vec3i c = circles[i];
        Point center(c[0], c[1]);
        int radius = c[2];
        // Отрисовка центра круга
        circle(resultImage, center, 3, Scalar(255, 0, 0), -1); // Центр синим
        // Отрисовка самого круга
        circle(resultImage, center, radius, Scalar(0, 255, 0), 2); // Зеленая окружность
    }

    // Отображаем маску, маскированное изображение и результат
    imshow("Mask", mask);
    imshow("Masked Image", maskedImage);
    imshow("Detected Circles", resultImage);

    // Сохраняем финальный результат
    imwrite("detected_circles_with_mask.png", resultImage);

    waitKey(0);
    return 0;
}
