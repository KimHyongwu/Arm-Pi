// STD
#include <chrono>
// ROS2
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
// ArmPi
#include "ros_robot_controller_msgs/msg/servo_position.hpp"
#include "ros_robot_controller_msgs/msg/servos_position.hpp"

using namespace std::chrono_literals;

class ServoController : rclcpp::Node {
public:
    ServoController() 
        :Node("ServoController")
    {
        this->pub = this->create_publisher<ros_robot_controller_msgs::msg::ServosPosition>("/ros_robot_controller/bus_servo/set_position", 1);

        // Waiting for robot arm underlying control services to start
        this->client = this->create_client<std_srvs::srv::Trigger>("/ros_robot_controller/init_finish");
        this->client->wait_for_service();
    }
    
public:
    void set_servo_position(const size_t& duration, std::vector<std::pair<size_t, size_t> > positions);

private:
    rclcpp::Publisher<ros_robot_controller_msgs::msg::ServosPosition>::SharedPtr pub;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client;
};

void ServoController::set_servo_position(const size_t& duration, 
    std::vector<std::pair<size_t, size_t> > positions) 
{
    // Generate message
    auto msg = ros_robot_controller_msgs::msg::ServosPosition();
    msg.duration = duration;
    for (const auto& i : positions) {
        auto position = ros_robot_controller_msgs::msg::ServoPosition();
        position.id = i.first;
        position.position = i.second;

        msg.position.emplace_back(position);
    }

    // Send message
    this->pub->publish(msg);

    // Print log
    for (const auto& i : msg.position) 
        RCLCPP_INFO(rclcpp::get_logger(""), "duration=%.2f, id=%d, position=%d", msg.duration, i.id, i.position);
}

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto controller = std::make_shared<ServoController>();

    try {
        while (rclcpp::ok()) {
            controller->set_servo_position(0.5, { std::make_pair(4, 200) });    // Set the position of Servo 4 to 200
            std::this_thread::sleep_for(0.5s);                                  // Wait 0.5 sec
            controller->set_servo_position(0.5, { std::make_pair(4, 500) });    // Set the position of Servo 4 to 500
            std::this_thread::sleep_for(0.5s);                                  // Wait 0.5 sec
        }
    } catch(const rclcpp::exceptions::RCLError& e) {
        RCLCPP_ERROR(rclcpp::get_logger(""), "rclcpp::exceptions::RCLError: %s", e.what());
    } catch(const rclcpp::exceptions::InvalidServiceNameError& e) {
        RCLCPP_ERROR(rclcpp::get_logger(""), "rclcpp::exceptions::InvalidServiceNameError: %s", e.what());
    } catch(...) {
        RCLCPP_ERROR(rclcpp::get_logger(""), "Unknown error.");
    }

    controller.reset();     // Clear node
    rclcpp::shutdown();     // Shutdown 

    return 0;
}
