#include "tvm_wrapper.hpp"

namespace api {
    namespace detection {

        float calculate_object_iou(const cv::Rect& box1, const cv::Rect& box2) {
            int inter_left_x = std::max(box1.x, box2.x);
            int inter_left_y = std::max(box1.y, box2.y);
            int inter_right_x = std::min(box1.x + box1.width, box2.x + box2.width);
            int inter_right_y = std::min(box1.y + box1.height, box2.y + box2.height);

            int inter_area = std::max(0, inter_right_x - inter_left_x) * std::max(0, inter_right_y - inter_right_y);
            int box_area1 = box1.width * box1.height;
            int box_area2 = box2.width * box2.height;

            return static_cast<float>(inter_area) / (box_area1 + box_area2 - inter_area);
        }

        std::vector<int> calculate_object_nms(const std::vector<api::detection::DetectedObject>& detected_objects, float threshold) {
            std::vector<int> detect_objects_indices;
            std::vector<int> sorted_detected_object_indices(detected_objects.size());
            std::vector<cv::Rect> detect_object_bboxes;
            std::vector<float> detect_object_confs;

            for (const auto& obj : detected_objects) {
                detect_object_bboxes.push_back(obj.bbox);
                detect_object_confs.push_back(obj.conf);
            }

            for (int i = 0; i < detect_object_bboxes.size(); ++i) {
                sorted_detected_object_indices[i] = i;
            }

            std::sort(sorted_detected_object_indices.begin(), sorted_detected_object_indices.end(), [&](int i, int j) {
                return detect_object_confs[i] > detect_object_confs[j];
            });
            
            std::vector<bool> is_supprressed (detect_object_bboxes.size(), false);
            for(int i  = 0; i < sorted_detected_object_indices.size(); ++i) {
                int idx1 = sorted_detected_object_indices[i];
                if (is_supprressed[idx1]) continue;
                detect_objects_indices.push_back(idx1);

                for(int j = i + 1; j < sorted_detected_object_indices.size(); ++j) {
                    int idx2 = sorted_detected_object_indices[j];
                    if(is_supprressed[idx2]) continue;

                    if(calculate_object_iou(detect_object_bboxes[idx1], detect_object_bboxes[idx2]) > threshold ) {
                        is_supprressed[idx2] = true;
                    }
                }
            }
            return detect_objects_indices;
        }

        tvm::Device config_model_device(const std::string& model_path, const std::string device_name) {
            tvm::runtime::Module  
        }

    }
}