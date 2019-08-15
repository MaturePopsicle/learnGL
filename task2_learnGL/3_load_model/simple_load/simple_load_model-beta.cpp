#include "includes/simple_model.h"
#include <glad/glad.h>
// 引入GLFW库
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "../../third_part_src/stb_image.h"

#include "../../third_part_src/glm/glm.hpp"
#include "../../third_part_src/glm/gtc/matrix_transform.hpp"
#include "../../third_part_src/glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <cstdlib>


//#define LOAD_RABIT
#define LOAD_CAR
#define DBG_CODE
//#define ALL_LAOD


// 包含着色器加载库
#include "includes/simple_shader.h"
// 包含相机控制辅助类
#include "includes/simple_camera.h"
//#include "camera.h"
// 包含简单的obj读取文件
#include "includes/simple_model.h"

using namespace std;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 键盘回调函数原型声明
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// 鼠标移动回调函数原型声明
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
// 鼠标滚轮回调函数原型声明
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// 场景中移动
void do_movement();

//add by Sam 2019-07-31
static unsigned int load_textrue(const char * filename);

//just for load dds file
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
static unsigned int loadDDS(const char * filename);

// 定义程序常量
const int WINDOW_WIDTH = 800, WINDOW_HEIGHT = 600;
// 用于相机交互参数
GLfloat lastX = WINDOW_WIDTH / 2.0f, lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouseMove = true;
bool keyPressedStatus[1024]; // 按键情况记录
GLfloat deltaTime = 0.0f; // 当前帧和上一帧的时间差
GLfloat lastFrame = 0.0f; // 上一帧时间
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));


#ifdef DBG_CODE
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3000.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
#endif

int main(int argc, char** argv)
{

	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}

	// 开启OpenGL 3.3 core profile
	std::cout << "Start OpenGL core profile version 3.3" << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
		"Demo of loading obj model", NULL, NULL);
	if (!window)
	{
		std::cout << "Error::GLFW could not create winddow!" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);

	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 注册鼠标事件回调函数
	// glfwSetCursorPosCallback(window, mouse_move_callback);
	// 注册鼠标滚轮事件回调函数
	glfwSetScrollCallback(window, mouse_scroll_callback);
	// 鼠标捕获 停留在程序内
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 初始化GLEW 获取OpenGL函数
	// glewExperimental = GL_TRUE; // 让glew获取所有拓展函数
	// GLenum status = glewInit();
	// if (status != GLEW_OK)
	// {
	// 	std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
	// 		<< " error string:" << glewGetErrorString(status) << std::endl;
	// 	glfwTerminate();
	// 	return -1;
	// }

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

	// 设置视口参数
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Section1 从obj文件加载数据
	std::vector<Vertex> vertData;
	// if (!ObjLoader::loadFromFile("../cube/cube.obj", vertData))
	// if (!ObjLoader::loadFromFile("./nanosuit.obj", vertData))
#ifdef LOAD_CAR
	if (!ObjLoader::loadFromFile("./models/car/car3da.obj", vertData))
#endif
#ifdef LOAD_RABIT
	if (!ObjLoader::loadFromFile("./models/Rabbit/Rabbit.obj", vertData))
#endif
	{
		std::cerr << "Could not load obj model, exit now.";
		std::system("pause");
		exit(-1);
	}
	// Section2 准备纹理
#ifdef LOAD_CAR
	GLint textureId = load_textrue("./models/car/qichen.png");
#endif
#ifdef LOAD_RABIT
	GLint textureId = load_textrue("./models/Rabbit/Rabbit_D.tga");
#endif


	/************************* load wheel and light *****************************/
	std::vector<Vertex> vertData_wheel;
	std::vector<Vertex> vertData_light;

	if (!ObjLoader::loadFromFile("./models/car/car3dw.obj", vertData_wheel))
	{
		std::cerr << "Could not load obj model, exit now.";
		std::system("pause");
		exit(-1);
	}
	if (!ObjLoader::loadFromFile("./models/car/car3dd.obj", vertData_wheel))
	{
		std::cerr << "Could not load obj model, exit now.";
		std::system("pause");
		exit(-1);
	}


	// Section3 建立Mesh对象
	Mesh mesh(vertData, textureId);
	//Mesh mesh(vertData, 0);

