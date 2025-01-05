namespace server {
    namespace rtp {
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
    }
}