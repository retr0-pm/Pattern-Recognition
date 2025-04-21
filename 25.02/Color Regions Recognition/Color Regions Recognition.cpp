#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Путь к изображению
    string pathToImage = "pic.jpg";
    Mat image = imread(pathToImage);

    // Проверка на ошибки при загрузке изображения
    if (image.empty()) {
        cerr << "Error: Failed to load image!" << endl;
        return -1;
    }

    // Преобразуем изображение в пространство цветовых каналов HSV
    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // Создаем маски для выделения красного, зеленого и синего цвета
    Mat redMask, greenMask, blueMask;
    inRange(hsvImage, Scalar(0, 120, 70), Scalar(10, 255, 255), redMask);   // Красный цвет
    inRange(hsvImage, Scalar(35, 40, 40), Scalar(85, 255, 255), greenMask); // Зеленый цвет
    inRange(hsvImage, Scalar(100, 150, 0), Scalar(140, 255, 255), blueMask); // Синий цвет

    // Отображаем маски
    imshow("Red Mask", redMask);
    imshow("Green Mask", greenMask);
    imshow("Blue Mask", blueMask);

    // Сохраняем маски на диск
    imwrite("red_mask.png", redMask);
    imwrite("green_mask.png", greenMask);
    imwrite("blue_mask.png", blueMask);

    // Выделение областей на исходном изображении для красного, зеленого и синего цвета
    Mat redAreas, greenAreas, blueAreas;
    image.copyTo(redAreas, redMask);   // Копируем только красные области
    image.copyTo(greenAreas, greenMask); // Копируем только зеленые области
    image.copyTo(blueAreas, blueMask);  // Копируем только синие области

    // Отображаем выделенные области
    imshow("Red Areas", redAreas);
    imshow("Green Areas", greenAreas);
    imshow("Blue Areas", blueAreas);

    // Сохраняем выделенные области на диск
    imwrite("red_areas.png", redAreas);
    imwrite("green_areas.png", greenAreas);
    imwrite("blue_areas.png", blueAreas);

    waitKey(0);

    return 0;
}
