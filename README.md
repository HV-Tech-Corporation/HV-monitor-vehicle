## Project
This project is designed to analyze the probability of vehicle accidents using computer vision.

## TEAM
- 김규진 (Kim Gyu Jin) [Github](https://github.com/garfield0xff) email : garfield0xff@gmail.com
- 김한빈 (Kim Han bin) [Github](https://github.com/hanbeen00)
email : hanbini2008@naver.com
- 김태훈 (Kim Tae hun) [Github](http://github.com/aksmf98)
email : aksmf98@naver.com
- 장윤재 (Jang Yun Jae) [Github](http://github.com/mealsOrder)
email : dbsw7007@gmail.com
- 조세기 (Jo Segi) [Github](https://github.com/orgs/HV-Tech-Corporation/people/SegiJo)
email : jsg0423@naver.com

## Features

Depending on our Monitoring System use case, you might need to know what features are included 

| Feature | Description |
|---------|--------------|
| BestShot |  Caputre Image by track ID ( YOLO, SORT )
| OCR | Vehicle plate license ( OpenCV, PaddleOCR ) |
| Accident Cost | Calculate the Accident Cost based on vision parameters: [Node, Cluster, Distance] |

## Installation

### Package  (Required)

1. [TVM ](https://github.com/apache/tvm)
```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DUSE_LLVM=ON \
  -DUSE_CUDA=OFF \   // If you use GPU, -DUSE_CUDA=ON
  -DUSE_OPENCL=OFF \
  -DUSE_VULKAN=OFF \
  -DUSE_METAL=OFF
```
2. [OpenCV](https://github.com/opencv/opencv) 4.8.0
3. [Boost](https://github.com/boostorg/boost) 1.86.0
4. [QT](https://www.qt.io/ko-kr/) 5.15.8
5. [Gstreamer](https://github.com/GStreamer/gstreamer) 1.24.9 

#### AI Model (Optional)

- **[`Detection model`](/defaults/main/main.yml)** is a 4-bit quantization-optimized YOLOv5, specifically designed for detection tasks on ARM-based targets.
- **[`OCR Model`](/defaults/main/main.yml)** is a Paddle-OCR recognition model designed to recognize license plate numbers.


#### Molecule (Optional)

- Molecule is used to test the various functionalities of the role.
- Instructions on how to install Molecule can be found in the [Molecule website](https://molecule.readthedocs.io/en/latest/installation.html). *You will also need to install the Molecule plugins package and the Docker Python SDK.*
- To run any of the NGINX Plus Molecule tests, you must first copy your NGINX Plus license to the role's [`files/license`](/files/license/) directory.

## Accident Cost Algorithm
- Our project aims to visualize Accident Cost by QT Framework





