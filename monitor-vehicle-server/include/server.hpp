/**
 * @file server.hpp
 * @brief Handles client connections and responses for video streaming and detection requests.
 * 
 * This file contains functions that handle various client requests for video streaming
 * and vehicle detection.
 * 
 * Supported endpoints:
 * - /start_stream : Start video streaming.
 * - /resume_stream : Resume video streaming.
 * - /pause_stream : Pause video streaming.
 * - /start_detection : Start vehicle detection in the video.
 */
#ifndef SERVER_HPP
#define SERVER_HPP

#include "tvm_wrapper.hpp"
#include "response.hpp" 

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <atomic>
#include <mutex>
#include <fstream>
#include <sstream>
/**
 * @class server::rtp::app
 * @brief Handles video streaming and detection requests.
 *
 * This class provides functions to handle various client requests, including starting, pausing,
 * and resuming video streaming, as well as starting vehicle detection.
 */
namespace server {
    namespace rtp {
        /**
         * @class app
         * @brief Handles video streaming and detection requests.
         *
         * This class provides functions to handle various client requests, including starting, pausing,
         * and resuming video streaming, as well as starting vehicle detection.
         */
        struct app {
            uint16_t port_num = 5004;
            std::string client_ip;

            app& port(uint16_t p) {
                port_num = p;
                return *this;
            }
            /**
             * @brief Handles client connections and provides responses for each request.
             * 
             * Supported endpoints:
             * - /start_stream : Start video streaming.
             * - /resume_stream : Resume video streaming.
             * - /pause_stream : Pause video streaming.
             * - /start_detection : Start vehicle detection in the video.
             * 
             * @param socket The client socket connection.
             */
            void handle_request(std::shared_ptr<boost::asio::ip::tcp::socket> shared_socket);
            // GStreamer 파이프라인
            std::string gstreamer_pipeline() const;
            // 비디오 스트리밍 함수
            void process_streaming(const std::string& video_path, std::shared_ptr<boost::asio::ip::tcp::socket> shared_socket);   
        }; //struct app

        // 서버 시작 함수
        void start_server(uint16_t port);  
        void response(std::shared_ptr<boost::asio::ip::tcp::socket> shared_socket, server::http_response::response_type status);

        enum class StreamingState {
            READY,
            STREAMING,
            PAUSED,
            REWIND,
        };

        enum class DetectionState {
            READY,
            START,
            PAUSED,
        };

        struct StreamingStatus {
            std::atomic<StreamingState> streaming_state;
            std::atomic<DetectionState> detection_state;

            StreamingStatus() : streaming_state(StreamingState::Ready), detection_state(DetectionState::Ready) {}
        };

        const std::string video_path = "/Users/gyujinkim/Desktop/Github/monitor-vehicle-api/server/traffic_jam2.mp4";
        std::string client_ip;
        

        extern std::condition_variable detection_cv;
        extern std::mutex frame_mutex;
        extern cv::Mat shared_frame;
        
    } // namespace rtp
} // namespace server

#endif // SERVER_HPP
