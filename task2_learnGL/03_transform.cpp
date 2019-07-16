
#include <iostream>
#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

using namespace std;



























/**
 * just for test glm lib
 */
int main_teslGLM()
{
    glm::vec4 vec(1.0f, 0, 0, 1.0f);

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(2.0f, 0, 0));

    vec = trans * vec;

    cout << vec.x << vec.y << vec.z << endl;

    return 0;
}