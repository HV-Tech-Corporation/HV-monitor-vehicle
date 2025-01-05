#include "server.hpp"

namespace server{
    namespace rtp {
        StreamingStatus streaming_status;

        void set_streaming_state(StreamingState state) {
            streaming_status.streaming_state.store(state);
        }

        void set_detection_state(DetectionState state) {
            streaming_status.detection_state.store(state);
        }

        void pause_streaming() {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }

        void response(std::shared_ptr<boost::asio::ip::tcp::socket> socket, server::http_response::response_type status) {
            try {
                server::http_response::response res;
                res.set_status(status);
                res.add_header("Content-Type", "text/html");
                res.add_header("Connection", "keep-alive");
                res.content = res.response_body_to_string(status);
                auto buffers = res.to_buffers();
                boost::asio::write(*socket, buffers);
            } catch (const std::exception& e) {
                BOOST_LOG_TRIVIAL(error) << "response error : " << e.what();
            }
        }

        void app::handle_request(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
            try {
                boost::asio::streambuf request;
                boost::asio::read_until(*socket, request, "\r\n");
                std::istream request_stream(&request);
                std::string method, path;
                request_stream >> method >> path;

                if (method == "GET" && path == "/start_stream") {
                    if(streaming_status.streaming_state.load() == StreamingState::STREAMING) {
                        response(socket, server::http_response::response_type::bad_request);
                        return;
                    }

                    response(socket, server::http_response::response_type::ok);
                    set_streaming_state(StreamingState::STREAMING);

                    std::string video_path = "";

                    std::thread([this, socket, video_path]() mutable {
                        try {
                            process_streaming(video_path, socket);
                        } catch (const std::exception& e) {
                            BOOST_LOG_TRIVIAL(error) << "streaming thread : " << e.what();
                        }
                    }).detach();    
                }
                else if  (method == "GET" && path == "/pause_stream") {
                    response(socket, server::http_response::response_type::ok);
                    set_streaming_state(StreamingState::PAUSED);
                }
                else if (method == "GET" && path == "/resume_stream") {
                    response(socket, server::http_response::response_type::ok);
                    set_streaming_state(StreamingState::REWIND);
                }
                else if (method == "GET" && path == "/start_detection") {
                    response(socket, server::http_response::response_type::ok);
                    set_detection_state(DetectionState::START);
                    api::detection::detect(streaming_status.detection_state, )
                }
                else if (method == "GET" && path == "/pause_detection") {
                    response(socket, server::http_response::response_type::ok);
                    set_detection_state(DetectionState::PAUSED);
                }
            } catch (const std::exception& e) {
                response(socket, server::http_response::response_type::internal_server_error);
            }
        }

        std::string app::gstreamer_pipeline() const {
            return  "appsrc ! videoconvert ! video/x-raw,format=I420 ! jpegenc ! rtpjpegpay ! "
			"udpsink host=" + client_ip + " port=5004 auto-multicast=true";
        } 

        void app::process_streaming(const std::string& video_path, std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
            cv::VideoCapture cap(video_path);
            if(!cap.isOpened()) {
                BOOST_LOG_TRIVIAL(error) << "could not open video file ";
                return;
            }

            cv::Size frame_size((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));
            double fps = cap.get(cv::CAP_PROP_FPS);
            cv::VideoWriter writer;
            if(!writer.open(gstreamer_pipeline(), cv::CAP_GSTREAMER, 0, fps, frame_size)) {
                BOOST_LOG_TRIVIAL(error) << "could not open gstreamer pipeline";
                return;
            }

            int frame_index = 0;
            cv::Mat frame;

            while (true) {
                switch(streaming_status.streaming_state.load()) {
                    case StreamingState::READY : {
                        break;
                    }

                    case StreamingState::STREAMING : {
                        break;
                    }

                    case StreamingState::PAUSED :  {
                        pause_streaming();
                        continue;
                    }

                    case StreamingState::REWIND : {
                        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
                        frame_index = 0;
                        break;
                    }
                }

                cap >> frame;
                if (frame.empty()) {
                    set_streaming_state(StreamingState::REWIND);
                    continue;
                }

                switch (streaming_status.detection_state.load())
                {
                    case DetectionState::READY :
                    break;
                    
                    case DetectionState::START :

                    case DetectionState::PAUSED :
                }
            }
            
        }

        void start_server(uint16_t port) {
            boost::asio::io_context io_context;
            boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
            app rtp_app;

            BOOST_LOG_TRIVIAL(debug) << "server started on port : " << port;

            while(true) {
                auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context);
                acceptor.accept(*socket);

                try {
                    if(socket->is_open()) {
                        boost::asio::socket_base::keep_alive option(true);
                        socket->set_option(option);
                        boost::asio::ip::tcp::endpoint remote_ep = socket->remote_endpoint();
                        client_ip  = remote_ep.address().to_string();

                        std::thread([&rtp_app, socket]() {
                            try {
                                
                            } catch (const std::exception& e) {
                                BOOST_LOG_TRIVIAL(error) << "Error in client thread : " << e.what();
                            }
                        }).detach();
                    } else {
                        BOOST_LOG_TRIVIAL(error) << "Error: Socket is not open after accept";
                    }
                } catch (const boost::system::system_error& e) {
                    BOOST_LOG_TRIVIAL(error) << "Error: handling client : " << e.what();
                }
            }
        }


    }
}




