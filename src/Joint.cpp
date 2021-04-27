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
    for (int i = 0; i < joints_length.size(); i++) {
        int id = i;
        float length = joints_length[i];
        vec2 ori(pre);
        joints_.push_back(JointNode(id, ori, ori + dir * length));
        pre = vec2(pre + dir * length);
    }
}

float Joint::getTotalLength() {
    float length = 0.0f;
    for (int i = 0; i < joints_.size(); i++) {
        length += joints_[i].getDis();
    }
    return length;
}

//cyclic coordinate decent
// cyclic coordinate decent
void Joint::updateJointsCCD() {
    joint_index = 0;
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
        // 可以到达，迭代计算
    } else {
        int cnt = 0;
        bool has_resolution = false;
        int i = 0;
        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {
                cnt++;
                vec2 end_position = joints_.back().destination;

//                cout << "end_position:(" << end_position.x << "," << end_position.y << ")" << endl;

                float end_target_dis = global::getDis(end_position, target_);
                if (end_target_dis <= threshold_) {
                    has_resolution = true;
                    break;
                }
                vec2 ori_end = joints_.back().destination - joints_[j].origin;
//                cout << "ori_end:(" << ori_end.x << "," << ori_end.y << ")" << endl;
                vec2 ori_target = target_ - joints_[j].origin;
//                cout << "ori_target:(" << ori_target.x << "," << ori_target.y << ")" << endl;
                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                float rotate_angle = acos(global::clampFloat(cos));

                if (sin < 0.0f) {
                    rotate_angle *= -1;
                }
//                cout << "angle:" << rotate_angle << endl;
//                // todo:
//                if (glm::dot(getEnd() - origin_, target_ - origin_) <= global::Epsilon) {
//                    rotate_angle /= 2.0f;
////                    cout << "----- divide 2 -----" << endl;
//                }
//                rotate_angle /= 2.0f;
                updateJoints(j, rotate_angle);
            }
        }

        cout << "iterator times:" << cnt << endl;
    }
}


vector<JointNode> Joint::getKeyVertex() {
    return joints_;
}

void Joint::setOrigin(const vec2 &origin) {
    origin_ = origin;
}

void Joint::setTarget(const vec2 &target) {
//    cout << "Joint::setTarget: (" << target.x << "," << target.y << ")" << endl;
    target_ = target;
}

void Joint::updateJoints(int ori_idx, float rotate_angle) {
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
    }
    ret.push_back(
            vec3(joints_.back().destination.x / screen_width_ * 2, joints_.back().destination.y / screen_height_ * 2,
                 0.0f));
    return ret;
}

vec2 Joint::getEnd() {
    if (joints_.empty()) {
        return vec2(0.0f, 0.0f);
    }
    return joints_.back().destination;
}

void Joint::updateJointsStep() {
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
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
            updateJoints(j, rotate_angle);

        }

        cout << "iterator times:" << i << endl;
    }

}

void Joint::updateJointsCJD() {
    joint_index = 0;
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
        // 可以到达，迭代计算
    } else {
        int cnt = 0;
        bool has_resolution = false;
        int i = 0;
        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            // 3,2,3,1,2,3
            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {
                for (int k = j; k < joints_.size(); k++) {
                    cnt++;
                    vec2 end_position = joints_.back().destination;
                    float end_target_dis = global::getDis(end_position, target_);
                    if (end_target_dis <= threshold_) {
                        has_resolution = true;
                        break;
                    }
                    vec2 ori_end = joints_.back().destination - joints_[k].origin;

                    vec2 ori_target = target_ - joints_[k].origin;

                    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                    float rotate_angle = acos(global::clampFloat(cos));

                    if (sin < 0.0f) {
                        rotate_angle *= -1;
                    }
//                // todo:
//                if (glm::dot(getEnd() - origin_, target_ - origin_) <= global::Epsilon) {
//                    rotate_angle /= 2.0f;
////                    cout << "----- divide 2 -----" << endl;
//                }
                    updateJoints(j, rotate_angle);
                }
            }
        }
        cout << "iterator times:" << cnt << endl;
    }
}

// combine CCD and CJD
//
void Joint::updateJointsCCJD() {
    joint_index = 0;
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
        // 可以到达，迭代计算
    } else {
        int cnt = 0;
        bool has_resolution = false;
        // 每次迭代
        for (int i; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            enum ROTATE_DIRECTION {
                CLOCKWISE,
                ANTICLOCKWISE,
                UNKNOWN
            } rotate_direction = ROTATE_DIRECTION::UNKNOWN;

            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {

                vec2 end_position = joints_.back().destination;
                float end_target_dis = global::getDis(end_position, target_);
                if (end_target_dis <= threshold_) {
                    has_resolution = true;
                    break;
                }
                vec2 ori_end = joints_.back().destination - joints_[j].origin;

                vec2 ori_target = target_ - joints_[j].origin;

                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                float rotate_angle = acos(global::clampFloat(cos));

                if (sin < 0.0f) {
                    rotate_angle *= -1;
                }
                ROTATE_DIRECTION now_rotate_direction =
                        rotate_angle >= 0.0f ? ROTATE_DIRECTION::ANTICLOCKWISE : ROTATE_DIRECTION::ANTICLOCKWISE;
                if (rotate_direction == ROTATE_DIRECTION::UNKNOWN || rotate_direction == now_rotate_direction) {
                    rotate_direction = now_rotate_direction;
                    updateJoints(j, rotate_angle);
                    cnt++;
                } else {
                    for (int k = j; k < joints_.size(); k++) {

                        cnt++;
                        if (updateSingleJoint(k)) {
                            has_resolution = true;
                            break;
                        }
                    }
                }
            }
        }
        cout << "iterator times:" << cnt << endl;
    }
}

