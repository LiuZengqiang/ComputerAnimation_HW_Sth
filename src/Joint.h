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

// todo:: recode JointNode using theta

struct JointNode {
    int id;
    vec2 origin;
    vec2 destination;

    JointNode(int id, vec2 ori, vec2 des) : id(id), origin(ori), destination(des) {};

    float getDis() {
        return global::getDis(destination, origin);
    }
};

class Joint {
public:
    Joint() {};

    Joint(const vector<float> &joints_length, unsigned int scr_width, unsigned int scr_height);

    // cyclic coordinate decent
    void updateJointsCCD();

    // cyclic joints decent
    void updateJointsCJD();

    // combine CCD and CJD
    // todo: fix bug
    void updateJointsCCJD();

    // cyclic circulate
    void updateJointsCC();

    // distance priority
    void updateJointsDP();

    void updateJointsStep();

    vector<JointNode> getKeyVertex();

    vector<vec3> getVertices();

    void setOrigin(const vec2 &origin);

    void setTarget(const vec2 &target);

    void setOneStep(int n) {
        max_iterator_ = n;
    }

    void debug() {
        for (int i = 0; i < joints_.size(); i++) {
            cout << "(" << joints_[i].origin.x << "," << joints_[i].origin.y << ")->";
        }
        cout << "(" << joints_.back().destination.x << "," << joints_.back().destination.y << ")" << endl;
    }

private:

    float getTotalLength();

    vec2 getEnd();

    void updateJoints(int ori_idx, float rotate_angle);

    bool updateSingleJoint(int origin_index);


    vector<JointNode> joints_;
    vector<float> joints_length_;
    vec2 origin_ = {0.0f, 0.0f};
    vec2 target_ = {0.0f, 0.0f};

    unsigned int screen_width_;
    unsigned int screen_height_;

    int max_iterator_ = 100;   // 最大的迭代次数
    float threshold_ = 0.1;     // 容许误差范围

    int joint_index = 0;
};

#endif //OPENGLHELLOWORLD_JOINT_H
