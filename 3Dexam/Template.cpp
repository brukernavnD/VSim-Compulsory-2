#include <iostream>
#include <vector>

#include "Actors/BSplineTensorProductSurface.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "Core/Camera.h"
#include "Core/Helpers.h"
#include "Core/World.h"
#include "Core/Shader/Shaders.h"

#include "Core/PlayerController.h"
#include "Actors/ContainerBox.h"
#include "Actors/PointCloudSurface.h"

// callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double XPos, double YPos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//the game world
World GameWorld;

//the shader program
unsigned int ShaderProgram;

//the model key location for the shader program
GLint ModelKeyLocation;

//function to create a glfw window
GLFWwindow* MyCreateWindow()
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create the window
	GLFWwindow* Window = glfwCreateWindow(WindowWidth, WindowHeight, "Exam", nullptr, nullptr);

    //set the window to the current context
	glfwMakeContextCurrent(Window);

    //set callback functions
	glfwSetFramebufferSizeCallback(Window, framebuffer_size_callback);
    glfwSetKeyCallback(Window, key_callback);
	glfwSetCursorPosCallback(Window, mouse_callback);
    glfwSetScrollCallback(Window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //glad: load all OpenGL function pointers
	gladLoadGLLoader(GLADloadproc(glfwGetProcAddress));

    //create the shader program
    ShaderProgram = Shaders::CreateShader();

    //use the shader program
    glUseProgram(ShaderProgram);

    //set the model key location
    ModelKeyLocation = glGetUniformLocation(ShaderProgram, ModelKey.c_str());

    return Window;
}

//the window our game will be displayed in
GLFWwindow* GameWindow = MyCreateWindow();

int main()
{
	//seed the random number generator
	srand(time(nullptr));

	//set the camera position
	GameWorld.GetPlayerCamera()->Position = { CBoxCen.x + CBoxSize.x / 2, CBoxCen.y + CBoxSize.y / 2 + 50, CBoxCen.z + CBoxSize.z / 2 + 50 };

	//create the container box
	ContainerBox_ ContainerBox = ContainerBox_(CBoxCen, CBoxSize);

	//create the point cloud surface
	PointCloudSurface_ PointCloudSurface = PointCloudSurface_(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));

	//create the b-spline tensor product surface
	BSplineTensorProductSurface_ BSplineTensorProductSurface = BSplineTensorProductSurface_(glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));

	//add the container box to the world's terrain
	GameWorld.Boxes.emplace_back(&ContainerBox);

	//add the point cloud to the world's list of objects
	GameWorld.WorldObjects.emplace_back(&PointCloudSurface);

    //add the b-spline tensor product surface
	GameWorld.WorldObjects.emplace_back(&BSplineTensorProductSurface);

    //add the container box class to the renderer
	GetWorld()->Renderer.AddBuffers(ContainerBox_::CreateVertices(), ContainerBox_::CreateIndices());

	//add the point cloud class to the renderer
	GetWorld()->Renderer.AddBuffers(PointCloudSurface_::CreateVertices(), PointCloudSurface_::CreateIndices());

	//add the b-spline tensor product surface class to the renderer
	GetWorld()->Renderer.AddBuffers(BSplineTensorProductSurface_::CreateVertices(), BSplineTensorProductSurface_::CreateIndices());

	//set the glpointsize for the point cloud
	glPointSize(2);

    //initialize the world
	GetWorld()->InitializeWorld();

    //enable depth testing
    glEnable(GL_DEPTH_TEST);

    // timing (from https://learnopengl.com/Getting-started/Camera)
	// time between current frame and last frame
	float lastFrame = 0;

    // render loop
    while (!glfwWindowShouldClose(GameWindow))
    {
        // per-frame time logic (from https://learnopengl.com/Getting-started/Camera)
        // --------------------
        const float currentFrame = glfwGetTime();
        const float DeltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //draw the background
        glClearColor(0.2f, 0.3f, 0.3f, 1);

        //clear the colour buffer
		glClear(GL_COLOR_BUFFER_BIT);

        //clear the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        //glBindVertexArray(VAO);
        glUseProgram(ShaderProgram);

        //(modified)from https://learnopengl.com/Getting-started/Camera
        glm::mat4 projection = glm::perspective(glm::radians(GameWorld.GetPlayerCamera()->Zoom), WindowWidth / WindowHeight, 0.1f, CullingDistance);
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, ProjectionKey.c_str()), 1, false, &projection[0][0]);

        // camera/view transformation
        glm::mat4 View = GameWorld.GetPlayerCamera()->GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram,ViewKey.c_str()), 1, false, &View[0][0]);

    	//model matrix
        glm::mat4 model = glm::mat4(1);

        //set the model matrix uniform
        glUniformMatrix4fv(glGetUniformLocation(ShaderProgram, ModelKey.c_str()), 1, false, &model[0][0]);

        //do the tick and rendering for this frame
        GameWorld.UpdateWorld(DeltaTime);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(GameWindow);
        glfwPollEvents();
    }

    //// optional: de-allocate all resources once they've outlived their purpose:
    //glDeleteVertexArrays(1, &VAO);
    //glDeleteBuffers(1, &VBO);
    //glDeleteProgram(SHADER_REF);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

//called whenever the window is resized
void framebuffer_size_callback(GLFWwindow* window, const int width, const int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    std::cout << " windows resized with " << width << " Height " << height << '\n';
}

//called whenever the mouse is moved
void mouse_callback(GLFWwindow* window, const double XPos, const double YPos)
{
	GetWorld()->PlayerController->ProcessMouseInput(float(XPos), float(YPos));
}

//called whenever a key is pressed
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GetWorld()->PlayerController->ProcessKeyboardInput(window, key, scancode, action, mods);
}

//called whenever the mouse scroll wheel is moved
void scroll_callback(GLFWwindow* window, double xoffset, const double yoffset)
{
    GetWorld()->PlayerController->ProcessMouseScroll(float(yoffset));
}