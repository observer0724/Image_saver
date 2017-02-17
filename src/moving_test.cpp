#include <ros/ros.h>
#include <ros/package.h>
#include <std_msgs/Float64.h>
#include <cwru_davinci_traj_streamer/trajAction.h>
#include <actionlib/client/simple_action_client.h>


// void scalerCallback(const std_msgs::Float64& weight)
// {
//   ROS_INFO("received value is: %f",weight.data);
//   weight_data = weight.data;
//   //really could do something interesting here with the received data...but all we do is print it
// }

bool g_server_goal_completed= false;

void doneCb(const actionlib::SimpleClientGoalState& state,
        const cwru_davinci_traj_streamer::trajResultConstPtr& result) {
    ROS_INFO(" doneCb: server responded with state [%s]", state.toString().c_str());
    ROS_INFO("got return val = %d; traj_id = %d",result->return_val,result->traj_id);
    g_server_goal_completed = true;
}

double weight_data;

void scalerCallback(const std_msgs::Float64& weight)
{
  ROS_INFO("received value is: %f",weight.data);

  weight_data = weight.data;
}

int main(int argc, char **argv) {
	//Set up our node.
	ros::init(argc, argv, "moving_test");
	ros::NodeHandle nh;

	std::string fname;


	trajectory_msgs::JointTrajectory track;

	trajectory_msgs::JointTrajectoryPoint default_position;
	default_position.positions.resize(14);

	default_position.positions[0] = -0.66709;
	default_position.positions[1] = -0.0507692;
	default_position.positions[2] = 0.201701;
	default_position.positions[3] = 1.25122;
	default_position.positions[4] = -0.0885405;
	default_position.positions[5] = 0.0329113;
	default_position.positions[6] = 0.15338;
	default_position.positions[7] = 0.734138;
	default_position.positions[8] = -0.253545;
	default_position.positions[9] = 0.200111;
	default_position.positions[10] = 0.540371;
	default_position.positions[11] = 0.0721441;
	default_position.positions[12] = -0.0711621;
	default_position.positions[13] = -0.163507;
	default_position.time_from_start = ros::Duration(5.0);

	track.points.push_back(default_position);
	cwru_davinci_traj_streamer::trajGoal tstart;
	tstart.trajectory = track;



	double wait_time_1 = 5.0;
  double wait_time_2 = 5.0;
	ros::Rate naptime(2.0);


	// ros::Subscriber scaler_sub= nh.subscribe("scaler", 1, scalerCallback);
	track.header.stamp = ros::Time::now();

	//Add an ID.
	cwru_davinci_traj_streamer::trajGoal tgoal;
	track.points.push_back(default_position);
	tgoal.trajectory = track;

	srand(time(NULL));
	tgoal.traj_id = rand();

	//Locate and lock the action server
	actionlib::SimpleActionClient<
	  cwru_davinci_traj_streamer::trajAction
	> action_client("trajActionServer", true);
	bool server_exists = action_client.waitForServer(ros::Duration(5.0));
	// something odd in above: does not seem to wait for 5 seconds, but returns rapidly if server not running
	ROS_INFO("Waiting for server: ");
	while (!server_exists && ros::ok()) {
	  server_exists = action_client.waitForServer(ros::Duration(5.0));
	  ROS_WARN("Could not connect to server; retrying...");
	}
	ROS_INFO("SERVER LINK LATCHED");

	//Send our message:
	ROS_INFO("Sending trajectory with ID %u", tgoal.traj_id);

	ros::Subscriber scaler_sub= nh.subscribe("scale", 1, scalerCallback);
  //ROS_INFO("%f",weight.data);
	while (weight_data <= 1.0){

		g_server_goal_completed= false;
		action_client.sendGoal(tgoal,&doneCb);

		while(!g_server_goal_completed){
			doneCb;
			naptime.sleep();
		}
		tgoal.trajectory.points[0].positions[2] -= 0.01;
    tgoal.trajectory.points[1].positions[2] -= 0.01;
    wait_time_1 = wait_time_2 + 1.0;
		wait_time_2 += 5.0;
		tgoal.trajectory.points[0].time_from_start = ros::Duration(wait_time_1);
    tgoal.trajectory.points[1].time_from_start = ros::Duration(wait_time_2);
    tstart.trajectory.points[0].time_from_start = ros::Duration(wait_time_2+2.0);
		tgoal.traj_id = rand();
		//ros::Subscriber scaler_sub= nh.subscribe("scale", 1, scalerCallback);
    ROS_INFO("%f",weight_data);
    ros::spinOnce();
	}
  action_client.sendGoal(tstart,&doneCb);
	//Wait for it to finish.
	while(!action_client.waitForResult(ros::Duration(wait_time_2 + 4.0)) && ros::ok()){
	  ROS_WARN("CLIENT TIMED OUT- LET'S TRY AGAIN...");
	  //Could add logic here to resend the request or take other actions if we conclude that
	  //the original is NOT going to get served.
	}
	//Report back what happened.
	ROS_INFO(
	  "Server state is %s, goal state for trajectory %u is %i",
	  action_client.getState().toString().c_str(),
	  action_client.getResult()->traj_id,
	  action_client.getResult()->return_val
	);

	//This has to do with the intermittent "Total Recall" bug that breaks the trajectory interpolator
	//If you see this appear in your execution, you are running the program too soon after starting Gazebo.
	if(action_client.getState() ==  actionlib::SimpleClientGoalState::RECALLED){
	  ROS_WARN("Server glitch. You may panic now.");
	}

	return 0;
}
