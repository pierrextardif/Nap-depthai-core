#include "OakFrameRender.h"

OakFrameRender::OakFrameRender() {
    setup();
}

OakFrameRender::~OakFrameRender() {
}



void OakFrameRender::setup() {

    // for testing purposes

    // Define sources and outputs
    auto monoRight = pipeline.create<dai::node::MonoCamera>();
    auto monoLeft = pipeline.create<dai::node::MonoCamera>();
    auto manipRight = pipeline.create<dai::node::ImageManip>();
    auto manipLeft = pipeline.create<dai::node::ImageManip>();

    auto controlIn = pipeline.create<dai::node::XLinkIn>();
    auto configIn = pipeline.create<dai::node::XLinkIn>();
    auto manipOutRight = pipeline.create<dai::node::XLinkOut>();
    auto manipOutLeft = pipeline.create<dai::node::XLinkOut>();

    controlIn->setStreamName("control");
    configIn->setStreamName("config");
    manipOutRight->setStreamName("right");
    manipOutLeft->setStreamName("left");

    // Crop range
    dai::Point2f topLeft(0.2f, 0.2f);
    dai::Point2f bottomRight(0.8f, 0.8f);

    // Properties
    monoRight->setBoardSocket(dai::CameraBoardSocket::RIGHT);
    monoLeft->setBoardSocket(dai::CameraBoardSocket::LEFT);
    monoRight->setResolution(dai::MonoCameraProperties::SensorResolution::THE_720_P);
    monoLeft->setResolution(dai::MonoCameraProperties::SensorResolution::THE_720_P);
    manipRight->initialConfig.setCropRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
    manipLeft->initialConfig.setCropRect(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

    // Linking
    monoRight->out.link(manipRight->inputImage);
    monoLeft->out.link(manipLeft->inputImage);
    controlIn->out.link(monoRight->inputControl);
    controlIn->out.link(monoLeft->inputControl);
    configIn->out.link(manipRight->inputConfig);
    configIn->out.link(manipLeft->inputConfig);
    manipRight->out.link(manipOutRight->input);
    manipLeft->out.link(manipOutLeft->input);

    // Connect to device and start pipeline
    dai::Device device(pipeline);

    // Output queues will be used to get the grayscale frames
    auto qRight = device.getOutputQueue(manipOutRight->getStreamName(), 4, false);
    auto qLeft = device.getOutputQueue(manipOutLeft->getStreamName(), 4, false);
    auto controlQueue = device.getInputQueue(controlIn->getStreamName());
    auto configQueue = device.getInputQueue(configIn->getStreamName());

    // Defaults and limits for manual focus/exposure controls
    int exp_time = 20000;
    int exp_min = 1;
    int exp_max = 33000;

    int sens_iso = 800;
    int sens_min = 100;
    int sens_max = 1600;


}
