#ifndef SIMPLE_CAMERA_H
#define SIMPLE_CAMERA_H


#include <iostream>
#include <fstream>
#include "../../../third_part_src/glm/glm.hpp"
#include "../../../third_part_src/glm/gtc/matrix_transform.hpp"
#include "../../../third_part_src/glm/gtx/string_cast.hpp"
#include <glad/glad.h>

#include <iomanip>      // std::setprecision
// 定义移动方向
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
// 定义预设常量
const GLfloat YAW = 0.0f;
const GLfloat PITCH = 10.0f;
const GLfloat SPEED = 2000.0f;
const GLfloat MOUSE_SENSITIVTY = 0.05f;
const GLfloat MOUSE_ZOOM = 999.0f;
const float  MAX_PITCH_ANGLE = 89.0f; // 防止万向锁

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 1000.0),
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0),
		GLfloat yaw = YAW, GLfloat pitch = PITCH)
		:position(pos), forward(0.0, 0.0, 0.0), viewUp(up),
		moveSpeed(SPEED), mouse_zoom(45.0f), mouse_sensitivity(MOUSE_SENSITIVTY),
		yawAngle(yaw), pitchAngle(pitch)
	{
		this->updateCameraVectors();
	}

	Camera(glm::vec3 pos, glm::vec3 forwd, glm::vec3 up):
		position(pos), forward(forwd), viewUp(up),
		moveSpeed(SPEED), mouse_zoom(45.0f), mouse_sensitivity(MOUSE_SENSITIVTY),
		yawAngle(YAW), pitchAngle(PITCH)
	{
		//this->updateCameraVectors();
		glm::vec3 side;
		side.x = cos(glm::radians(this->yawAngle));
		side.y = 0;
		side.z = -sin(glm::radians(this->yawAngle));
		this->side = glm::normalize(side);
	}

public:
	// change view matrix
	glm::vec3 change_position_sphere()
	{
	 	GLfloat radius = 6000.0f;
	    GLfloat theta = glfwGetTime(), phi = glfwGetTime() / 2.0f;
	    GLfloat xPos = radius * sin(theta) * cos(phi);
	    GLfloat yPos = radius * sin(theta) * sin(phi);
	    GLfloat zPos = radius * cos(theta);
    	return glm::vec3(xPos, yPos, zPos);
	}

	glm::vec3 change_position_circle()
	{
		GLfloat radius = 6000.0f;
	    GLfloat xPos = radius * cos(glfwGetTime());
	    GLfloat zPos = radius * sin(glfwGetTime());
	    return glm::vec3(xPos, 0.0f, zPos);
	}


	// 获取视变换矩阵
	glm::mat4 getViewMatrix()
	{
		#if 0
		//add for change view
		//this->position = change_position_sphere();
		this->position = change_position_circle();
		#endif

		return glm::lookAt(this->position, this->position + this->forward, this->viewUp);
	}
	// 处理键盘按键后方向移动
	void handleKeyPress(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->moveSpeed * deltaTime;
		switch (direction)
		{
		case FORWARD:
			this->position += this->forward * velocity;
			break;
		case BACKWARD:
			this->position -= this->forward * velocity;
			break;
		case LEFT:
			this->position -= this->side * velocity;
			break;
		case RIGHT:
			this->position += this->side * velocity;
			break;
		default:
			break;
		}
	}
	// 处理鼠标移动
	void handleMouseMove(GLfloat xoffset, GLfloat yoffset)
	{
		xoffset *= this->mouse_sensitivity; // 用鼠标灵敏度调节角度变换
		yoffset *= this->mouse_sensitivity;

		this->pitchAngle += yoffset;
		this->yawAngle += xoffset;

		this->normalizeAngle();
		this->updateCameraVectors();
	}
	// 处理鼠标滚轮缩放 保持在[1.0, MOUSE_ZOOM]之间
	void handleMouseScroll(GLfloat yoffset)
	{
		if (this->mouse_zoom >= 1.0f && this->mouse_zoom <= MOUSE_ZOOM)
			this->mouse_zoom -= this->mouse_sensitivity * yoffset;
		if (this->mouse_zoom <= 1.0f)
			this->mouse_zoom = 1.0f;
		if (this->mouse_zoom >= MOUSE_ZOOM)
			this->mouse_zoom = MOUSE_ZOOM;
	}
	// 使pitch yaw角度保持在合理范围内
	void normalizeAngle()
	{
		if (this->pitchAngle > MAX_PITCH_ANGLE)
			this->pitchAngle = MAX_PITCH_ANGLE;
		if (this->pitchAngle < -MAX_PITCH_ANGLE)
			this->pitchAngle = -MAX_PITCH_ANGLE;
		if (this->yawAngle < 0.0f)
			this->yawAngle += 360.0f;
	}
	// 计算forward side向量
	void updateCameraVectors()
	{
		glm::vec3 forward;
		forward.x = -sin(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		forward.y = sin(glm::radians(this->pitchAngle));
		forward.z = -cos(glm::radians(this->yawAngle)) * cos(glm::radians(this->pitchAngle));
		this->forward = glm::normalize(forward);

		glm::vec3 side;
		side.x = cos(glm::radians(this->yawAngle));
		side.y = 0;
		side.z = -sin(glm::radians(this->yawAngle));
		this->side = glm::normalize(side);
	}
public:
	glm::vec3 forward,up, side, viewUp, position; // 相机属性
	GLfloat yawAngle, pitchAngle; // 欧拉角
	GLfloat moveSpeed, mouse_sensitivity, mouse_zoom; // 相机选项
};

#endif
