#include "engine.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "common.hpp"
#include "camera.hpp"


namespace camera {
    static mat4 view_matrix = glm::mat4(1.0f);
    static mat4 projection_matrix = glm::mat4(1.0f);

    static vec3 position = vec3(0.0f);
    static vec3 up = vec3(0.0f, 1.0f, 0.0f);
    static vec3 right = vec3(1.0f, 0.0f, 0.0f);
    static vec3 front = vec3(0.0f, 0.0f, 1.0f);

    static int screenWidth = 640;
    static int screenHeight = 480;
    static float current_near = 0.1f;
    static float current_far = 100.0f;
    static float current_fov = 60.0f;

    bool perspective = true;

    static float yaw = 0.0f;
	static float pitch = 0.0f;
    
    static void updateView(){
        view_matrix[0][0] = right.x;
        view_matrix[0][1] = up.x;
        view_matrix[0][2] = front.x;
        view_matrix[0][3] = 0.0f;
        view_matrix[1][0] = right.y;
        view_matrix[1][1] = up.y;
        view_matrix[1][2] = front.y;
        view_matrix[1][3] = 0.0f;
        view_matrix[2][0] = right.z;
        view_matrix[2][1] = up.z; 
        view_matrix[2][2] = front.z;
        view_matrix[2][3] = 0.0f;
        view_matrix[3][0] = -glm::dot(right, position);
        view_matrix[3][1] = -glm::dot(up, position);
        view_matrix[3][2] = -glm::dot(front, position);
        view_matrix[3][3] = 1.0f;
    }

    void setProjection(float fov, float near, float far){
        perspective = true;
        current_fov = fov;
        current_near = near;
        current_far = far;

        float aspectXtoY = (float)screenWidth / (float)screenHeight;
        float fov_y = 2.0f * atan(aspectXtoY * (tan(fov * PI / 180.0f * 0.5f)));
        projection_matrix = glm::perspective(fov_y, aspectXtoY, near, far);

        //revert handedness swap so we look down positive z axis
        projection_matrix[2][2] = -projection_matrix[2][2];
        projection_matrix[2][3] = -projection_matrix[2][3];
        updateView();
    }

    void setOrthographic(float left, float right, float bottom, float top){
        perspective = false;
        //untested
        projection_matrix = glm::ortho(left, right, bottom, top);
    }

    mat4 getViewMatrix(){
        return view_matrix;
    }

    mat4 getProjectionMatrix(){
        return projection_matrix;
    }

    void setScreenSize(int w, int h){
        screenWidth = w;
        screenHeight = h;
        engine::setViewport(0, 0, w, h);
        if(perspective){
            camera::setProjection(current_fov, current_near, current_far);
        }
    }

    vec3 screenToWorld(int x, int y){
        // IN XY plane
        // we want to find a point on the X-Y plane so
        // therefore we project a random point in the z = 0 plane to find the corresponding
        // z value in NDC coordinates (note that depth buffer is different from NDC z and remapped to 0-1!!)
        vec4 test_point = vec4(0, 0, 0, 1);
        test_point = view_matrix * test_point;
        test_point = projection_matrix * test_point;
        test_point = test_point / test_point[3];

        // now we compute the correct NDCs, all [-1, 1]
        float nx = (float)x / (float)screenWidth * 2.0f - 1.0f;
        float ny = (1.0f - (float)y / (float)screenHeight) * 2.0f - 1.0f; 
        float nz = test_point.z;

        // now it's a simple projection back
        vec4 ret = vec4(nx, ny, nz, 1.0f);
        ret = glm::inverse(projection_matrix) * ret;
        ret = glm::inverse(view_matrix) * ret;
        return glm::vec3(ret) / ret.w;
    }

    void setPosition(vec3 pos){
        position = pos;
        updateView();
    }

    void addPosition(vec3 vel){
        //cout << "Cam pos " << glm::to_string(position) << endl;

        position += right * vel[0];
        position += up * vel[1];
        position += front * vel[2];
        //std::cout << glm::to_string(position) << std::endl;
        //std::cout << glm::to_string(front) << " | " << glm::to_string(right) << " | " << glm::to_string(up) << std::endl;
        updateView();
    }

    void setOrientation(float y, float p){
        //limit view angles
        yaw = y;
        pitch = p;

        yaw = fmod(yaw, 2.0f * PI);
        if(yaw < 0.0f){
            yaw += 2.0f * PI;
        }
        
        //grace value so we dont get view angle glitch
        if(pitch > 0.499f * PI){
            pitch = 0.499f * PI;
        }
        if(pitch < -0.499f * PI){
            pitch = -0.499f * PI;
        }
        
        //calcuate look at vector
        front.x = sin(yaw) * cos(pitch);
        front.y = sin(pitch);
        front.z = cos(yaw) * cos(pitch);

        //update basis
        front = glm::normalize(front);
        right = glm::normalize(glm::cross(vec3(.0f, 1.0f, .0f), front));
        up = glm::cross(front, right);

        //std::cout << "yaw " << yaw << std::endl;
        //std::cout << glm::to_string(front) << " | " << glm::to_string(right) << " | " << glm::to_string(up) << std::endl;

        updateView();
    }

    void addOrientation(float yaw_offset, float pitch_offset){
        setOrientation(yaw + yaw_offset, pitch + pitch_offset);
    }
}