bool Joint::updateSingleJoint(int origin_index) {
//    cout << "single back" << endl;

    vec2 end_position = joints_.back().destination;
    float end_target_dis = global::getDis(end_position, target_);

    if (end_target_dis <= threshold_) {
        return true;
    }
    vec2 ori_end = joints_.back().destination - joints_[origin_index].origin;
    vec2 ori_target = target_ - joints_[origin_index].origin;
    float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
    float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
    float rotate_angle = acos(global::clampFloat(cos));
    if (sin < 0.0f) {
        rotate_angle *= -1;
    }
    updateJoints(origin_index, rotate_angle);
    return false;
}

void Joint::updateJointsCC() {
    joint_index = 0;
    // 不能达到 直接绘制一个直线
    if (global::getDis(target_, origin_) >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
        // 可以到达，迭代计算
    } else {
        int cnt = 0;
        bool has_resolution = false;
        int i = 0;
        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            // 3,2,1,2
            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {

                cnt++;

                vec2 end_position = joints_.back().destination;
                float end_target_dis = global::getDis(end_position, target_);
                if (end_target_dis <= threshold_) {
                    has_resolution = true;
                    break;
                }
                vec2 ori_end = joints_.back().destination - joints_[j].origin;

                vec2 ori_target = target_ - joints_[j].origin;

                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                float rotate_angle = acos(global::clampFloat(cos));

                if (sin < 0.0f) {
                    rotate_angle *= -1;
                }
                updateJoints(j, rotate_angle);

            }

            for (int j = 1; j < joints_.size() - 1 && has_resolution == false; j++) {
                cnt++;
                vec2 end_position = joints_.back().destination;
                float end_target_dis = global::getDis(end_position, target_);
                if (end_target_dis <= threshold_) {
                    has_resolution = true;
                    break;
                }
                vec2 ori_end = joints_.back().destination - joints_[j].origin;

                vec2 ori_target = target_ - joints_[j].origin;

                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                float rotate_angle = acos(global::clampFloat(cos));

                if (sin < 0.0f) {
                    rotate_angle *= -1;
                }
                updateJoints(j, rotate_angle);

            }
        }
        cout << "iterator times:" << cnt << endl;
    }
}

void Joint::updateJointsDP() {
    joint_index = 0;
    float distance = global::getDis(target_, origin_);
    if (distance >= getTotalLength()) {
        vec2 direction = glm::normalize(target_ - origin_);
        vec2 ori = origin_;
        for (int i = 0; i < joints_.size(); i++) {
            joints_[i].origin = ori;
            joints_[i].destination = direction * joints_length_[i] + ori;
            ori = joints_[i].destination;
        }
    } else {
        int cnt = 0;
        bool has_resolution = false;
        int i = 0;

        // init target:
        vec2 dir = glm::normalize(getEnd() - origin_);
        vec2 temp_target = distance * dir;

        for (; i < max_iterator_ && has_resolution == false; i++) {
            // 迭代各个关节
            for (int j = joints_.size() - 1; j >= 0 && has_resolution == false; j--) {
                cnt++;
                vec2 end_position = joints_.back().destination;

//                cout << "end_position:(" << end_position.x << "," << end_position.y << ")" << endl;

                float end_target_dis = global::getDis(end_position, temp_target);
                if (end_target_dis <= threshold_) {
                    has_resolution = true;
                    break;
                }
                vec2 ori_end = joints_.back().destination - joints_[j].origin;
//                cout << "ori_end:(" << ori_end.x << "," << ori_end.y << ")" << endl;
                vec2 ori_target = temp_target - joints_[j].origin;
//                cout << "ori_target:(" << ori_target.x << "," << ori_target.y << ")" << endl;
                float cos = glm::dot(ori_end, ori_target) / (glm::length(ori_end) * glm::length(ori_target));
                float sin = (ori_end.x * ori_target.y) - (ori_end.y * ori_target.x);
                float rotate_angle = acos(global::clampFloat(cos));

                if (sin < 0.0f) {
                    rotate_angle *= -1;
                }
                updateJoints(j, rotate_angle);
            }
        }
        updateSingleJoint(0);
        cout << "iterator times:" << cnt + 1 << endl;
    }

}
