#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../third_part_src/glm/glm.hpp"
#include "../third_part_src/glm/gtc/matrix_transform.hpp"
#include "../third_part_src/glm/gtc/type_ptr.hpp"

#include "../tools_src/shader.h"
#include "../tools_src/camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../third_part_src/stb_image.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
static bool loadFromFile(const char* path, std::vector<Vertex>& vertData);
static int loadDDS(const char * filename);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;
	glm::vec3 normal;
};

struct VertexCombineIndex
{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GL model", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // -------------------------------------------------
    // [1] load a obj
    vector<Vertex> vertData;
    bool ret = loadFromFile("./cube/cube.obj", vertData);
    // [2] load texture
    int textureID;
    textureID = loadDDS("./cube/cube.dds");
    // [3] build mesh object


    // [4] prepare shader
    Shader m_shader("./shader.vs", "./shader.fs");


    m_shader.use();
    m_shader.setInt("ourTexture1", 0);
    m_shader.setInt("ourTexture2", 1);


    
    //Model m_model("./model/car3da.obj");

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        m_shader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(m_shader.ID, "projection"), 
                1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader.ID, "view"), 
                1, GL_FALSE, &view[0][0]);
        

        //m_shader.setMat4("projection", projection);
        //m_shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
        //m_shader.setMat4("model", model);
        glUniformMatrix4fv(glGetUniformLocation(m_shader.ID, "model"), 
                1, GL_FALSE, &model[0][0]);
        //m_model.Draw(m_shader);

        for(unsigned int i = 0; i < 10; i++) {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelLoc = glGetUniformLocation(m_shader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}



void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}


static bool loadFromFile(const char* path, std::vector<Vertex>& vertData)
{
    vector<VertexCombineIndex> vertComIndices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec3> temp_textCoords;
    vector<glm::vec3> temp_normals;

    std::ifstream file(path);
    if(!file) {
        cerr << "obj file load failed, path: " << path << endl;
        return false;
    }

    string line;
    while(getline(file, line)) {
        //get vertices textcoords
        if(line.substr(0, 2) == "vt") 
        {
            istringstream s(line.substr(2));
            glm::vec2 v;
            s >> v.x; 
            s >> v.y;
            v.y = -v.y;  // 注意这里加载的dds纹理 要对y进行反转
            temp_textCoords.push_back(v);
        }
        else if (line.substr(0, 2) == "vn") // 顶点法向量数据
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            temp_normals.push_back(v);
        }
        else if (line.substr(0, 1) == "v") // 顶点位置数据
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x; s >> v.y; s >> v.z;
            temp_vertices.push_back(v);
        }
        else if (line.substr(0, 1) == "f") // 面数据
        {
            std::istringstream vtns(line.substr(2));
            std::string vtn;
            while (vtns >> vtn) // 处理一行中多个顶点属性
            {
                VertexCombineIndex vertComIndex;
                std::replace(vtn.begin(), vtn.end(), '/', ' ');
                std::istringstream ivtn(vtn);
                if (vtn.find("  ") != std::string::npos) // 没有纹理数据
                {
                    std::cerr << "Error:ObjLoader, no texture data found within file:"
                        << path << std::endl;
                    return false;
                }
                ivtn >> vertComIndex.posIndex
                    >> vertComIndex.textCoordIndex
                    >> vertComIndex.normIndex;
                
                vertComIndex.posIndex--;
                vertComIndex.textCoordIndex--;
                vertComIndex.normIndex--;
                vertComIndices.push_back(vertComIndex);
            }
        }
        else if (line[0] == '#') // 注释忽略
        { }
        else  
        {
            // 其余内容 暂时不处理
        }

        for (std::vector<GLuint>::size_type i = 0; i < vertComIndices.size(); ++i)
		{
			Vertex vert;
			VertexCombineIndex comIndex = vertComIndices[i];

			vert.position = temp_vertices[comIndex.posIndex];
			vert.texCoords = temp_textCoords[comIndex.textCoordIndex];
			vert.normal = temp_normals[comIndex.normIndex];

			vertData.push_back(vert);
		}

		return true;
    }

}


static int loadDDS(const char * filename)
{
    /* try to open the file */
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file){
        std::cout << "Error::loadDDs, could not open:" 
            << filename << "for read." << std::endl;
        return 0;
    }

    /* verify the type of file */
    char filecode[4];
    file.read(filecode, 4);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        std::cout << "Error::loadDDs, format is not dds :"
            << filename  << std::endl;
        file.close();
        return 0;
    }

    /* get the surface desc */
    char header[124];
    file.read(header, 124);

    unsigned int height = *(unsigned int*)&(header[8]);
    unsigned int width = *(unsigned int*)&(header[12]);
    unsigned int linearSize = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC = *(unsigned int*)&(header[80]);


    char * buffer = NULL;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = new char[bufsize];
    file.read(buffer, bufsize);
    /* close the file pointer */
    file.close();

    unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC)
    {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        break;
    default:
        delete[] buffer;
        return 0;
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
            0, size, buffer + offset);

        offset += size;
        width /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if (width < 1) width = 1;
        if (height < 1) height = 1;

    }

    delete[] buffer;

    return textureID;
}