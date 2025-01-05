#ifndef TVM_WRAPPER_
#define TVM_WRAPPER_

#include "tracker.hpp"
#include "line.hpp"
#include "state.hpp"

#include <tvm/runtime/module.h>
#include <tvm/runtime/packed_func.h>
#include <tvm/runtime/ndarray.h>
#include <opencv4/opencv2/opencv.hpp>
#include <thread>
#include <chrono>


/**
 * @file tvm_wrapper.hpp
 * @brief Handles Detect thread if the DetectionState is START.
 * 
 * This file contains functions that handle video frame to provide  vehicle localization, Line ID, Count.
 * Load AI Graph module by TVM Runtime ( MobileNetV2 + SSD , YOLOv5n or YOLOv5s )
 * 
 */
namespace api {
    /**
         * @class Detection
         * @brief Load AI Graph Moudle by tvm runtime ( Detect, OCR )
         *
         * This class provides functions to Detect Object, OCR, Line ID, Vehicle count
         * Graph Layer 
    */
    namespace detection {
        struct DetectedObject {
            int track_id;
            int class_id;
            float conf;
            bool is_matched = false;
            cv::Rect bbox;
        };

        struct GraphModule {
            tvm::runtime::PackedFunc input_func;
            tvm::runtime::PackedFunc run_func;
            tvm::runtime::PackedFunc output_func;
        };

        const static std::vector<std::string> COCO_CLASS_80 = {
            "person", "bicycle", "car", "motorbike", "aeroplane", "bus", "train", "truck", "boat", "traffic light", 
            "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", 
            "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", 
            "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", 
            "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", 
            "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "sofa", 
            "pottedplant", "bed", "diningtable", "toilet", "tvmonitor", "laptop", "mouse", "remote", "keyboard", 
            "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", 
            "teddy bear", "hair drier", "toothbrush"
        };

        GraphModule load_tvm_runtime_graph_module(const std::string& model_path, const std::string device = "cpu");

        void detect(
            DetectionState detection_state,
            std::mutex& frame_mutex,
            cv::Mat& shared_frame
        );
        
    } // namespace detection
}// namsespace api
#endif // TVM_WRAPPER_