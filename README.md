# FaceEnhancer

AI-assisted face enhancement and restoration pipeline built using C++ and OpenCV.

This project detects faces, applies multiple image enhancement techniques, and generates improved outputs with side-by-side visual comparisons.

---

## Demo Video

https://github.com/atulsharma47/FaceEnhancer/raw/main/assets/demo.mp4

---

## Sample Results

### Before vs Enhanced

<p align="center">
  <img src="assets/paired_face-8-DSC_6181.JPG.png" width="700"/>
</p>

---

## Features

- Face detection using Haar Cascades
- Image denoising
- Contrast enhancement using CLAHE
- Sharpening and detail enhancement
- Seamless face blending
- Batch image processing
- Before/after comparison generation
- Demo video generation

---

## Tech Stack

- C++
- OpenCV
- CMake

---

## Processing Pipeline

```text
Input Image
   ↓
Face Detection
   ↓
Noise Reduction
   ↓
Contrast Enhancement
   ↓
Sharpening
   ↓
Seamless Blending
   ↓
Enhanced Output
```

---

## Project Structure

```text
FaceEnhancer/
│
├── assets/
│   ├── demo.mp4
│   └── paired_face-*.png
│
├── images/
├── models/
│   └── haarcascade_frontalface_default.xml
│
├── output/
├── scripts/
│   └── make_demo.ps1
│
├── src/
│   └── main.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## How It Works

The application scans input images, detects facial regions using OpenCV Haar Cascades, and applies multiple enhancement operations including denoising, contrast improvement, sharpening, and blending.

Enhanced outputs are saved automatically along with paired comparison images.

---

## Build Instructions

### Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## Usage

Run the executable:

```bash
FaceEnhancer.exe
```

Input images should be placed inside:

```text
images/
```

Enhanced outputs are generated inside:

```text
output/
```

---

## Example Outputs

<p align="center">
  <img src="assets/paired_face-11-DSC_6050.JPG.png" width="700"/>
</p>

<p align="center">
  <img src="assets/paired_face-9-DSC_6051.JPG.png" width="700"/>
</p>

---

## Future Improvements

- Real-time webcam enhancement
- Deep learning-based face restoration
- GPU acceleration
- GUI application support
- Video enhancement pipeline

---

## Author

Atul Sharma

GitHub: https://github.com/atulsharma47