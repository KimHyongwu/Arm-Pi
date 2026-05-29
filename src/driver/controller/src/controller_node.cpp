#include "rclcpp/rclcpp.hpp"

class ROSRobotController : public rclcpp::Node {
public:
    ROSRobotController() 
        :Node("controller")
    {}
private:
    double gravity = 9.80665;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    return 0;
}