//
// Created by Sth on 2021/4/21.
//
#include "Joint.h"
#include "globalFunction.h"
#include <iostream>

Joint::Joint(const vector<float> &joints_length, const vector<float> &joints_angle, unsigned int scr_width,
             unsigned int scr_height) {
    screen_width_ = scr_width;
    screen_height_ = scr_height;
    joints_length_.assign(joints_length.begin(), joints_length.end());
    joints_angle_.assign(joints_angle.begin(), joints_angle.end());

    for (int i = 0; i < joints_length_.size(); i++) {
        total_length_ += joints_length_[i];
        if (i < joints_angle.size()) {
            joints_.push_back(JointNode(i, joints_length_[i], joints_angle_[i]));
        } else {
            joints_.push_back(JointNode(i, joints_length_[i], 0.0f));
        }
    }
    transfer_matrix_.resize(joints_.size());
    for (int i = 0; i < transfer_matrix_.size(); i++) {
        transfer_matrix_[i] = getCDF(i);
    }
    vec2 end = getEndVertex();
    cout << "end:(" << end.x << "," << end.y << ")" << endl;
}

// cyclic coordinate decent
// 4 3 2 1
void Joint::updateJointsCCD() {
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
    } else {
        bool has_resolution = false;
        int i = 0;
        float rotate_angle = 0.0f;
        vector<int> indexes(joints_.size());
        for (int j = 0; j < indexes.size(); j++) {
            indexes[j] = indexes.size() - j - 1;
        }
        for (; i < max_iterator_ && has_resolution == false; i++) {
            int idx = indexes[i % indexes.size()];
            has_resolution = updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
        }
        cout << "iterator times:" << i << endl;
    }
}

void Joint::setTarget(const vec2 &target) {
    target_ = target;
}

void Joint::rotateJoints(int ori_idx, float rotate_angle) {

    for (int i = ori_idx; i < joints_.size(); i++) {
        joints_[i].theta += rotate_angle;
    }
}

vector<vec3> Joint::getVertices() {
    vector<vec3> ret;

    vec2 end_vertex(origin_);
    ret.push_back(vec3(end_vertex, 0.0f));
    float angle = 0.0f;
    for (int i = 0; i < joints_.size(); i++) {
        angle += joints_[i].theta;
        vec2 vec(joints_[i].length, 0.0f);
        vec = global::rotateVec2(vec2(0.0f, 0.0f), vec, angle);
        end_vertex = end_vertex + vec;
        ret.push_back(vec3((end_vertex.x * 2) / (screen_width_), (end_vertex.y * 2) / screen_height_, 0.0f));
    }
//    for (int i = 0; i < ret.size(); i++) {
//        cout << "(" << ret[i].x << "," << ret[i].y << ")";
//    }
//    cout << endl;
    return ret;
}

void Joint::updateJointsCJD() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
    } else {

        bool has_resolution = false;

        float rotate_angle = 0.0f;
        vector<int> indexes;
        for (int j = joints_.size() - 1; j >= 0; j--) {
            for (int k = j; k < joints_.size(); k++) {
                if (j == 0 && k == joints_.size() - 1) {
                    continue;
                } else {
                    indexes.push_back(k);
                }
            }
        }
        int i = 0;
        for (; i < max_iterator_ && has_resolution == false; i++) {
            int idx = indexes[i % indexes.size()];
            has_resolution = updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
        }
        cout << "iterator times:" << i << endl;
    }
}

bool Joint::updateSingleJoint(int origin_index, float &rotate_angle) {
    vec2 end_vertex = getEndVertex();
    if (global::getDis(end_vertex, target_) <= threshold_) {
        return true;
    }
    vec2 ori_vertex = getIndexVertex(origin_index);
    vec2 ori_end = end_vertex - ori_vertex;
    vec2 ori_target = target_ - ori_vertex;

    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
    rotate_angle = acos(global::clampFloat(cos));
    if (sin < 0.0f) {
        rotate_angle *= -1;
    }
    if (abs(rotate_angle) <= global::Epsilon) {
        rotate_angle = global::Epsilon * 10;
    }
    end_vertex = getEndVertex();
    return global::getDis(end_vertex, target_) <= threshold_;
}