#ifdef ALL_LAOD
	unsigned int VAO_wheel;
	unsigned int VBO_wheel;
	glGenVertexArrays(1, &VAO_wheel);
	glGenBuffers(1, &VBO_wheel);
	glBindVertexArray(VAO_wheel);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_wheel);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(vertData_wheel.size()),
		&vertData_wheel[0], GL_STATIC_DRAW);
	glVertexAttribPointer(VAO_wheel, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(VAO_wheel);

	unsigned int VAO_light;
	unsigned int VBO_light;
	glGenVertexArrays(1, &VAO_light);
	glGenBuffers(1, &VBO_light);
	glBindVertexArray(VAO_light);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_light);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*(vertData_light.size()),
		&vertData_light[0], GL_STATIC_DRAW);
	glVertexAttribPointer(VAO_light, 3, GL_FLOAT, GL_FALSE,
			sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(VAO_light);


	printf("[---] VAO_light:%d VBO_light:%d\n", VAO_light, VBO_light);
	printf("[---] VA0_light:%d VBO_light:%d\n", VAO_wheel, VBO_wheel);
#endif

	//wheel positions
	glm::vec3 wheel_positions[] = {
		glm::vec3(835.979f, 1535.782f, 345.872f), //wheel_RF
        glm::vec3(835.979f, -1409.863f, 345.872f), //wheel_RB
        glm::vec3(-847.535f, 1535.782f, 345.872f), //wheel_LF
        glm::vec3(-847.535f, -1409.863f, 345.872f) //wheel_LB

	};

	/************************ add wheel and light vertices ****************/



	// Section4 准备着色器程序
	Shader shader("./shaders/simple_cube.vert", "./shaders/simple_cube.frag");

	glEnable(GL_DEPTH_TEST);

	// bool rotaOne = true;
	//glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		do_movement(); // 根据用户操作情况 更新相机属性

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
#if 0
		glm::mat4 projection = glm::perspective(camera.mouse_zoom,
			(GLfloat)(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 10000.0f); // 投影矩阵
		glm::mat4 view = camera.getViewMatrix(); // 视变换矩阵
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "projection"),
			1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "view"),
			1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
#ifdef LOAD_CAR
		float angle = -60.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle),
		// 	glm::vec3(0.0f, 0.0f, 1.0f));
#endif
#ifdef LOAD_RABIT
		float angle = -60.0f;
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle),
		 	glm::vec3(0.0f, 1.0f, 0.0f));
#endif
#endif

#ifdef DBG_CODE
     	glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 10000.0f);

        unsigned int viewLoc = glGetUniformLocation(shader.programId, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        unsigned int projeLoc = glGetUniformLocation(shader.programId, "projection");
        glUniformMatrix4fv(projeLoc, 1, GL_FALSE, &projection[0][0]);

        glm::mat4 model;
        //model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f;
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        model = glm::rotate(model, glm::radians(-60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        //add transform mat4
        // glm::vec3 wheel_RF(835.979f, 1535.782f, 345.872f);
        // glm::vec3 wheel_RB(835.979f, -1409.863f, 345.872f);
        // glm::vec3 wheel_LF(-847.535f, 1535.782f, 345.872f);
        // glm::vec3 wheel_LB(-847.535f, -1409.863f, 345.872f);

        //model = glm::translate(model, wheel_RF);
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-60.0f),
        //	glm::vec3(0.0f, 0.0f, 1.0f));
        //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        unsigned int modelLoc = glGetUniformLocation(shader.programId, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
#endif

		glUniformMatrix4fv(glGetUniformLocation(shader.programId, "model"),
			1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(0);
#ifdef ALL_LAOD
		//glBindVertexArray(VAO_light);
		//glDrawArrays(GL_TRIANGLES, 0, vertData_light.size());
		glBindVertexArray(VAO_wheel);
		glDrawArrays(GL_TRIANGLES, 0, vertData_wheel.size());
#endif
		glUseProgram(0);
		// 这里填写场景绘制代码
		mesh.draw(shader); // 绘制物体

		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	glfwTerminate();
	return 0;
}





void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keyPressedStatus[key] = true;
		else if (action == GLFW_RELEASE)
			keyPressedStatus[key] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
	}
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // 首次鼠标移动
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}
	// printf("(%f, %f)\n", xpos, ypos);

	GLfloat xoffset = xpos - lastX;
	//GLfloat yoffset = lastY - ypos;
	GLfloat yoffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}


// 由相机辅助类处理鼠标滚轮控制
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}


// 由相机辅助类处理键盘控制
void do_movement()
{

	if (keyPressedStatus[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keyPressedStatus[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keyPressedStatus[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
}


static unsigned int loadDDS(const char * filename)
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
		std::cout << "Error::loadDDs, format is not dds: "
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

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


static unsigned int load_textrue(const char * filename)
{
	unsigned int texture;
	int width, height, nrChannels;

	glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        cout << "failed to load texture" << endl;
    }
    stbi_image_free(data);

	return texture;
}
