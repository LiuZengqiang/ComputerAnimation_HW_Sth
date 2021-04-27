//
// Created by Sth on 2021/4/21.
//
#include "Joint.h"
#include "globalFunction.h"
#include <iostream>

Joint::Joint(const vector<float> &joints_length, unsigned int scr_width, unsigned int scr_height) {
    screen_width_ = scr_width;
    screen_height_ = scr_height;
    joints_length_.assign(joints_length.begin(), joints_length.end());
    vec2 pre(origin_);
    vec2 dir(0.0f, 1.0f);
    for (int i = 0; i < joints_length_.size(); i++) {
        total_length_ += joints_length_[i];
        vec2 ori(pre);
        joints_.push_back(JointNode(i, ori, ori + dir * joints_length[i]));
        pre = vec2(pre + dir * joints_length[i]);
    }
    transfer_matrix_.resize(joints_.size());
    for (int i = 0; i < transfer_matrix_.size(); i++) {
        transfer_matrix_[i] = getCDF(i);
    }
}

// cyclic coordinate decent
// 4 3 2 1
void Joint::updateJointsCCD() {
    joint_index = 0;
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        // 可以到达，迭代计算
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
    vec2 ori = joints_[ori_idx].origin;
    vec2 pre_des = ori;
    for (int i = ori_idx; i < joints_.size(); i++) {
        joints_[i].origin = pre_des;
        joints_[i].destination = global::rotateVec2(ori, joints_[i].destination, rotate_angle);
        pre_des = joints_[i].destination;
    }
}

vector<vec3> Joint::getVertices() {
    vector<vec3> ret;
    for (int i = 0; i < joints_.size(); i++) {
        ret.push_back(vec3(joints_[i].origin.x / screen_width_ * 2, joints_[i].origin.y / screen_height_ * 2, 0.0f));
//        cout << "(" << joints_[i].origin.x << "," << joints_[i].origin.y << ")";
    }
//    cout << endl;
    ret.push_back(
            vec3(joints_.back().destination.x / screen_width_ * 2, joints_.back().destination.y / screen_height_ * 2,
                 0.0f));
    return ret;
}

void Joint::updateJointsStep() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= total_length_) {
        updateJointsSL();
        // 可以到达，迭代计算
    } else {

        bool has_resolution = false;
        int i = 0;
        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            int j = joints_.size() - 1 - ((joint_index++) % joints_.size());
            vec2 end_position = joints_.back().destination;
            cout << "end_position:(" << end_position.x << "," << end_position.y << ")" << endl;

            float end_target_dis = global::getDis(end_position, target_);
            if (end_target_dis <= threshold_) {
                has_resolution = true;
                break;
            }
            vec2 ori_end = joints_.back().destination - joints_[j].origin;
            cout << "j:" << j << endl;
            cout << "ori_end:(" << ori_end.x << "," << ori_end.y << ")" << endl;
            vec2 ori_target = target_ - joints_[j].origin;
            cout << "target:(" << target_.x << "," << target_.y << ")" << endl;
            cout << "ori_target:(" << ori_target.x << "," << ori_target.y << ")" << endl;
            float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
            float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
            float rotate_angle = acos(global::clampFloat(cos));

            if (sin < 0.0f) {
                rotate_angle *= -1;
            }
            cout << "angle:" << rotate_angle * 180 / 3.141592654f << endl;
            // todo:
//            if (glm::dot(getEnd() - origin_, target_ - origin_) <= global::Epsilon) {
//                rotate_angle /= 2.0f;
////                    cout << "----- divide 2 -----" << endl;
//            }
//                rotate_angle /= 2.0f;
            rotateJoints(j, rotate_angle);

        }

        cout << "iterator times:" << i << endl;
    }

}

void Joint::updateJointsCJD() {
    joint_index = 0;
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

// combine CCD and CJD
//
//void Joint::updateJointsCCJD() {
//    joint_index = 0;
//    // 不能达到 直接绘制一个直线
//    if (global::getDis(target_, origin_) >= total_length_) {
//        updateJointsSL();
//        // 可以到达，迭代计算
//    } else {
//        int cnt = 0;
//        bool has_resolution = false;
//        // 每次迭代
//        for (int i; i < max_iterator_ && has_resolution == false; i++) {
//            // 迭代各个关节
//            enum ROTATE_DIRECTION {
//                CLOCKWISE,
//                ANTICLOCKWISE,
//                UNKNOWN
//            } rotate_direction = ROTATE_DIRECTION::UNKNOWN;
//
//            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {
//
//                vec2 end_position = joints_.back().destination;
//                float end_target_dis = global::getDis(end_position, target_);
//                if (end_target_dis <= threshold_) {
//                    has_resolution = true;
//                    break;
//                }
//                vec2 ori_end = joints_.back().destination - joints_[j].origin;
//
//                vec2 ori_target = target_ - joints_[j].origin;
//
//                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
//                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
//                float rotate_angle = acos(global::clampFloat(cos));
//
//                if (sin < 0.0f) {
//                    rotate_angle *= -1;
//                }
//                ROTATE_DIRECTION now_rotate_direction =
//                        rotate_angle >= 0.0f ? ROTATE_DIRECTION::ANTICLOCKWISE : ROTATE_DIRECTION::ANTICLOCKWISE;
//                if (rotate_direction == ROTATE_DIRECTION::UNKNOWN || rotate_direction == now_rotate_direction) {
//                    rotate_direction = now_rotate_direction;
//                    rotateJoints(j, rotate_angle);
//                    cnt++;
//                } else {
//                    for (int k = j; k < joints_.size(); k++) {
//
//                        cnt++;
//                        if (updateSingleJoint(k,)) {
//                            has_resolution = true;
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//        cout << "iterator times:" << cnt << endl;
//    }
//}

bool Joint::updateSingleJoint(int origin_index, float &rotate_angle) {

    if (global::getDis(joints_.back().destination, target_) <= threshold_) {
        return true;
    }
    vec2 ori_end = joints_.back().destination - joints_[origin_index].origin;
    vec2 ori_target = target_ - joints_[origin_index].origin;
    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
    rotate_angle = acos(global::clampFloat(cos));
    if (sin < 0.0f) {
        rotate_angle *= -1;
    }
    if (abs(rotate_angle) <= global::Epsilon) {
        rotate_angle = global::Epsilon * 10;
    }
    return global::getDis(joints_.back().destination, target_) <= threshold_;
}

void Joint::updateJointsCC() {
    joint_index = 0;
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
    vec2 direction = glm::normalize(target_ - origin_);
    vec2 ori = origin_;
    for (int i = 0; i < joints_.size(); i++) {
        joints_[i].origin = ori;
        joints_[i].destination = direction * joints_length_[i] + ori;
        ori = joints_[i].destination;
    }
}

void Joint::updateJointsMCD() {
    joint_index = 0;
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
    joint_index = 0;
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