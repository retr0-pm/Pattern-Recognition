#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    string pathToVideo = "vid.mp4";
    VideoCapture cap(pathToVideo);
    if (!cap.isOpened()) {
        cerr << "Error: Failed to open video!" << endl;
        return -1;
    }

    // Получаем параметры видео (размеры кадра, частота кадров)
    int frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(CAP_PROP_FPS);

    // Создание объекта VideoWriter для сохранения обработанного видео
    VideoWriter writer("result.mp4", VideoWriter::fourcc('a', 'v', 'c', '1'), 25, Size(frameWidth, frameHeight));


    while (true) {
        Mat frame;
        cap >> frame; // Чтение очередного кадра
        if (frame.empty()) {
            break;
        }

        Mat gray, blurred, thresh;
        cvtColor(frame, gray, COLOR_BGR2GRAY); // Преобразование в оттенки серого
        GaussianBlur(gray, blurred, Size(5, 5), 2); // Гауссово размытие
        Canny(blurred, thresh, 50, 150); // Детектор Canny для выделения контуров

        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(thresh, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // Поиск контуров

        Mat resultFrame = frame.clone();

        for (size_t i = 0; i < contours.size(); i++) {
            double area = contourArea(contours[i]);
            if (area < 500) continue;

            vector<Point> approx;
            approxPolyDP(contours[i], approx, 0.02 * arcLength(contours[i], true), true); // Аппроксимация многоугольника

            Moments M = moments(contours[i]);
            int cX = int(M.m10 / M.m00);
            int cY = int(M.m01 / M.m00);

            // Классификация объектов на основе количества вершин
            string shapeType;
            if (approx.size() == 3) {
                shapeType = "Triangle";  // Треугольник
            }
            else if (approx.size() == 4) {
                Rect rect = boundingRect(approx);
                double aspectRatio = (double)rect.width / rect.height;
                double angle = fabs(90 - abs(atan2(approx[1].y - approx[0].y, approx[1].x - approx[0].x) * 180 / CV_PI));
                if (aspectRatio >= 0.85 && aspectRatio <= 1.15 && angle < 10) shapeType = "Square";  // Квадрат
                else shapeType = "Rectangle";  // Прямоугольник
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

            // Визуализация
            drawContours(resultFrame, contours, int(i), Scalar(0, 255, 0), 2); // Отображаем контур
            putText(resultFrame, shapeType, Point(cX - 30, cY + 5), FONT_HERSHEY_SIMPLEX, 0.45, Scalar(0, 0, 255), 1); // Подпись формы
        }

        imshow("Detected Shapes", resultFrame);

        writer.write(resultFrame);

        if (waitKey(1) == 27) break;
    }

    cap.release();
    writer.release(); // Закрытие видеофайла для записи
    return 0;
}
