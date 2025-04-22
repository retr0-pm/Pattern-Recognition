#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    string pathToImage = "pic.jpg";
    Mat image = imread(pathToImage);

    if (image.empty()) {
        cerr << "Error: Failed to load image!" << endl;
        return -1;
    }

    // Преобразуем изображение в оттенки серого
    Mat gray, blurred, thresh;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(3, 3), 2);
    Canny(blurred, thresh, 50, 150);

    // Поиск контуров
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat resultImage = image.clone();

    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);

        vector<Point> approx;
        approxPolyDP(contours[i], approx, 0.02 * arcLength(contours[i], true), true);

        // Пропускаем контуры с менее чем тремя вершинами
        if (approx.size() < 3) continue;

        cout << "Contour " << i << " vertices: " << approx.size() << endl; // Выводим количество вершин контура

        string shapeType;
        Moments M = moments(contours[i]);
        int cX = int(M.m10 / M.m00);
        int cY = int(M.m01 / M.m00);

        // Классификация на основе количества вершин
        if (approx.size() == 3) {
            shapeType = "Triangle";  // Треугольник
        }
        else if (approx.size() == 4) {
            Rect rect = boundingRect(approx);
            double aspectRatio = (double)rect.width / rect.height;
            double angle = fabs(90 - abs(atan2(approx[1].y - approx[0].y, approx[1].x - approx[0].x) * 180 / CV_PI));
            cout << "Contour " << i << " aspect ratio: " << aspectRatio << ", angle: " << angle << endl; // Выводим соотношение сторон и угол

            // Если соотношение сторон близко к 1 (квадрат) и угол близок к 90, это квадрат
            if (aspectRatio >= 0.85 && aspectRatio <= 1.15 && angle < 10)
                shapeType = "Square";
            else
                shapeType = "Rectangle";  // Прямоугольник
        }
        else if (approx.size() == 5) {
            shapeType = "Pentagon";  // Пятиугольник
        }
        else if (approx.size() == 6) {
            shapeType = "Hexagon";  // Шестиугольник
        }
        else {
            // Для контуров с более чем 6 вершинами
            if (approx.size() > 6) {
                // Проверяем, может ли это быть кругом
                RotatedRect ellipseFit = fitEllipse(contours[i]);
                double ratio = (double)ellipseFit.size.width / ellipseFit.size.height;

                // Определяем круг или эллипс
                if (ratio > 0.85 && ratio < 1.15) {
                    shapeType = "Circle";  // Круг
                }
                else {
                    shapeType = "Ellipse";  // Эллипс
                }
            }
            else {
                shapeType = "Polygon";  // Многоугольник
            }
        }

        // Рисуем контуры и пишем название фигуры
        drawContours(resultImage, contours, (int)i, Scalar(0, 255, 0), 2);
        putText(resultImage, shapeType, Point(cX - 30, cY + 5),
            FONT_HERSHEY_SIMPLEX, 0.45, Scalar(0, 0, 255), 1);
    }

    // Отображаем результат
    imshow("Detected Shapes", resultImage);

    // Сохраняем результат в файл
    string outputPath = "result_image.jpg";
    imwrite(outputPath, resultImage);
    cout << "Result saved to: " << outputPath << endl;

    waitKey(0);
    return 0;
}
