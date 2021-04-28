//
// Created by Sth on 2021/4/21.
//

#ifndef OPENGLHELLOWORLD_JOINT_H
#define OPENGLHELLOWORLD_JOINT_H

#include "globalFunction.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <iostream>
#include <vector>


using namespace std;
using namespace glm;

struct JointNode {

    int id;
    float theta;
    //
    pair<float, float> theta_limit;

    float length;

    JointNode(int id, float length, float theta = 0.0f,
              pair<float, float> theta_limit = {global::Pi * (-1.0f), global::Pi})
            :
            id(id), length(length), theta(theta), theta_limit(theta_limit) {};

};

enum class METHODS {
    CCD,
    CJD,
    CC,
    RCD
};

// 不同的算法只是下标顺序的不同
class Joint {
public:
    Joint() {};

    Joint(const vector<float> &joints_length, const vector<float> &joints_angle, unsigned int scr_width,
          unsigned int scr_height);

    void updateJoints(METHODS method);

    vector<vec3> getVertices();

    void setTarget(const vec2 &target);

private:
    // cyclic coordinate decent
    int updateJointsCCD();

    // cyclic joints decent
    int updateJointsCJD();

    // cyclic circulate
    int updateJointsCC();

    // Random coordinate decent
    int updateJointsRCD();

    // update joints straight line
    void updateJointsSL();

    vec2 getEndVertex();

    vec2 getIndexVertex(int index);

    void rotateJoints(int ori_idx, float rotate_angle);

    void updateSingleJoint(int origin_index, float &rotate_angle);

    vector<float> getCDF(int index);


    vector<JointNode> joints_;
    vector<float> joints_length_;
    vector<float> joints_angle_;

    // 转移矩阵
    vector<vector<float>> transfer_matrix_;

    vec2 origin_ = {0.0f, 0.0f};
    vec2 target_ = {0.0f, 0.0f};

    unsigned int screen_width_;
    unsigned int screen_height_;

    int max_iterator_ = 300;   // 最大的迭代次数
    float threshold_ = 0.1;     // 容许误差范围

    float total_length_ = 0.0f;

};

#endif //OPENGLHELLOWORLD_JOINT_H
