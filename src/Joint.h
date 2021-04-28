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
#include <fstream>
#include <sstream>
#include <string>


using namespace std;
using namespace glm;

struct JointNode {

    int id;
    float theta = 0.0f;
    //
    pair<float, float> theta_limit;

    float length;

    JointNode(int id, float length, pair<float, float> theta_limit = {global::Pi * (-1.0f), global::Pi}) : id(id),
                                                                                                           length(length),
                                                                                                           theta_limit(
                                                                                                                   theta_limit) {};
};

enum class METHODS {
    CCD,
    CJD,
    CC,
    RCD
};

class Joint {
public:
    Joint() {};

    Joint(int argc, char **argv, unsigned int scr_width, unsigned int scr_height) {
        if (argc <= 1) {
            cout << "Using default parameter." << endl;
            vector<float> joint_length = {100, 100, 50, 50};
            vector<pair<float, float>> joint_angle_limit;
            construct(joint_length, joint_angle_limit, scr_width, scr_height);
        } else {
            string input_file_path = argv[1];
            cout << input_file_path << endl;

            fstream input_file(input_file_path, fstream::in);
            if (!input_file.good()) {
                cerr << "Can't open file: " << input_file_path
                     << ". Please put the file in the directory with .exe file." << endl;
                return;
            }
            vector<float> joint_length;
            vector<pair<float, float>> joint_angle;
            stringstream ss;
            ss << input_file.rdbuf();
            string str;
            enum STATE {
                LENGTH,
                LIMIT,
                NOTHING
            } state = STATE::NOTHING;

            int line_cnt = 0;
            while (getline(ss, str)) {
                line_cnt++;
                if (str == "length:") {
                    state = STATE::LENGTH;
                } else if (str == "limit:") {
                    state = STATE::LIMIT;
                } else {
                    vector<float> nums;
                    vector<string> subStr = split(str, ' ');
                    for (int i = 0; i < subStr.size(); i++) {
                        nums.push_back(atof(subStr[i].c_str()));
                    }
                    cout << "nums:";
                    for (int i = 0; i < nums.size(); i++) {
                        cout << nums[i] << " ";
                    }
                    cout << endl;
                    if (state == STATE::LENGTH) {
                        joint_length.insert(joint_length.end(), nums.begin(), nums.end());
                    } else if (state == STATE::LIMIT) {
                        joint_angle.push_back(
                                pair<float, float>(nums[0] / 180.0f * global::Pi, nums[1] / 180.0f * global::Pi));
                    } else { ;
                    }
                }
            }
            construct(joint_length, joint_angle, scr_width, scr_height);
        }
    }

    void updateJoints(METHODS method);

    int getVerticesSize();

    void setScreenSize(unsigned int scr_width, unsigned int scr_height);

    vector<vec3> getVertices();

    void setTarget(const vec2 &target);

private:
    void construct(const vector<float> &joints_length, const vector<pair<float, float>> &joints_angle_limit,
                   unsigned int scr_width,
                   unsigned int scr_height);

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

    vector<string> split(string str, char ch) {
        vector<string> ret;
        int left = 0;
        int right = 0;
        while (left < str.length() && right < str.length()) {
            while (left < str.length() && str[left] == ch) {
                left++;
            }
            right = left;
            while (right < str.length() && str[right] != ch) {
                right++;
            }
            if (left < right) {
                ret.push_back(str.substr(left, right - left));
            }
            left = right;
        }
        return ret;
    };


    vector<JointNode> joints_;
    vector<float> joints_length_;
    vector<pair<float, float>> joints_angle_limit_;

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