void Joint::updateJointsCC() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        // 可以到达，迭代计算
    } else {
        bool has_resolution = false;
        int i = 0;
        float rotate_angle = 0.0f;
        vector<int> indexes;
        for (int j = joints_.size() - 1; j >= 0; j--) {
            indexes.push_back(j);
        }
        for (int j = 1; j < joints_.size() - 1; j++) {
            indexes.push_back(j);
        }
        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            // 4,3,2,1,2,3
            int idx = indexes[i % indexes.size()];
            has_resolution = updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
        }

        cout << "iterator times:" << i << endl;
    }
}

void Joint::updateJointsSL() {

//    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
//    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
//    rotate_angle = acos(global::clampFloat(cos));
//    if (sin < 0.0f) {
//        rotate_angle *= -1;
//    }
//    if (abs(rotate_angle) <= global::Epsilon) {
//        rotate_angle = global::Epsilon * 10;
//    }

    vec2 direction = glm::normalize(target_ - origin_);
    float cos = 1.0f * glm::dot(direction, glm::vec2(1.0f, 0.0f));
    float sin = -1.0f * direction.y;
    float theta = acos(global::clampFloat(cos));
    if (sin > 0.0f) {
        theta *= -1.0f;
    }
    joints_[0].theta = theta;

    for (int i = 1; i < joints_.size(); i++) {
        joints_[i].theta = 0.0f;
    }
}

void Joint::updateJointsMCD() {
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
    } else {
        vector<int> cnt(3, 0);
        int i = 0;
        float rotate_angle = 0.0f;
        bool has_resolution = false;
        int index = joints_.size() - 1;

        for (; i < max_iterator_ * joints_.size() && has_resolution == false; i++) {
            cnt[index]++;

            has_resolution = updateSingleJoint(index, rotate_angle);
            rotateJoints(index, rotate_angle);
            float x = global::getRand();
            int j = 0;
            for (; j < transfer_matrix_[index].size(); j++) {
                if (transfer_matrix_[index][j] >= x) {
                    break;
                }
            }
            if (j >= joints_.size()) {
                index = joints_.size() - 1;
            } else {
                index = j;
            }
        }
        for (int j = 0; j < cnt.size(); j++) {
            cout << cnt[j] << " ";
        }
        cout << "iterator times:" << i + 1 << endl;
    }
}

vector<float> Joint::getCDF(int index) {
    vector<float> cdf(joints_length_.size(), 0.0f);

    vector<float> lengths(joints_.size(), 0.0f);
    float length = 0.0f;
    float total_proportion = 0.0f;
    for (int i = joints_length_.size() - 1; i >= 0; i--) {
        if (i != index) {
            length += joints_length_[i];
            lengths[i] = 1.0f / length;
            total_proportion += lengths[i];
        } else {
            length += joints_length_[i];
        }
    }
    for (int i = 0; i < lengths.size(); i++) {
        if (i != index) {
            lengths[i] /= total_proportion;
        }
    }

    cdf[0] = global::clampFloat(lengths[0], 0.0f, 1.0f);
    for (int i = 1; i < joints_.size(); i++) {
        if (i != index) {
            cdf[i] = global::clampFloat(cdf[i - 1] + lengths[i], 0.0f, 1.0f);
        } else {
            cdf[i] = cdf[i - 1];
        }
    }
    return cdf;
}

void Joint::updateJointsRCCD() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        // 可以到达，迭代计算
    } else {
        bool has_resolution = false;
        int i = 0;
        float rotate_angle = 0.0f;
        vector<int> indexes;
        for (int j = 0; j < joints_.size(); j++) {
            for (int k = j; k < joints_.size(); k++) {
                indexes.push_back(k);
            }
        }

        for (; i < max_iterator_ && has_resolution == false; i++) {
            int idx = indexes[i % indexes.size()];
            has_resolution = updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
        }
        cout << "iterator times:" << i << endl;
    }
}

vec2 Joint::getEndVertex() {
    return getIndexVertex(joints_.size());
}

/**
 * 返回第index个点，点的个数等于joints.size()+1
 * @param index
 * @return
 */
vec2 Joint::getIndexVertex(int index) {
    vec2 ret_end(origin_);
    float angle = 0.0f;
    for (int i = 0; i < index; i++) {
        angle += joints_[i].theta;
        vec2 vec(joints_[i].length, 0.0f);
        vec = global::rotateVec2(vec2(0.0f, 0.0f), vec, angle);
        ret_end = ret_end + vec;
    }
    return ret_end;
}
