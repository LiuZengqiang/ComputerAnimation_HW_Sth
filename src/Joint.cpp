//
// Created by Sth on 2021/4/21.
//
#include "Joint.h"
#include "globalFunction.h"
#include <iostream>

void Joint::construct(const vector<float> &joints_length, const vector<pair<float, float>> &joints_angle_limit,
                      unsigned int scr_width,
                      unsigned int scr_height) {
    screen_width_ = scr_width;
    screen_height_ = scr_height;
    joints_length_.assign(joints_length.begin(), joints_length.end());
    joints_angle_limit_.assign(joints_angle_limit.begin(), joints_angle_limit.end());


    for (int i = 0; i < joints_length_.size(); i++) {
        total_length_ += joints_length_[i];
        if (i < joints_angle_limit_.size()) {
            joints_.push_back(JointNode(i, joints_length_[i], joints_angle_limit_[i]));
        } else {
            joints_.push_back(JointNode(i, joints_length_[i]));
        }
    }
    transfer_matrix_.resize(joints_.size());
    for (int i = 0; i < transfer_matrix_.size(); i++) {
        transfer_matrix_[i] = getCDF(i);
    }
}

// cyclic coordinate decent
int Joint::updateJointsCCD() {
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        return 1;
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
            updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
            has_resolution = global::getDis(getEndVertex(), target_) <= threshold_;
        }
        return i;
    }
}

void Joint::setTarget(const vec2 &target) {
    target_ = target;
}

void Joint::rotateJoints(int ori_idx, float rotate_angle) {

    for (int i = ori_idx; i < joints_.size(); i++) {
        joints_[i].theta += rotate_angle;
        if (ori_idx != 0) {
            joints_[i].theta = global::clampFloat(joints_[i].theta, joints_[ori_idx].theta_limit.first,
                                                  joints_[ori_idx].theta_limit.second);
        }
    }
}

vector<vec3> Joint::getVertices() {
    vector<vec3> ret;
    vec2 end_vertex(origin_);
    ret.push_back(vec3(end_vertex, 0.0f));
    float angle = 0.0f;
    for (int i = 0; i < joints_.size(); i++) {

        angle += joints_[i].theta;

        angle = global::clampAngel(angle);

        vec2 vec(joints_[i].length, 0.0f);
        vec = global::rotateVec2(vec2(0.0f, 0.0f), vec, angle);
        end_vertex = end_vertex + vec;
        ret.push_back(vec3((end_vertex.x * 2) / (screen_width_), (end_vertex.y * 2) / screen_height_, 0.0f));
    }
    return ret;
}

int Joint::updateJointsCJD() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        return 1;
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
            updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
            has_resolution = global::getDis(getEndVertex(), target_) <= threshold_;
        }
        return i;
    }
}

void Joint::updateSingleJoint(int origin_index, float &rotate_angle) {
    vec2 ori_vertex = getIndexVertex(origin_index);
    vec2 ori_end = getEndVertex() - ori_vertex;
    vec2 ori_target = target_ - ori_vertex;

    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
    rotate_angle = acos(global::clampFloat(cos));
    if (sin < 0.0f) {
        rotate_angle *= -1;
    }

    if (abs(rotate_angle) <= global::Epsilon) {
        rotate_angle = global::Epsilon * 100;
    }
}

// 4 3 2 1 2 3
int Joint::updateJointsCC() {
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        return 1;
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
            // 4,3,2,1,2,3
            int idx = indexes[i % indexes.size()];
            updateSingleJoint(idx, rotate_angle);
            rotateJoints(idx, rotate_angle);
            has_resolution = global::getDis(getEndVertex(), target_) <= threshold_;
        }
        return 1;
    }
}

void Joint::updateJointsSL() {
    vec2 direction = glm::normalize(target_ - origin_);
    float cos = 1.0f * glm::dot(direction, glm::vec2(1.0f, 0.0f));
    float sin = -1.0f * direction.y;
    float theta = acos(global::clampFloat(cos));
    if (sin > 0.0f) {
        theta *= -1.0f;
    }
    theta = global::clampAngel(theta);
    joints_[0].theta = theta;

    for (int i = 1; i < joints_.size(); i++) {
        joints_[i].theta = 0.0f;
    }
}

int Joint::updateJointsRCD() {
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        return 1;
    } else {
        vector<int> cnt(3, 0);
        int i = 0;
        float rotate_angle = 0.0f;
        bool has_resolution = false;
        int index = joints_.size() - 1;

        for (; i < max_iterator_ * joints_.size() && has_resolution == false; i++) {
            cnt[index]++;

            updateSingleJoint(index, rotate_angle);
            rotateJoints(index, rotate_angle);
            has_resolution = global::getDis(getEndVertex(), target_) <= threshold_;

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
        return i;
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
        angle = global::clampAngel(angle);
        vec2 vec(joints_[i].length, 0.0f);
        vec = global::rotateVec2(vec2(0.0f, 0.0f), vec, angle);
        ret_end = ret_end + vec;
    }
    return ret_end;
}

void Joint::updateJoints(METHODS method) {
    switch (method) {
        case METHODS::CCD:
            cout << "algorithm:CCD, iterators: " << updateJointsCCD() << ", error:"
                 << global::getDis(getEndVertex(), target_) << endl;
            break;
        case METHODS::CJD:
            cout << "algorithm:CJD, iterators: " << updateJointsCJD() << ", error:"
                 << global::getDis(getEndVertex(), target_) << endl;
            break;
        case METHODS::RCD:
            cout << "algorithm:RCD, iterators: " << updateJointsRCD() << ", error:"
                 << global::getDis(getEndVertex(), target_) << endl;
            break;
        case METHODS::CC:
            cout << "algorithm:CC, iterators: " << updateJointsCC() << ", error:"
                 << global::getDis(getEndVertex(), target_) << endl;
            break;
        default:
            cout << "algorithm:CCD, iterators: " << updateJointsCCD() << ", error:"
                 << global::getDis(getEndVertex(), target_) << endl;
            break;
    }
}

int Joint::getVerticesSize() {
    return joints_.size();
}

void Joint::setScreenSize(unsigned int scr_width, unsigned int scr_height) {
    screen_width_ = scr_width;
    screen_height_ = scr_height;
}
