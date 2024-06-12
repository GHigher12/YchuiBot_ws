/*
 * @Author: Ychui 2535410943@qq.com
 * @Date: 2024-03-20
 * @Description: 手柄速度控制节点
 */

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

class RobotTeleop
{
public:
    RobotTeleop();

private:
    void joyCallback(const sensor_msgs::Joy::ConstPtr &joy);

    ros::NodeHandle n;

    int i_velLinear_x, i_velLinear_y, i_velAngular;
    int i_accelerator, i_deceleration;
    int i_maxLinearVelIncrease, i_maxAngularVelIncrease, i_maxLinearVelReduce, i_maxAngularVelReduce;
    double linearvel_Max, angularvel_Max, linearvel_init, angularvel_init;
    ros::Publisher vel_pub_;
    ros::Subscriber joy_sub_;
};

RobotTeleop::RobotTeleop()
{

    n.param<int>("axis_linear_x",i_velLinear_x,1);  // axis[1] LS摇杆前后推 前为1 后为-1
    n.param<int>("axis_angular",i_velAngular,0);    // axis[0] LS摇杆左右推 左为1 右为-1
      
    n.param<double>("linear_vel_max", linearvel_Max, 0.8);//最大线速度可增加到的最大值（无论如何加速也会限制在该速度下）
    n.param<double>("angular_vel_max", angularvel_Max, 2);//最大角速度可增加到的最大值（无论如何加速也会限制在该速度下）
    n.param<double>("linear_vel", linearvel_init, 0.2);//初始最大线速度
    n.param<double>("angular_vel", angularvel_init, 0.6);//初始最大角速度

    vel_pub_ = n.advertise<geometry_msgs::Twist>("joy/cmd_vel", 1);
    joy_sub_ = n.subscribe<sensor_msgs::Joy>("joy", 10, &RobotTeleop::joyCallback, this);
}

void RobotTeleop::joyCallback(const sensor_msgs::Joy::ConstPtr &joy)
{
    geometry_msgs::Twist robot_vel;
    float linear_gain = 1.0;
    float angular_gain = 1.0;

    /**************线速度控制******************/
    //—————————————线速率因子————————————————//
    if (joy->axes[4] == -1) // RT加之最大线速率因子
    {
        linear_gain = 2.0;
        ROS_INFO("linear_gain accelerate....");
    }
    else if (joy->axes[5] == -1) // LT减至最小线速率因子
    {
        linear_gain = 0.5;
        ROS_INFO("linear_gain slow dwon....");
    }
    else if (joy->buttons[9] == 1) // LS摇杆按下
    {
        linear_gain = 1.0;
        ROS_INFO("linear_gain reset....");
    }
    //—————————————线速度大小————————————————//
    if (joy->buttons[0] == 1) // A键增大线速度
    {
        linearvel_init += 0.02;
        if (linearvel_init <= linearvel_Max)
            ROS_INFO("linear velocity add to %lf", linearvel_init);
        else
        {
            linearvel_init = linearvel_Max;
            ROS_INFO("The maximum linear speed limit has been reached");
        }
    }
    if (joy->buttons[1] == 1) // B键减小线速度
    {
        linearvel_init -= 0.02;
        if (linearvel_init > 0)
            ROS_INFO("linear velocity reduce to %lf", linearvel_init);
        else
        {
            linearvel_init = 0.1;
            ROS_INFO("The minimum linear speed limit has been reached");
        }
    }
    /**************角速度控制******************/
    //—————————————角速率因子————————————————//
    if (joy->buttons[5] == 1) // RB加至最大线速率因子
    {
        angular_gain = 1.5;
        ROS_INFO("angular_gain accelerate....");
    }
    else if (joy->buttons[4] == 1) // LB减至最小线速率因子
    {
        angular_gain = 0.5;
        ROS_INFO("angular_gain slow dwon....");
    }
    else if (joy->buttons[10] == 1) // RS摇杆按下
    {
        angular_gain = 1.0;
        ROS_INFO("angular_gain reset....");
    }

    if (joy->buttons[2] == 1) // X键增加角速度
    {
        angularvel_init += 0.1;
        if (angularvel_init <= angularvel_Max)
            ROS_INFO("angluar velocity add to %lf", angularvel_init);
        else
        {
            angularvel_init = angularvel_Max;
            ROS_INFO("The maximum angular speed limit has been reached");
        }
    }
    else if (joy->buttons[3] == 1) // Y键增加角速度
    {                              
        angularvel_init -= 0.1;
        if (angularvel_init > 0)
            ROS_INFO("angluar velocity reduce to %lf", angularvel_init);
        else
        {
            angularvel_init = 0.1;
            ROS_INFO("The minimum angular speed limit has been reached");
        }
       
    }
    robot_vel.angular.z = -1 * angularvel_init * joy->axes[i_velAngular] * angular_gain;
    robot_vel.linear.x = linearvel_init * joy->axes[i_velLinear_x] * linear_gain;
    ROS_INFO("robot_vel.angular.z = %.4f", robot_vel.angular.z);
    ROS_INFO("robot_vel.linear.x = %.4f", robot_vel.linear.x);
    vel_pub_.publish(robot_vel);
}
int main(int argc, char **argv)
{
    ros::init(argc, argv, "robot_teleop_joy");
    RobotTeleop robot_teleop;
    ros::spin();
}