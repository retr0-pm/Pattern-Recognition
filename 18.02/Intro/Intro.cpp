#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Загружаем изображение
    Mat image = imread("pic.jpg");

    // Проверяем успешность загрузки
    if (image.empty()) {
        cout << "Could not open or find the image!" << endl;
        return -1;
    }

    // Рисуем линию
    line(image, Point(50, 50), Point(300, 50), Scalar(255, 0, 0), 2);  // Синяя линия

    // Рисуем прямоугольник
    rectangle(image, Point(50, 70), Point(300, 150), Scalar(0, 255, 0), 3);  // Зелёный прямоугольник

    // Рисуем круг
    circle(image, Point(175, 250), 50, Scalar(0, 0, 255), -1);  // Красный круг, залитый

    // Рисуем эллипс
    ellipse(image, Point(175, 400), Size(100, 50), 45, 0, 360, Scalar(255, 255, 0), 2);  // Жёлтый эллипс

    // Рисуем многоугольник (пятиугольник)
    Point pts[1][5];
    pts[0][0] = Point(100, 500);
    pts[0][1] = Point(150, 450);
    pts[0][2] = Point(200, 500);
    pts[0][3] = Point(175, 550);
    pts[0][4] = Point(125, 550);
    const Point* ppt[1] = { pts[0] };
    int npt[] = { 5 };
    polylines(image, ppt, npt, 1, true, Scalar(255, 0, 255), 2);  // Фиолетовый контур

    // Добавляем текст
    putText(image, "Hello, OpenCV!", Point(50, 650), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);

    // Сохраняем итоговое изображение
    imwrite("result.jpg", image);

    // Отображаем изображение
    imshow("Result", image);
    waitKey(0);
    destroyAllWindows();

    return 0;
}
