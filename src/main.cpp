// src/main.cpp
#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>   // for seamlessClone
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using namespace cv;
using namespace std;

// Detect faces using Haar cascade
vector<Rect> detectFaces(const Mat &img, const string &cascadePath) {
    CascadeClassifier faceCascade;
    if (!faceCascade.load(cascadePath)) {
        cerr << "ERROR: Failed to load cascade at: " << cascadePath << endl;
        return {};
    }
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    equalizeHist(gray, gray);
    vector<Rect> faces;
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0|CASCADE_SCALE_IMAGE, Size(50,50));
    return faces;
}

// Expand rectangle by some fraction, staying inside image boundaries
Rect expandRect(const Rect &r, const Size &imgSize, double factor=0.35) {
    int w = r.width, h = r.height;
    int dx = int(w * factor), dy = int(h * factor);
    int x = max(0, r.x - dx);
    int y = max(0, r.y - dy);
    int x2 = min(imgSize.width, r.x + w + dx);
    int y2 = min(imgSize.height, r.y + h + dy);
    return Rect(x, y, x2 - x, y2 - y);
}

// Simple unsharp mask: result = img*amount + blurred*(1-amount)
Mat unsharpMask(const Mat &img, double amount=1.5, double sigma=3.0) {
    Mat blurred;
    GaussianBlur(img, blurred, Size(0,0), sigma);
    Mat result;
    addWeighted(img, amount, blurred, 1.0 - amount, 0, result);
    return result;
}

// Enhance face ROI: upscale -> denoise -> sharpen -> CLAHE
Mat enhanceFaceROI(const Mat &face) {
    // 1) Upscale (bicubic)
    Mat up;
    const int scale = 2; // 2x upsample
    resize(face, up, Size(), scale, scale, INTER_CUBIC);

    // 2) Denoise
    Mat den;
    fastNlMeansDenoisingColored(up, den, 10, 10, 7, 21);

    // 3) Sharpen with unsharp mask
    Mat sharp = unsharpMask(den, 1.6, 3.0);

    // 4) Improve contrast by applying CLAHE on L-channel
    Mat lab;
    cvtColor(sharp, lab, COLOR_BGR2Lab);
    vector<Mat> planes;
    split(lab, planes);
    Ptr<CLAHE> clahe = createCLAHE(2.0);
    clahe->apply(planes[0], planes[0]);
    merge(planes, lab);
    Mat result;
    cvtColor(lab, result, COLOR_Lab2BGR);

    return result;
}

// Process one image: detect faces, enhance each face and blend back
int processSingleImage(const string &inPath, const string &outPath, const string &cascadePath) {
    Mat img = imread(inPath);
    if (img.empty()) {
        cerr << "Error: Cannot open " << inPath << endl;
        return -1;
    }

    vector<Rect> faces = detectFaces(img, cascadePath);
    Mat working = img.clone();

    if (faces.empty()) {
        // No faces: apply global mild enhancement
        Mat enhanced = enhanceFaceROI(img);
        resize(enhanced, enhanced, img.size()); // bring back to original size
        addWeighted(img, 0.35, enhanced, 0.65, 0, working);
    } else {
        // For each detected face: crop, enhance, seamlessClone back
        for (const Rect &f : faces) {
            Rect ext = expandRect(f, img.size(), 0.4);
            Mat faceROI = img(ext).clone();

            Mat enhanced = enhanceFaceROI(faceROI);

            Mat enhancedResized;
            resize(enhanced, enhancedResized, faceROI.size(), 0, 0, INTER_CUBIC);

            Mat mask(enhancedResized.size(), CV_8UC1, Scalar(255));
            Point center(ext.x + ext.width/2, ext.y + ext.height/2);

            Mat tmp;
            seamlessClone(enhancedResized, working, mask, center, tmp, NORMAL_CLONE);
            working = tmp;
        }
    }

    if (!imwrite(outPath, working)) {
        cerr << "ERROR: Could not write " << outPath << endl;
        return -1;
    }
    cout << "Saved: " << outPath << endl;
    return 0;
}

int main(int argc, char** argv) {
    const string cascadeName = "haarcascade_frontalface_default.xml";
    if (argc < 3) {
        cout << "Usage:\n  FaceEnhancer input.jpg output.jpg\n  FaceEnhancer -b input_folder output_folder\n";
        return 0;
    }

    if (string(argv[1]) == "-b") {
        if (argc < 4) {
            cerr << "Batch mode requires input_folder and output_folder\n";
            return -1;
        }
        string inFolder = argv[2], outFolder = argv[3];
        fs::create_directories(outFolder);

        for (auto &p : fs::directory_iterator(inFolder)) {
            if (!p.is_regular_file()) continue;
            string inPath = p.path().string();
            string outPath = (fs::path(outFolder) / p.path().filename()).string();
            cout << "Processing: " << inPath << " -> " << outPath << endl;
            processSingleImage(inPath, outPath, cascadeName);
        }
    } else {
        string inPath = argv[1], outPath = argv[2];
        return processSingleImage(inPath, outPath, cascadeName);
    }
    return 0;
}
