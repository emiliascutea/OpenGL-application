#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraDirection = glm::normalize(cameraPosition - cameraTarget);

        //TODO - Update the rest of camera parameters
      

        this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
        this->cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);

    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraFrontDirection + cameraPosition, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;

        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            break;

        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        case MOVE_UP:
            cameraPosition += cameraUpDirection * speed;
            break;
        case MOVE_DOWN:
            cameraPosition -= cameraUpDirection * speed;
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        cameraFrontDirection = glm::normalize(front);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }

    void Camera::startVisualization(float universeAngle) {
       
        this->cameraPosition = glm::vec3(-50.0, 700.0, -48.0); // set the camera position up in the sky   
        this->cameraTarget = glm::vec3(0.0, 1.0, 0.0); // look on the y axis
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(universeAngle), glm::vec3(0, 1, 0)); // create the rotationMatrix with the universeAngle value
        cameraPosition = glm::vec4(rotationMatrix * glm::vec4(this->cameraPosition, 1)); // update camera position with the rotation
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition); // update the front direction to view the scene in front
    }   
}