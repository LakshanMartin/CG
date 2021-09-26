#include "park.h"

#include <iostream>

// SETTING
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// CAMERA
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
glm::mat4 projection;
glm::vec3 lastPosition;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// TIMING
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int brightnessTimer = 0;
int lightTimer = 0;
int projectionTimer = 0;

// LIGHT
float amb = 1.0f;
float linearAtten[2] = {0.0014f, 0.045f};
float quadAtten[2] = {0.000007f, 0.0075f};
int attenIndex = 1;

// TOGGLES
bool brightToggle = false;
bool lightStay = false;
bool orthographic = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader shader("5.4.light_casters.vs", "5.4.light_casters.fs");
    Shader lampShader("5.4.lamp.vs", "5.4.lamp.fs");

    // SETUP TEXTURES -----------------------------------------------------------
    unsigned int noSpec = loadTexture(FileSystem::getPath("resources/textures/no_spec.png").c_str());
    unsigned int mildSpec = loadTexture(FileSystem::getPath("resources/textures/mild_spec.png").c_str());
    unsigned int highSpec = loadTexture(FileSystem::getPath("resources/textures/high_spec.png").c_str());
    unsigned int grassDiff = loadTexture(FileSystem::getPath("resources/textures/grass.jpg").c_str());
    unsigned int bballCourtDiff = loadTexture(FileSystem::getPath("resources/textures/bball_court.png").c_str());
    unsigned int treeTopDiff = loadTexture(FileSystem::getPath("resources/textures/tree_leaves.jpg").c_str());
    unsigned int treeTrunkDiff = loadTexture(FileSystem::getPath("resources/textures/tree_trunk.png").c_str());
    unsigned int bballPoleDiff = loadTexture(FileSystem::getPath("resources/textures/bball_pole.png").c_str());
    unsigned int bballBoardFrontDiff = loadTexture(FileSystem::getPath("resources/textures/bball_board_front.png").c_str());
    unsigned int bballBoardBackDiff = loadTexture(FileSystem::getPath("resources/textures/bball_board_back.png").c_str());
    unsigned int bballBoardEdgeDiff = loadTexture(FileSystem::getPath("resources/textures/bball_board_edge.png").c_str());
    unsigned int bballRingDiff = loadTexture(FileSystem::getPath("resources/textures/bball_ring.png").c_str());
    unsigned int manShoeDiff = loadTexture(FileSystem::getPath("resources/textures/man_shoe.png").c_str());
    unsigned int manLegsDiff = loadTexture(FileSystem::getPath("resources/textures/man_leg.png").c_str());
    unsigned int manTopBackDiff = loadTexture(FileSystem::getPath("resources/textures/man_top_back.png").c_str());
    unsigned int manTopDiff = loadTexture(FileSystem::getPath("resources/textures/man_top.png").c_str());
    unsigned int manArmDiff = loadTexture(FileSystem::getPath("resources/textures/man_arm.png").c_str());
    unsigned int manNeckDiff = loadTexture(FileSystem::getPath("resources/textures/man_neck.png").c_str());
    unsigned int manFaceDiff = loadTexture(FileSystem::getPath("resources/textures/man_face.png").c_str());
    unsigned int manHeadTopDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_top.png").c_str());
    unsigned int manHeadBackDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_back.png").c_str());
    unsigned int manHeadLeftDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_left.png").c_str());
    unsigned int manHeadRightDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_right.png").c_str());
    


    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	//vertex coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal vectors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
    

    // shader configuration
    // --------------------
    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_delay();

        // input
        // -----
        processInput(window);        

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        shader.use();

        if(lightStay)
        {
            shader.setVec3("light.position", lastPosition);
        }
        else
        {
            shader.setVec3("light.position", camera.Position);
        }

        // lightingShader.setVec3("light.position", camera.Position);
        // lightingShader.setVec3("light.direction", camera.Front);
        shader.setVec3("light.direction", camera.Position);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
        // lightingShader.setVec3("viewPos", camera.Position);

        // light properties
        // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
        // each environment and lighting type requires some tweaking to get the best out of your environment.
        shader.setVec3("light.ambient", amb, amb, amb);
        shader.setVec3("light.diffuse", 3.5f, 3.5f, 3.5f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", linearAtten[attenIndex]);
        shader.setFloat("light.quadratic", quadAtten[attenIndex]);

        // material properties
        shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);

        // view/projection transformations
        if(orthographic)
        {
            projection = glm::ortho(-10.0f, 10.0f, -2.0f, 10.0f, -100.0f, 200.0f);
        }
        else
        {
            projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        }

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // world transformation
        glm::mat4 model;
        shader.setMat4("model", model);

        // DRAW OBJECTS ---------------------------------------------------------
        grassDraw(VAO, shader, grassDiff, mildSpec);
        bballCourtDraw(VAO, shader, bballCourtDiff, noSpec);
        treeDraw(3.0f, 1.0f, 0.0f, VAO, shader, treeTopDiff, mildSpec, treeTrunkDiff, noSpec);
        bballRingDraw(false, 0.0f, 0.75f, -5.5f, VAO, shader, bballPoleDiff, bballBoardFrontDiff, bballBoardBackDiff, bballBoardEdgeDiff, bballRingDiff, highSpec, mildSpec);
        bballRingDraw(true, 0.0f, 0.75f, 5.5f, VAO, shader, bballPoleDiff, bballBoardFrontDiff, bballBoardBackDiff, bballBoardEdgeDiff, bballRingDiff, highSpec, mildSpec);
        manDraw(6.0f, 0.0f, 5.0f, VAO, shader, manShoeDiff, manLegsDiff, manTopBackDiff, manTopDiff, manArmDiff, manNeckDiff, manFaceDiff, manHeadTopDiff, manHeadBackDiff, manHeadLeftDiff, manHeadRightDiff, noSpec);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    // [ESC] - Quit 
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed;

    // [LShift] - Hold to increase camera speed
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        cameraSpeed =  1.0f * deltaTime;
    else
        cameraSpeed = 1.0f * deltaTime * 2;

    // [W] - Move forwards
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, cameraSpeed);
    
    // [S] - Move backwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, cameraSpeed);
    
    // [A] - Straif left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cameraSpeed);
    
    // [D] - Straif right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cameraSpeed);
    
    // [SPACEBAR] - Move vertically up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.Position.y = camera.Position.y + 2.0f * cameraSpeed;
	
    // [X] - Move vertically down
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera.Position.y = camera.Position.y - 2.0f * cameraSpeed;

    // [F] - Toggle light to follow/stay
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && lightTimer == 0)
    {
        lightTimer = 20;

        if(!lightStay)
        {
            lastPosition = camera.Position;
            lightStay = true;
        }
        else
        {
            lightStay = false;
        }
    }

    // [K] - Reduce light brightness 
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        if(amb > -1.0) // lower limit
            amb -= 0.2f;

    // [L] - Increase light brightness
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        if(amb < 5.0) // upper limit
            amb += 0.2f;

    // [O] - Toggle brightness on/off
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && brightnessTimer == 0)
    {
        brightnessTimer = 20;
        
        if(brightToggle)
        {
            attenIndex = 1;
            brightToggle = false;
        }
        else
        {
            attenIndex = 0;
            brightToggle = true;
        }
    }

    // [P] - Toggle between Orthographic/Perspective projection
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && projectionTimer == 0)
    {
        projectionTimer = 20;

        if(orthographic)
        {
            orthographic = false;
        }
        else
        {
            orthographic = true;
        }
    }
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void update_delay()
{
    if(brightnessTimer > 0)
    {
        brightnessTimer -= 1;
    }

    if(lightTimer > 0)
    {
        lightTimer -= 1;
    }

    if(projectionTimer > 0)
    {
         projectionTimer -= 1;
    }
}

