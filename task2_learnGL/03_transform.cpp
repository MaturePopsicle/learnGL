
#include <iostream>
#include "./third_part_src/glm/glm.hpp"
#include "./third_part_src/glm/gtc/matrix_transform.hpp"
#include "./third_part_src/glm/gtc/type_ptr.hpp"

using namespace std;









#if 1
/**
 * just for test glm lib
 */
int main()
{
    glm::vec4 vec(1.0f, 0, 0, 1.0f);

    glm::mat4 trans;
    trans = glm::translate(trans, glm::vec3(2.0f, 0, 0));

    vec = trans * vec;

    cout << vec.x << vec.y << vec.z << endl;

    return 0;
}
#endif