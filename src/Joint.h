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
// d
struct JointNode {
    int id;
    vec2 origin;
    vec2 destination;

    JointNode(int id, vec2 ori, vec2 des) : id(id), origin(ori), destination(des) {};

    float getDis() {
        return global::getDis(destination, origin);
    }
};

// 不同的算法只是下标顺序的不同
class Joint {
public:
    Joint() {};

    Joint(const vector<float> &joints_length, unsigned int scr_width, unsigned int scr_height);

    // cyclic coordinate decent
    void updateJointsCCD();

    // reverse cyclic coordinate decent
    void updateJointsRCCD();

    // cyclic joints decent
    void updateJointsCJD();

    // combine CCD and CJD
    // todo: fix bug
    void updateJointsCCJD();

    // cyclic circulate
    void updateJointsCC();

    // Monte Carlo coordinate decent
    void updateJointsMCD();

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

        cout << "Transfer Matrix:" << endl;
        for (int i = 0; i < transfer_matrix_.size(); i++) {
            for (int j = 0; j < transfer_matrix_[i].size(); j++) {
                cout << transfer_matrix_[i][j] << " ";
            }
            cout << endl;
        }
    }

private:
    // update joints straight line
    void updateJointsSL();

    vec2 getEnd();

    void rotateJoints(int ori_idx, float rotate_angle);

    bool updateSingleJoint(int origin_index, float & rotate_angle);

    vector<float> getCDF(int index);


    vector<JointNode> joints_;
    vector<float> joints_length_;

    // 转移矩阵
    vector<vector<float>> transfer_matrix_;

    vec2 origin_ = {0.0f, 0.0f};
    vec2 target_ = {0.0f, 0.0f};

    unsigned int screen_width_;
    unsigned int screen_height_;

    int max_iterator_ = 300;   // 最大的迭代次数
    float threshold_ = 0.1;     // 容许误差范围

    float total_length_ = 0.0f;

    int joint_index = 0;
};

#endif //OPENGLHELLOWORLD_JOINT_H
