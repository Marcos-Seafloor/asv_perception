#ifndef CLASSIFIEDOBSTACLEPROJECTIONNODELET_H
#define CLASSIFIEDOBSTACLEPROJECTIONNODELET_H

#include <ros/ros.h>
#include <nodelet_topic_tools/nodelet_lazy.h>
#include <sensor_msgs/Image.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <asv_perception_common/Homography.h>
#include <asv_perception_common/ClassificationArray.h>

#include "defs.h"

namespace obstacle_id
{
    /*
    Nodelet for classified obstacle backprojection from 2D to 3D.  
    In 2D, combines segmented/unknown obstacle map with classified obstacle bounding boxes
    Expands classified obstacle bounding boxes as needed, estimates 3d properties, then creates Obstacle messages for the classified obstacles
    Outputs filtered unknown obstacle map for further processing
    Input topics:
    - segmentation:       2d obstacle map of unknown obstacle types
    - classification:     ClassificationArray
    - homography:         rgb to world homography matrix
    Output topics:
    - output:             ObstacleArray
    - output_segmentation:  2d obstacle map, with classified obstacles removed
    */
    class ClassifiedObstacleProjectionNodelet 
    : public nodelet_topic_tools::NodeletLazy
    {
    public:

        using base_type = nodelet_topic_tools::NodeletLazy;
        using segmentation_msg_type = sensor_msgs::Image;
        using classification_msg_type = asv_perception_common::ClassificationArray;
        using homography_msg_type = asv_perception_common::Homography;
        
        // default constructor
        ClassifiedObstacleProjectionNodelet() = default;
                                        
    protected:

        /** \brief Nodelet initialization routine. */
        void onInit () override;

        /** \brief LazyNodelet connection routine. */
        void subscribe () override;
        void unsubscribe () override;

        // the callback function to handle input
        void sub_callback ( 
            typename segmentation_msg_type::ConstPtr
            , typename classification_msg_type::ConstPtr
        );

        // homography callback
        void cb_homography(
            typename homography_msg_type::ConstPtr
        );
        
    private:

        // publishers
        ros::Publisher _pub_obstacles;
        ros::Publisher _pub_segmentation;

        // subscriptions
        ros::Subscriber _sub_homography;

        message_filters::Subscriber<segmentation_msg_type> _sub_segmentation;
        message_filters::Subscriber<classification_msg_type> _sub_classification;

        // sync policy for segmentation + classification subscriptions
        using _seg_cls_sync_policy_type = message_filters::sync_policies::ApproximateTime<segmentation_msg_type, classification_msg_type>;
        using _seg_cls_synchronizer_type = message_filters::Synchronizer<_seg_cls_sync_policy_type>;
        boost::shared_ptr<_seg_cls_synchronizer_type> _seg_cls_sync;

        // homography msg storage
        typename asv_perception_common::Homography::ConstPtr _homography;

    public:
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};  // class
}

#endif  //#ifndef