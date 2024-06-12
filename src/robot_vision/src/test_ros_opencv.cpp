#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

int main(int argc, char** argv)
{
    ros::init(argc, argv, "publisher_img");
    ros::NodeHandle nh;

    image_transport::ImageTransport it(nh);
    image_transport::Publisher pub = it.advertise("image", 1);

    cv::Mat image = cv::imread("/home/ghigher/workplace/pointcloud_ws/src/pub_sub_image/images/ros_logo.png");
    if (image.empty())
    {
        ROS_INFO("Image is Empty!");
    }
    sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();
    ros::Rate loop_rate(5);

    while(nh.ok())
    {
        pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
}