void grassDraw(unsigned int VAO, Shader shader, unsigned int grassDiff, unsigned int mildSpec)
{
        glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grassDiff);
		glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mildSpec);

		glm::mat4 grassObj = glm::mat4();
		grassObj = glm::translate(grassObj, glm::vec3(0.0f, -0.01f, 0.0f));
		grassObj = glm::scale(grassObj, glm::vec3(50.0f, 0.0f, 50.0f));

		shader.setMat4("model", grassObj);

		glDrawArrays(GL_TRIANGLES, 0, 36);
}

void bballCourtDraw(unsigned int VAO, Shader shader, unsigned int courtDiff, unsigned int noSpec)
{
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, courtDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    glm::mat4 courtObj = glm::mat4();
    courtObj = glm::scale(courtObj, glm::vec3(5.0f, 0.0f, 10.0f));

    shader.setMat4("model", courtObj);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void treeDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int treeTopDiff, unsigned int mildSpec, unsigned int treeTrunkDiff, unsigned int noSpec)
{
    glBindVertexArray(VAO);

    // Tree trunk
    glm::mat4 trunkObj = glm::mat4();;
    trunkObj = glm::translate(trunkObj, glm::vec3(x, y, z));
    trunkObj = glm::scale(trunkObj, glm::vec3(0.20f, 2.0f, 0.20f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treeTrunkDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", trunkObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);
    
    // Tree top
    glm::vec3 treeTop_scales[] = {
        glm::vec3( 1.0f, 0.4f, 1.0f ),
        glm::vec3( 0.6f, 0.2f, 0.6f ),
        glm::vec3( 0.3f, 0.1f, 0.3f ),
    };
    glm::vec3 treeTop_positions[] = {
        glm::vec3( x - 1.0f, y + 1.0f, z ),
        glm::vec3( x - 0.6f, y + 1.4f, z ),
        glm::vec3( x - 0.3f, y + 1.6f, z )
    };

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, treeTopDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    for(int i = 0; i < 3; i++)
    {	
        glm::mat4 treeTopObj = glm::mat4();
        treeTopObj = glm::translate(treeTopObj, treeTop_positions[i]);
        treeTopObj = glm::scale(treeTopObj, treeTop_scales[i]);
        treeTopObj = glm::translate(treeTopObj, glm::vec3(1.0f, 0.5f, 0.0f));

        shader.setMat4("model", treeTopObj);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void bballRingDraw(bool isSecond, float x, float y, float z, unsigned int VAO, Shader shader, unsigned int bballPoleDiff, unsigned int bballBoardFrontDiff, unsigned int bballBoardBackDiff, unsigned int bballBoardEdgeDiff, unsigned int bballRingDiff, unsigned int highSpec, unsigned int mildSpec)
{
    glBindVertexArray(VAO);
    
    // Base Pole ----------------------------------------------------------------
    glm::mat4 basePoleObj = glm::mat4();
    basePoleObj = glm::translate(basePoleObj, glm::vec3(x, y, z));
    basePoleObj = glm::scale(basePoleObj, glm::vec3(0.1f, 1.5f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballPoleDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", basePoleObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Horizontal Pole ----------------------------------------------------------
    glm::mat4 horizonPoleObj = glm::mat4();

    // Check which basketball ring to draw 
    if(isSecond)
    {
        horizonPoleObj = glm::translate(horizonPoleObj, glm::vec3(x, y + 0.8f, z - 0.2f));
    }
    else
    {
        horizonPoleObj = glm::translate(horizonPoleObj, glm::vec3(x, y + 0.8f, z + 0.2f));
    }
    horizonPoleObj = glm::scale(horizonPoleObj, glm::vec3(0.1f, 0.1f, 0.5f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballPoleDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", horizonPoleObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Front of backboard -------------------------------------------------------
    glm::mat4 frontBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        frontBoardObj = glm::translate(frontBoardObj, glm::vec3(x, y + 1.0f, z - 0.5f));
        frontBoardObj = glm::scale(frontBoardObj, glm::vec3(1.0f, 1.0f, 0.05f));
    }
    else
    {
        frontBoardObj = glm::translate(frontBoardObj, glm::vec3(x, y + 1.0f, z + 0.5f));
        frontBoardObj = glm::scale(frontBoardObj, glm::vec3(1.0f, 1.0f, 0.05f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardFrontDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", frontBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Back of backboard --------------------------------------------------------
    glm::mat4 backBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        backBoardObj = glm::translate(backBoardObj, glm::vec3(x, y + 1.0f, z - 0.45f));
        backBoardObj = glm::scale(backBoardObj, glm::vec3(1.0f, 1.0f, 0.05f));
    }
    else
    {
        backBoardObj = glm::translate(backBoardObj, glm::vec3(x, y + 1.0f, z + 0.45f));
        backBoardObj = glm::scale(backBoardObj, glm::vec3(1.0f, 1.0f, 0.05f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardBackDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", backBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Top edge of backboard ----------------------------------------------------
    glm::mat4 topEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        topEdgeBoardObj = glm::translate(topEdgeBoardObj, glm::vec3(x, y + 1.5f, z - 0.475f));
        topEdgeBoardObj = glm::scale(topEdgeBoardObj, glm::vec3(1.0f, 0.01f, 0.1f));
    }
    else
    {
        topEdgeBoardObj = glm::translate(topEdgeBoardObj, glm::vec3(x, y + 1.5f, z + 0.475f));
        topEdgeBoardObj = glm::scale(topEdgeBoardObj, glm::vec3(1.0f, 0.01f, 0.1f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardEdgeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", topEdgeBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Bottom edge of backboard -------------------------------------------------
    glm::mat4 botEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        botEdgeBoardObj = glm::translate(botEdgeBoardObj, glm::vec3(x, y + 0.5f, z - 0.475f));
        botEdgeBoardObj = glm::scale(botEdgeBoardObj, glm::vec3(1.0f, 0.01f, 0.1f));
    }
    else
    {
        botEdgeBoardObj = glm::translate(botEdgeBoardObj, glm::vec3(x, y + 0.5f, z + 0.475f));
        botEdgeBoardObj = glm::scale(botEdgeBoardObj, glm::vec3(1.0f, 0.01f, 0.1f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardEdgeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", botEdgeBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Left edge of backboard ---------------------------------------------------
    glm::mat4 leftEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        leftEdgeBoardObj = glm::translate(leftEdgeBoardObj, glm::vec3(x + 0.5f, y + 1.0f, z - 0.475f));
        leftEdgeBoardObj = glm::scale(leftEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }
    else
    {
        leftEdgeBoardObj = glm::translate(leftEdgeBoardObj, glm::vec3(x - 0.5f, y + 1.0f, z + 0.475f));
        leftEdgeBoardObj = glm::scale(leftEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardEdgeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", leftEdgeBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right edge of backboard --------------------------------------------------
    glm::mat4 rightEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        rightEdgeBoardObj = glm::translate(rightEdgeBoardObj, glm::vec3(x - 0.5f, y + 1.0f, z - 0.475f));
        rightEdgeBoardObj = glm::scale(rightEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }
    else
    {
        rightEdgeBoardObj = glm::translate(rightEdgeBoardObj, glm::vec3(x + 0.5f, y + 1.0f, z + 0.475f));
        rightEdgeBoardObj = glm::scale(rightEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballBoardEdgeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mildSpec);

    shader.setMat4("model", rightEdgeBoardObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Ring base ----------------------------------------------------------------
    glm::mat4 ringBaseObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringBaseObj = glm::translate(ringBaseObj, glm::vec3(x, y + 0.675f, z - 0.575f));
        ringBaseObj = glm::scale(ringBaseObj, glm::vec3(0.05f, 0.05f, 0.1f));
    }
    else
    {
        ringBaseObj = glm::translate(ringBaseObj, glm::vec3(x, y + 0.675f, z + 0.575f));
        ringBaseObj = glm::scale(ringBaseObj, glm::vec3(0.05f, 0.05f, 0.1f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballRingDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", ringBaseObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Ring back ----------------------------------------------------------------
    glm::mat4 ringBackObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringBackObj = glm::translate(ringBackObj, glm::vec3(x, y + 0.675f, z - 0.625f));
        ringBackObj = glm::scale(ringBackObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }
    else
    {
        ringBackObj = glm::translate(ringBackObj, glm::vec3(x, y + 0.675f, z + 0.625f));
        ringBackObj = glm::scale(ringBackObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballRingDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", ringBackObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Ring front ----------------------------------------------------------------
    glm::mat4 ringFrontObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringFrontObj = glm::translate(ringFrontObj, glm::vec3(x, y + 0.675f, z - 0.85f));
        ringFrontObj = glm::scale(ringFrontObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }
    else
    {
        ringFrontObj = glm::translate(ringFrontObj, glm::vec3(x, y + 0.675f, z + 0.85f));
        ringFrontObj = glm::scale(ringFrontObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballRingDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", ringFrontObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Ring left
    glm::mat4 ringLeftObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringLeftObj = glm::translate(ringLeftObj, glm::vec3(x - 0.137f, y + 0.675f, z - 0.737f));
        ringLeftObj = glm::scale(ringLeftObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }
    else
    {
        ringLeftObj = glm::translate(ringLeftObj, glm::vec3(x - 0.137f, y + 0.675f, z + 0.737f));
        ringLeftObj = glm::scale(ringLeftObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballRingDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", ringLeftObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Ring right
    glm::mat4 ringRightObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringRightObj = glm::translate(ringRightObj, glm::vec3(x + 0.137f, y + 0.675f, z - 0.737f));
        ringRightObj = glm::scale(ringRightObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }
    else
    {
        ringRightObj = glm::translate(ringRightObj, glm::vec3(x + 0.137f, y + 0.675f, z + 0.737f));
        ringRightObj = glm::scale(ringRightObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bballRingDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, highSpec);

    shader.setMat4("model", ringRightObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);
}

void manDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int manShoeDiff, unsigned int manLegsDiff, unsigned int manTopBackDiff, unsigned int manTopDiff, unsigned int manArmDiff, unsigned int manNeckDiff, unsigned int manFaceDiff, unsigned int manHeadTopDiff, unsigned int manHeadBackDiff, unsigned int manHeadLeftDiff, unsigned int manHeadRightDiff,unsigned int noSpec)
{
    glBindVertexArray(VAO);

    // SHOES --------------------------------------------------------------------
    // Left shoe
    glm::mat4 leftShoeObj = glm::mat4();

    leftShoeObj = glm::translate(leftShoeObj, glm::vec3(x, y + 0.045f, z));
    leftShoeObj = glm::scale(leftShoeObj, glm::vec3(0.15f, 0.10f, 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manShoeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", leftShoeObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right shoe
    glm::mat4 rightShoeObj = glm::mat4();

    rightShoeObj = glm::translate(rightShoeObj, glm::vec3(x + 0.25f, y + 0.045f, z));
    rightShoeObj = glm::scale(rightShoeObj, glm::vec3(0.15f, 0.10f, 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manShoeDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", rightShoeObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // // LEGS ---------------------------------------------------------------------
    // Left leg
    glm::mat4 leftLegObj = glm::mat4();

    leftLegObj = glm::translate(leftLegObj, glm::vec3(x, y + 0.27f, z + 0.05f));
    leftLegObj = glm::scale(leftLegObj, glm::vec3(0.15f, 0.35f, 0.15f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manLegsDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", leftLegObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right leg
    glm::mat4 rightLegObj = glm::mat4();

    rightLegObj = glm::translate(rightLegObj, glm::vec3(x + 0.25f, y + 0.27f, z + 0.05f));
    rightLegObj = glm::scale(rightLegObj, glm::vec3(0.15f, 0.35f, 0.15f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manLegsDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", rightLegObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // // TORSO --------------------------------------------------------------------
    glm::mat4 torsoObj = glm::mat4();

    torsoObj = glm::translate(torsoObj, glm::vec3(x + 0.125f, y + 0.67f, z + 0.05f));
    torsoObj = glm::scale(torsoObj, glm::vec3(0.4f, 0.45f, 0.15f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manTopDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", torsoObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // ARMS
    // Left arm sleave
    glm::mat4 leftArmObj = glm::mat4();

    leftArmObj = glm::translate(leftArmObj, glm::vec3(x - 0.13f, y + 0.75f, z + 0.05f));
    leftArmObj = glm::scale(leftArmObj, glm::vec3(0.1f, 0.25f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manTopDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", leftArmObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Left hand
    glm::mat4 leftHandObj = glm::mat4();

    leftHandObj = glm::translate(leftHandObj, glm::vec3(x - 0.13f, y + 0.58f, z + 0.05f));
    leftHandObj = glm::scale(leftHandObj, glm::vec3(0.1f, 0.1f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manNeckDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", leftHandObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right arm sleave
    glm::mat4 rightArmObj = glm::mat4();

    rightArmObj = glm::translate(rightArmObj, glm::vec3(x + 0.38f, y + 0.75f, z + 0.05f));
    rightArmObj = glm::scale(rightArmObj, glm::vec3(0.1f, 0.25f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manTopDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", rightArmObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right hand
    glm::mat4 rightHandObj = glm::mat4();

    rightHandObj = glm::translate(rightHandObj, glm::vec3(x + 0.38f, y + 0.58f, z + 0.05f));
    rightHandObj = glm::scale(rightHandObj, glm::vec3(0.1f, 0.1f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manNeckDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", rightHandObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // NECK
    glm::mat4 neckObj = glm::mat4();

    neckObj = glm::translate(neckObj, glm::vec3(x + 0.125f, y + 0.92f, z + 0.05f));
    neckObj = glm::scale(neckObj, glm::vec3(0.1f, 0.05f, 0.1f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manNeckDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", neckObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // HEAD
    // Face
    glm::mat4 headObj = glm::mat4();

    headObj = glm::translate(headObj, glm::vec3(x + 0.125f, y + 1.07f, z + 0.05f));
    headObj = glm::rotate(headObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    headObj = glm::rotate(headObj, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
    headObj = glm::scale(headObj, glm::vec3(0.25f, 0.25f , 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manFaceDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", headObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Chin
    glm::mat4 chinObj = glm::mat4();

    chinObj = glm::translate(chinObj, glm::vec3(x + 0.125f, y + 0.945f, z + 0.05f));
    chinObj = glm::scale(chinObj, glm::vec3(0.24f, 0.01f, 0.24f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manNeckDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", chinObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Hair
    glm::mat4 hairObj = glm::mat4();

    hairObj = glm::translate(hairObj, glm::vec3(x + 0.125f, y + 1.2f, z + 0.05f));
    hairObj = glm::scale(hairObj, glm::vec3(0.25f, 0.01f, 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manHeadTopDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", hairObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Back of head
    glm::mat4 backHeadObj = glm::mat4();

    backHeadObj = glm::translate(backHeadObj, glm::vec3(x + 0.125f, y + 1.07f, z + 0.18f));
    backHeadObj = glm::rotate(backHeadObj, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    backHeadObj = glm::scale(backHeadObj, glm::vec3(0.25f, 0.25f, 0.01f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manHeadBackDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", backHeadObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Left of head
    glm::mat4 leftHeadObj = glm::mat4();

    leftHeadObj = glm::translate(leftHeadObj, glm::vec3(x, y + 1.07f, z + 0.05f));
    leftHeadObj = glm::rotate(leftHeadObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    leftHeadObj = glm::rotate(leftHeadObj, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    leftHeadObj = glm::scale(leftHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manHeadLeftDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", leftHeadObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);

    // Right of head
    glm::mat4 rightHeadObj = glm::mat4();

    rightHeadObj = glm::translate(rightHeadObj, glm::vec3(x + 0.25f, y + 1.07f, z + 0.05f));
    rightHeadObj = glm::rotate(rightHeadObj, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    rightHeadObj = glm::scale(rightHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manHeadRightDiff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noSpec);

    shader.setMat4("model", rightHeadObj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);
}