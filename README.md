## Project
This project is designed to analyze the probability of vehicle accidents using computer vision.

<img src="https://github.com/user-attachments/assets/966650ed-8b8a-49f8-b774-9d16ede31880" alt="bestmodel_1" width="350"/>
<img src="https://github.com/user-attachments/assets/db2de0b1-84d0-42fb-9f37-5815caec8190" alt="bestmodel_2" width="350"/>


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
| Feature | Description |
|---------|--------------|
| BestShot |  Caputre Image by track ID ( YOLO, SORT )
| OCR | Vehicle plate license ( OpenCV, PaddleOCR ) |
| Accident Cost | Calculate the Accident Cost based on vision parameters: [Node, Cluster, Distance] |

## Installation

### Package  (Required)

1. [TVM](https://github.com/apache/tvm)
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


#### Edge Device (Raspberry5)
- Processor: Broadcom BCM2712 \
- GPU: VideoCore VII \
- CPU Clock Speed: Quad-core @ 2.4GHz \
- RAM: 4GB LPDDR4X-4267 SDRAM \
- Maximum Power Consumption / Voltage: 5A @ 5V 


## Accident Cost Algorithm


<img src="https://github.com/user-attachments/assets/11513875-55f8-45dc-bdba-14c4b81419d1" alt="">
<img src="https://github.com/user-attachments/assets/6e5e4c5d-fab5-4ea0-b12b-491ea3350b4a" alt="">
<img src="https://github.com/user-attachments/assets/5b2f6719-bfd5-49a6-a031-d3f4c4c4fae3" alt=""><br> 
<img src="https://github.com/user-attachments/assets/edf6e42f-9237-411f-a319-0537622c45f4" alt=""><br>
<img src="https://github.com/user-attachments/assets/32e2b13c-26b6-4c27-bfbd-2f414e2fab15" alt="">






