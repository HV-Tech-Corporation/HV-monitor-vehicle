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

        tvm::Device config_model_device(const std::string& device, int device_core) {
            if (device == "gpu") {
                return {kDLCUDA, device_core};
            } else if (device == "cpu") {
                return {kDLCPU, device_core};
            } else {
                throw std::invalid_argument("Unsupported device type: " + device);
            }
        }

        GraphModule load_tvm_runtime_graph_module(const std::string& model_path, const std::string device) {
            try {
                tvm::runtime::Module runtime_moudle = tvm::runtime::Module::LoadFromFile(model_path);
                DLDevice dev = config_model_device(device, 0);
                tvm::runtime::Module graph_module = runtime_moudle.GetFunction("default")(dev);
                return {
                    graph_module.GetFunction("set_input"),
                    graph_module.GetFunction("run"),
                    graph_module.GetFunction("get_output")
                };
            } catch (const std::exception& e) {
                LOG(ERROR) << "this graph module cannot be loaded : " << model_path;
                throw;
            }
        }

        std::vector<api::detection::DetectedObject> descript_tvm_output_data(tvm::runtime::NDArray output, const std::vector<std::string>& coco_class, cv::Mat& frame) {
            const int64_t* shape = output.Shape().data();
            int num_detections = shape[1];

            float* output_data = static_cast<float*>(output->data);
            int class_num = coco_class.size();

            const float w_ratio = static_cast<float>(frame.cols) / 640.0f;
            const float h_ratio = static_cast<float>(frame.rows) / 640.0f;

            const int yolo_output_data_stride = 5 + class_num;
            std::vector<api::detection::DetectedObject> detected_objects;
            float conf_threshold = 0.8;

            for(int i = 0; i < num_detections; ++i) {
                int yolo_output_data_idx = i * yolo_output_data_stride;

                float cx  = output_data[yolo_output_data_idx + 0] * w_ratio;
                float cy = output_data[yolo_output_data_idx + 1] * h_ratio;
                float w = output_data[yolo_output_data_idx + 2] * w_ratio;
                float h = output_data[yolo_output_data_idx + 3] * h_ratio;
                float confidence = output_data[yolo_output_data_idx + 4];

                
                if (confidence > conf_threshold) {
                    float* match_score = &output_data[yolo_output_data_idx + 5];
                    int class_id = std::distance(match_score, std::max_element(match_score, match_score + class_num));

                    if(class_id == 1 || class_id == 2 || class_id == 3 || class_id == 5 ) {
                        int x1 = static_cast<int>(cx - (w / 2));
                        int y1 = static_cast<int>(cy - (h / 2));
                        int x2 = static_cast<int>(cx + (w / 2));
                        int y2 = static_cast<int>(cy + (h / 2));

                        api::detection::DetectedObject obj;
                        obj.track_id = -1;
                        obj.bbox = cv::Rect(x1, y1, x2 - x1, y2 - y1);
                        obj.conf = confidence;
                        obj.class_id = class_id;

                        detected_objects.push_back(obj);
                    }
                }
            }

            return detected_objects;
        }

        tvm::runtime::NDArray convert_frame_to_tvm_array(cv::Mat& frame, int batch_size, int input_w, int input_h) {
            cv::Mat resize_img;
            cv::resize(frame, resize_img, cv::Size(640, 640));
            resize_img.convertTo(resize_img, CV_32F, 1.0 / 255.0 );

            std::vector<int64_t> shape = { batch_size, 3, input_w, input_h};
            tvm::runtime::NDArray input_array = tvm::runtime::NDArray::Empty( shape, DLDataType{ kDLFloat, 32, 1 }, DLDevice{ kDLCPU, 0 });

            float* input_data = static_cast<float*>(input_array->data);

            for(int h = 0; h < input_h; ++h) {
                for(int w = 0; w < input_w; ++w) {
                    cv::Vec3f pixel = resize_img.at<cv::Vec3f>(h, w);
                    input_data[0 * input_h * input_w + h * input_w + w] = pixel[0];
                    input_data[1 * input_h * input_w + h * input_w + w] = pixel[1];
                    input_data[2 * input_h * input_w + h * input_w + w] = pixel[2];
                }
            }
            
            return input_array;
        };

        std::pair<cv::Scalar, std::string> get_warning_level(double distance) {
            if (distance < max_distance / 4.0) {
                return {cv::Scalar(255, 255, 255), "1"}; // 밝은 빨간색
            } else if (distance < max_distance / 3.0) {
                return {cv::Scalar(50, 50, 255), "2"}; // 어두운 빨간색
            } else if (distance < 2 * max_distance / 3.0) {
                return {cv::Scalar(0, 165, 255), "3"}; // 주황색
            } else {
                return {cv::Scalar(0x4A, 0xB2, 0x2C), "4"}; // 초록색
            }
        }

        void detect(
            std::atomic<bool>& is_start_detection,
            std::atomic<bool>& is_pause_detection,
            std::mutex& frame_mutex,
            cv::Mat& shared_frame
        ) {
            std::unordered_set<int> track_ids;
            std::map<int, std::vector<std::pair<int, cv::Point>>> track_points_by_objects_bbox;
            std::map<int, int> vehicle_num_by_lane;

            GraphModule graph = load_tvm_runtime_graph_module("model_path", "cpu or gpu");
            ObjectTracker tracker;


            while(is_start_detection) {
                std::unique_lock<std::mutex> lock(frame_mutex);
                
                if(shared_frame.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    continue;
                }

                tvm::runtime::NDArray input_array = convert_frame_to_tvm_array(shared_frame, 1, 640, 640);
                
                graph.input_func("input", input_array);
                graph.run_func();
                tvm::runtime::NDArray output = graph.output_func(0);

                track_points_by_objects_bbox.clear();
                vehicle_num_by_lane.clear();

                std::vector<api::detection::DetectedObject> detected_vehicles = descript_tvm_output_data(output, COCO_CLASS_80, shared_frame);
                std::vector<int> nms_indices = calculate_object_nms(detected_vehicles, 0.45f);
                
                std::vector<std::pair<int, cv::Rect>> detected_vehicles_with_class_id;
                for(int idx : nms_indices) {
                    const auto& detected_vehicle = detected_vehicles[idx];
                    detected_vehicles_with_class_id.emplace_back(detected_vehicle.class_id, detected_vehicle.bbox);
                }
                
                tracker.Run(detected_vehicles_with_class_id);
                const auto tracks = tracker.GetTracks();

                int track_cnt = 0;
                int node_cnt = 0;

                for(auto &trk : tracks) {
                    if (trk.second.second.coast_cycles_ < kMaxCoastCycles && (trk.second.second.hit_streak_ >= kMinHits)) {
                        const auto &bbox = trk.second.second.GetStateAsBbox();
                        if (bbox.x >= 0 && bbox.y >= 0 && bbox.x + bbox.width <= shared_frame.cols && bbox.y + bbox.height <= shared_frame.rows) { 
                            cv::Point tracked_point = trk.second.second.GetPredictedPoint();
                            cv::Point trk_pos(tracked_point.x, tracked_point.y);
                            cv::circle(shared_frame, trk_pos, 5, cv::Scalar(0, 0, 0), 2);

                            int lane = api::detection::line::checkPosition(trk_pos);
                            vehicle_num_by_lane[lane]++;

                            cv::putText(shared_frame, std::to_string(trk.first), cv::Point(bbox.tl().x, bbox.tl().y - 10),
                                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
                            cv::putText(shared_frame, std::to_string(lane), cv::Point(bbox.tl().x + bbox.width, bbox.tl().y - 10),
                                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 2);
                            cv::rectangle(shared_frame, bbox, cv::Scalar(0, 0, 255), 2);

                            track_points_by_objects_bbox[lane].emplace_back(node_cnt, trk_pos);
                            node_cnt++;

                            if (track_ids.find(trk.first) == track_ids.end()) {
                                cv::Mat bestShot_frame = shared_frame(bbox).clone();
                                bestShot_frame = shared_frame(bbox).clone();
                                
                            }
                            track_cnt++;
                        }
                    }

                    for (const auto &[bbox_id, count] : vehicle_num_by_lane) {
                        std::string text = "LINE ID: " + std::to_string(bbox_id) + ", CAR: " + std::to_string(count);
                        cv::putText(shared_frame, text, cv::Point(50, 50 + bbox_id * 30),
                            cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
                    }

                    std::vector<int> track_ids_by_lane;
                    for (const auto &[id, _] : track_points_by_objects_bbox) {
                        track_ids_by_lane.push_back(id);
                    }

                    std::sort(track_ids_by_lane.begin(), track_ids_by_lane.end());
                    std::string warning_lev;

                    for(size_t idx = 0; idx < track_ids_by_lane.size(); ++idx) {
                        int current_id = track_ids_by_lane[idx];
                        auto &current_pos = track_points_by_objects_bbox[current_id];

                        if(current_pos.size() <= 1) {
                            continue;
                        } 

                        std::sort(current_pos.begin(), current_pos.end(),
                        [](const std::pair<int, cv::Point> &a, const std::pair<int, cv::Point> &b) {
                            return a.second.y < b.second.y; // y 좌표 기준 오름차순 정렬
                        });

                        for (size_t i = 1; i < current_pos.size(); ++i) {
                            double distance = cv::norm(current_pos[i].second - current_pos[i - 1].second);                        
                            auto [color, emergency_level] = get_warning_level(distance);
                            cv::line(shared_frame, current_pos[i - 1].second, current_pos[i].second, color, 2);
                        }

                        if (idx < track_ids_by_lane.size() - 1) {
                        int next_id = track_ids_by_lane[idx + 1];
                        auto &next_points = track_points_by_objects_bbox[next_id];

                        // 다음 ID의 points와 연결
                        for (const auto &[node1, p1] : current_pos) {
                            for (const auto &[node2, p2] : next_points) {
                                double distance = cv::norm(p2 - p1);

                                // 거리 구간에 따른 색상과 긴급 수준
                                auto [color, emergency_level] = get_warning_level(distance);

                                // 선 그리기
                                cv::line(shared_frame, p1, p2, color, 2);
                            }
                        }
                        }

                        if (current_pos.size() > 10) {
                            current_pos.erase(current_pos.begin(), current_pos.end() - 10);
                        }
                    }

                    
                }

            }
        }
    }
}