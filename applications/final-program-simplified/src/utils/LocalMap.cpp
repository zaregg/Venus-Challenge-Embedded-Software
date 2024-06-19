#ifndef DETECTEDOBJECT_H
#define DETECTEDOBJECT_H

#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

using namespace cv;
using namespace std;
using json = nlohmann::json;

class DetectedObject {
public:
    string type;
    Point coordinates;

    DetectedObject(string t, Point c);
    json toJson() const;
};

#endif // DETECTEDOBJECT_H