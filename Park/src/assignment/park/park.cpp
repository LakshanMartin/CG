#include "park.h"

// SETTING
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
const float LOWER_BOUNDS = 0.15f;
const float UPPER_BOUNDS = 10.0f;

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
int incBrightTimer = 0;
int decBrightTimer = 0;
int dayNightTimer = 0;
int followStayTimer = 0;
int projectionTimer = 0;
int animationTimer = 0;

// LIGHT
float amb = 1.0f;
float linearAtten[2] = {0.0014f, 0.045f};
float quadAtten[2] = {0.000007f, 0.0075f};
int attenIndex = 1;

// TOGGLES
bool brightToggle = false;
bool lightStay = false;
bool orthographic = false;

// ANIMATION TRIGGER
bool playAnimation = true;
float ballDistance;
float dogBodyDistance; 
float birdDistance; 

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
    Shader skyShader("5.4.lamp.vs", "5.4.lamp.fs");

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
    unsigned int bballDiff = loadTexture(FileSystem::getPath("resources/textures/bball.png").c_str());
    unsigned int manShoeDiff = loadTexture(FileSystem::getPath("resources/textures/shoes.png").c_str());
    unsigned int manLegsDiff = loadTexture(FileSystem::getPath("resources/textures/pants.png").c_str());
    unsigned int manTopBackDiff = loadTexture(FileSystem::getPath("resources/textures/man_top_back.png").c_str());
    unsigned int manTopDiff = loadTexture(FileSystem::getPath("resources/textures/man_top.png").c_str());
    unsigned int manArmDiff = loadTexture(FileSystem::getPath("resources/textures/man_arm.png").c_str());
    unsigned int manNeckDiff = loadTexture(FileSystem::getPath("resources/textures/man_neck.png").c_str());
    unsigned int manFaceDiff = loadTexture(FileSystem::getPath("resources/textures/man_face.png").c_str());
    unsigned int manFace2Diff = loadTexture(FileSystem::getPath("resources/textures/man_face2.png").c_str());
    unsigned int manHeadTopDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_top.png").c_str());
    unsigned int manHeadBackDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_back.png").c_str());
    unsigned int manHeadLeftDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_left.png").c_str());
    unsigned int manHeadRightDiff = loadTexture(FileSystem::getPath("resources/textures/man_head_right.png").c_str());
    unsigned int dogHeadDiff = loadTexture(FileSystem::getPath("resources/textures/dog_head.png").c_str());
    unsigned int dogBodyDiff = loadTexture(FileSystem::getPath("resources/textures/dog_fur.png").c_str());
    unsigned int birdDiff = loadTexture(FileSystem::getPath("resources/textures/bird.png").c_str());
    unsigned int skyDiff = loadTexture(FileSystem::getPath("resources/textures/sky.jpg").c_str());
    unsigned int playFloorDiff = loadTexture(FileSystem::getPath("resources/textures/play_floor.png").c_str());
    unsigned int swingFrameDiff = loadTexture(FileSystem::getPath("resources/textures/log.png").c_str());
    unsigned int swingRopeDiff = loadTexture(FileSystem::getPath("resources/textures/rope.png").c_str());
    unsigned int swingSeatDiff = loadTexture(FileSystem::getPath("resources/textures/swing_seat.png").c_str());

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

        shader.setVec3("light.direction", camera.Position);
        shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));

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
            projection = glm::ortho(-10.0f, 10.0f, -2.0f, 10.0f, -10.0f, 200.0f);
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

        // DRAW SKY BOX
        skyDraw(VAO, shader, skyDiff, noSpec);

        // DRAW OBJECTS ---------------------------------------------------------
        grassDraw(VAO, shader, grassDiff, mildSpec);
        bballCourtDraw(VAO, shader, bballCourtDiff, noSpec);
        bballRingDraw(false, 0.0f, 1.0f, -5.5f, VAO, shader, bballPoleDiff, bballBoardFrontDiff, bballBoardBackDiff, bballBoardEdgeDiff, bballRingDiff, highSpec, mildSpec);
        bballRingDraw(true, 0.0f, 1.0f, 5.5f, VAO, shader, bballPoleDiff, bballBoardFrontDiff, bballBoardBackDiff, bballBoardEdgeDiff, bballRingDiff, highSpec, mildSpec);
        manDraw(-0.12f, 0.0f, -1.5f, VAO, shader, manShoeDiff, manLegsDiff, manTopBackDiff, manTopDiff, manArmDiff, manNeckDiff, manFaceDiff, manFace2Diff, manHeadTopDiff, manHeadBackDiff, manHeadLeftDiff, manHeadRightDiff, noSpec);
        bballDraw(0.0f, 0.3f, -1.5f, VAO, shader, bballDiff, mildSpec);
        dogDraw(3.0f, 0.2f, -3.0f, VAO, shader, dogHeadDiff, dogBodyDiff, noSpec);
        birdDraw(2.9f, 1.0f, -3.0f, VAO, shader, birdDiff, noSpec);
        playFloorDraw(VAO, shader, playFloorDiff, noSpec);
        swingDraw(VAO, shader, swingFrameDiff, swingRopeDiff, swingSeatDiff, noSpec, mildSpec);

        // DRAW TREE BARRIERS
        for(int i = -12; i <= 12; i++)
        {
            treeDraw(i, 2.5f, 12.0f, VAO, shader, treeTopDiff, mildSpec, treeTrunkDiff, noSpec);
            treeDraw(-12.0f, 2.5f, i, VAO, shader, treeTopDiff, mildSpec, treeTrunkDiff, noSpec);
            treeDraw(i, 2.5f, -12.0f, VAO, shader, treeTopDiff, mildSpec, treeTrunkDiff, noSpec);
            treeDraw(12.0f, 2.5f, i, VAO, shader, treeTopDiff, mildSpec, treeTrunkDiff, noSpec);
        }

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && camera.Position.y > LOWER_BOUNDS && camera.Position.y < UPPER_BOUNDS)
        camera.ProcessKeyboard(FORWARD, cameraSpeed);
    
    // [S] - Move backwards
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && camera.Position.y > LOWER_BOUNDS && camera.Position.y < UPPER_BOUNDS)
        camera.ProcessKeyboard(BACKWARD, cameraSpeed);
    
    // [A] - Straif left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, cameraSpeed);
    
    // [D] - Straif right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, cameraSpeed);
    
    // [SPACEBAR] - Move vertically up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && camera.Position.y < UPPER_BOUNDS)
		camera.Position.y = camera.Position.y + 2.0f * cameraSpeed;
	
    // [X] - Move vertically down
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && camera.Position.y > LOWER_BOUNDS)
		camera.Position.y = camera.Position.y - 2.0f * cameraSpeed;

    // [F] - Toggle light to follow/stay
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && followStayTimer == 0)
    {
        followStayTimer = 20;

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
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS && decBrightTimer == 0)
    {
        decBrightTimer = 20;
    
        if(amb > -1.0) // lower limit
        {
            amb -= 0.25f;
        }
    }

    // [L] - Increase light brightness
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && incBrightTimer == 0)
    {
        incBrightTimer = 20;

        if(amb < 5.0) // upper limit
        {
            amb += 0.25f;
        }
    }

    // [O] - Toggle brightness on/off
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && dayNightTimer == 0)
    {
        dayNightTimer = 20;
        
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

    // [R] - Play/Reset Animations
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && animationTimer == 0)
    {
        animationTimer = 20;

        if(playAnimation)
        {
            playAnimation = false;
        }
        else
        {
            playAnimation = true;
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

void applyTexture(Shader shader, glm::mat4 obj, unsigned int diff, unsigned int spec)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diff);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, spec);

    shader.setMat4("model", obj);
    glDrawArrays(GL_TRIANGLES, 0 , 36);
}

void update_delay()
{
    if(incBrightTimer > 0)
    {
        incBrightTimer -= 1;
    }

    if(decBrightTimer > 0)
    {
        decBrightTimer -= 1;
    }

    if(dayNightTimer > 0)
    {
        dayNightTimer -= 1;
    }

    if(followStayTimer > 0)
    {
        followStayTimer -= 1;
    }

    if(projectionTimer > 0)
    {
         projectionTimer -= 1;
    }
    
    if(animationTimer > 0)
    {
         animationTimer -= 1;
    }
}

void skyDraw(unsigned int VAO, Shader shader, unsigned int skyDiff, unsigned int noSpec)
{
    glBindVertexArray(VAO);

    glm::mat4 skyObj = glm::mat4();
    
    skyObj = glm::scale(skyObj, glm::vec3(50.0f, 50.0f, 50.0f));

    applyTexture(shader, skyObj, skyDiff, noSpec);
}

void grassDraw(unsigned int VAO, Shader shader, unsigned int grassDiff, unsigned int mildSpec)
{
        glBindVertexArray(VAO);

		glm::mat4 grassObj = glm::mat4();

		grassObj = glm::translate(grassObj, glm::vec3(0.0f, -0.51f, 0.0f));
		grassObj = glm::scale(grassObj, glm::vec3(25.0f, 1.0f, 25.0f));

        applyTexture(shader, grassObj, grassDiff, mildSpec);
}

void bballCourtDraw(unsigned int VAO, Shader shader, unsigned int courtDiff, unsigned int noSpec)
{
    glBindVertexArray(VAO);

    glm::mat4 courtObj = glm::mat4();
    courtObj = glm::scale(courtObj, glm::vec3(5.0f, 0.0f, 10.0f));

    applyTexture(shader, courtObj, courtDiff, noSpec);
}

void treeDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int treeTopDiff, unsigned int mildSpec, unsigned int treeTrunkDiff, unsigned int noSpec)
{
    glBindVertexArray(VAO);

    // Tree trunk
    glm::mat4 trunkObj = glm::mat4();;

    trunkObj = glm::translate(trunkObj, glm::vec3(x, y, z));
    trunkObj = glm::scale(trunkObj, glm::vec3(0.3f, 5.0f, 0.3f));

    applyTexture(shader, trunkObj, treeTrunkDiff, noSpec);
    
    // Tree top
    glm::vec3 treeTop_scales[] = {
        glm::vec3( 1.0f, 0.4f, 1.0f ),
        glm::vec3( 0.6f, 0.2f, 0.6f ),
        glm::vec3( 0.3f, 0.1f, 0.3f ),
    };
    glm::vec3 treeTop_positions[] = {
        glm::vec3( x - 1.0f, y + 2.5f, z ),
        glm::vec3( x - 0.6f, y + 2.9f, z ),
        glm::vec3( x - 0.3f, y + 3.1f, z )
    };

    for(int i = 0; i < 3; i++)
    {	
        glm::mat4 treeTopObj = glm::mat4();
        
        treeTopObj = glm::translate(treeTopObj, treeTop_positions[i]);
        treeTopObj = glm::scale(treeTopObj, treeTop_scales[i]);
        treeTopObj = glm::translate(treeTopObj, glm::vec3(1.0f, 0.5f, 0.0f));
    
        applyTexture(shader, treeTopObj, treeTopDiff, mildSpec);
    }
}

void bballRingDraw(bool isSecond, float x, float y, float z, unsigned int VAO, Shader shader, unsigned int bballPoleDiff, unsigned int bballBoardFrontDiff, unsigned int bballBoardBackDiff, unsigned int bballBoardEdgeDiff, unsigned int bballRingDiff, unsigned int highSpec, unsigned int mildSpec)
{
    glBindVertexArray(VAO);
    
    // Base Pole ----------------------------------------------------------------
    glm::mat4 basePoleObj = glm::mat4();
    basePoleObj = glm::translate(basePoleObj, glm::vec3(x, y, z));
    basePoleObj = glm::scale(basePoleObj, glm::vec3(0.1f, 2.0f, 0.1f));

    // Horizontal Pole ----------------------------------------------------------
    glm::mat4 horizonPoleObj = glm::mat4();

    // Check which basketball ring to draw 
    if(isSecond)
    {
        horizonPoleObj = glm::translate(horizonPoleObj, glm::vec3(x, y + 1.05f, z - 0.2f));
    }
    else
    {
        horizonPoleObj = glm::translate(horizonPoleObj, glm::vec3(x, y + 1.05f, z + 0.2f));
    }
    horizonPoleObj = glm::scale(horizonPoleObj, glm::vec3(0.1f, 0.1f, 0.5f));


    // Front of backboard -------------------------------------------------------
    glm::mat4 frontBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        frontBoardObj = glm::translate(frontBoardObj, glm::vec3(x, y + 1.2f, z - 0.5f));
        frontBoardObj = glm::scale(frontBoardObj, glm::vec3(1.5f, 1.0f, 0.05f));
    }
    else
    {
        frontBoardObj = glm::translate(frontBoardObj, glm::vec3(x, y + 1.2f, z + 0.5f));
        frontBoardObj = glm::scale(frontBoardObj, glm::vec3(1.5f, 1.0f, 0.05f));
    }

    // Back of backboard --------------------------------------------------------
    glm::mat4 backBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        backBoardObj = glm::translate(backBoardObj, glm::vec3(x, y + 1.2f, z - 0.45f));
        backBoardObj = glm::scale(backBoardObj, glm::vec3(1.5f, 1.0f, 0.05f));
    }
    else
    {
        backBoardObj = glm::translate(backBoardObj, glm::vec3(x, y + 1.2f, z + 0.45f));
        backBoardObj = glm::scale(backBoardObj, glm::vec3(1.5f, 1.0f, 0.05f));
    }

    // Top edge of backboard ----------------------------------------------------
    glm::mat4 topEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        topEdgeBoardObj = glm::translate(topEdgeBoardObj, glm::vec3(x, y + 1.7f, z - 0.475f));
        topEdgeBoardObj = glm::scale(topEdgeBoardObj, glm::vec3(1.5f, 0.01f, 0.1f));
    }
    else
    {
        topEdgeBoardObj = glm::translate(topEdgeBoardObj, glm::vec3(x, y + 1.7f, z + 0.475f));
        topEdgeBoardObj = glm::scale(topEdgeBoardObj, glm::vec3(1.5f, 0.01f, 0.1f));
    }

    // Bottom edge of backboard -------------------------------------------------
    glm::mat4 botEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        botEdgeBoardObj = glm::translate(botEdgeBoardObj, glm::vec3(x, y + 0.7f, z - 0.475f));
        botEdgeBoardObj = glm::scale(botEdgeBoardObj, glm::vec3(1.5f, 0.01f, 0.1f));
    }
    else
    {
        botEdgeBoardObj = glm::translate(botEdgeBoardObj, glm::vec3(x, y + 0.7f, z + 0.475f));
        botEdgeBoardObj = glm::scale(botEdgeBoardObj, glm::vec3(1.5f, 0.01f, 0.1f));
    }

    // Left edge of backboard ---------------------------------------------------
    glm::mat4 leftEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        leftEdgeBoardObj = glm::translate(leftEdgeBoardObj, glm::vec3(x + 0.75f, y + 1.2f, z - 0.475f));
        leftEdgeBoardObj = glm::scale(leftEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }
    else
    {
        leftEdgeBoardObj = glm::translate(leftEdgeBoardObj, glm::vec3(x - 0.75f, y + 1.2f, z + 0.475f));
        leftEdgeBoardObj = glm::scale(leftEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }

    // Right edge of backboard --------------------------------------------------
    glm::mat4 rightEdgeBoardObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        rightEdgeBoardObj = glm::translate(rightEdgeBoardObj, glm::vec3(x - 0.75f, y + 1.2f, z - 0.475f));
        rightEdgeBoardObj = glm::scale(rightEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }
    else
    {
        rightEdgeBoardObj = glm::translate(rightEdgeBoardObj, glm::vec3(x + 0.75f, y + 1.2f, z + 0.475f));
        rightEdgeBoardObj = glm::scale(rightEdgeBoardObj, glm::vec3(0.01f, 1.0f, 0.1f));
    }

    // Ring base ----------------------------------------------------------------
    glm::mat4 ringBaseObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringBaseObj = glm::translate(ringBaseObj, glm::vec3(x, y + 0.85f, z - 0.575f));
        ringBaseObj = glm::scale(ringBaseObj, glm::vec3(0.05f, 0.05f, 0.1f));
    }
    else
    {
        ringBaseObj = glm::translate(ringBaseObj, glm::vec3(x, y + 0.85f, z + 0.575f));
        ringBaseObj = glm::scale(ringBaseObj, glm::vec3(0.05f, 0.05f, 0.1f));
    }

    // Ring back ----------------------------------------------------------------
    glm::mat4 ringBackObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringBackObj = glm::translate(ringBackObj, glm::vec3(x, y + 0.85f, z - 0.625f));
        ringBackObj = glm::scale(ringBackObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }
    else
    {
        ringBackObj = glm::translate(ringBackObj, glm::vec3(x, y + 0.85f, z + 0.625f));
        ringBackObj = glm::scale(ringBackObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }

    // Ring front ----------------------------------------------------------------
    glm::mat4 ringFrontObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringFrontObj = glm::translate(ringFrontObj, glm::vec3(x, y + 0.85f, z - 0.85f));
        ringFrontObj = glm::scale(ringFrontObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }
    else
    {
        ringFrontObj = glm::translate(ringFrontObj, glm::vec3(x, y + 0.85f, z + 0.85f));
        ringFrontObj = glm::scale(ringFrontObj, glm::vec3(0.25f, 0.05f, 0.025f));
    }

    // Ring left
    glm::mat4 ringLeftObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringLeftObj = glm::translate(ringLeftObj, glm::vec3(x - 0.137f, y + 0.85f, z - 0.737f));
        ringLeftObj = glm::scale(ringLeftObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }
    else
    {
        ringLeftObj = glm::translate(ringLeftObj, glm::vec3(x - 0.137f, y + 0.85f, z + 0.737f));
        ringLeftObj = glm::scale(ringLeftObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }

    // Ring right
    glm::mat4 ringRightObj = glm::mat4();

    // Check which basketball ring to draw
    if(isSecond)
    {
        ringRightObj = glm::translate(ringRightObj, glm::vec3(x + 0.137f, y + 0.85f, z - 0.737f));
        ringRightObj = glm::scale(ringRightObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }
    else
    {
        ringRightObj = glm::translate(ringRightObj, glm::vec3(x + 0.137f, y + 0.85f, z + 0.737f));
        ringRightObj = glm::scale(ringRightObj, glm::vec3(0.025f, 0.05f, 0.25f));
    }

    applyTexture(shader, basePoleObj, bballPoleDiff, highSpec);
    applyTexture(shader, horizonPoleObj, bballPoleDiff, highSpec);
    applyTexture(shader, frontBoardObj, bballBoardFrontDiff, mildSpec);
    applyTexture(shader, backBoardObj, bballBoardBackDiff, mildSpec);
    applyTexture(shader, topEdgeBoardObj, bballBoardEdgeDiff, mildSpec);
    applyTexture(shader, botEdgeBoardObj, bballBoardEdgeDiff, mildSpec);
    applyTexture(shader, leftEdgeBoardObj, bballBoardEdgeDiff, mildSpec);
    applyTexture(shader, rightEdgeBoardObj, bballBoardEdgeDiff, mildSpec);
    applyTexture(shader, ringBaseObj, bballRingDiff, highSpec);
    applyTexture(shader, ringBackObj, bballRingDiff, highSpec);
    applyTexture(shader, ringFrontObj, bballRingDiff, highSpec);
    applyTexture(shader, ringLeftObj, bballRingDiff, highSpec);
    applyTexture(shader, ringRightObj, bballRingDiff, highSpec);
}

void manDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int manShoeDiff, unsigned int manLegsDiff, unsigned int manTopBackDiff, unsigned int manTopDiff, unsigned int manArmDiff, unsigned int manNeckDiff, unsigned int manFaceDiff, unsigned int manFace2Diff, unsigned int manHeadTopDiff, unsigned int manHeadBackDiff, unsigned int manHeadLeftDiff, unsigned int manHeadRightDiff,unsigned int noSpec)
{
    glBindVertexArray(VAO);

    glm::mat4 leftShoeObj = glm::mat4();
    glm::mat4 rightShoeObj = glm::mat4();
    glm::mat4 leftLegObj = glm::mat4();
    glm::mat4 rightLegObj = glm::mat4();
    glm::mat4 torsoObj = glm::mat4();
    glm::mat4 backTorsoObj = glm::mat4();
    glm::mat4 leftArmObj = glm::mat4();
    glm::mat4 leftHandObj = glm::mat4();
    glm::mat4 rightArmObj = glm::mat4();
    glm::mat4 rightHandObj = glm::mat4();
    glm::mat4 neckObj = glm::mat4();
    glm::mat4 headObj = glm::mat4();
    glm::mat4 chinObj = glm::mat4();
    glm::mat4 hairObj = glm::mat4();
    glm::mat4 backHeadObj = glm::mat4();
    glm::mat4 leftHeadObj = glm::mat4();
    glm::mat4 rightHeadObj = glm::mat4();

    // SHOES --------------------------------------------------------------------
    // Left shoe
    leftShoeObj = glm::translate(leftShoeObj, glm::vec3(x, y + 0.045f, z));
    leftShoeObj = glm::scale(leftShoeObj, glm::vec3(0.15f, 0.10f, 0.25f));

    // Right shoe
    rightShoeObj = glm::translate(rightShoeObj, glm::vec3(x + 0.25f, y + 0.045f, z));
    rightShoeObj = glm::scale(rightShoeObj, glm::vec3(0.15f, 0.10f, 0.25f));

    // // LEGS ---------------------------------------------------------------------
    // Left leg
    leftLegObj = glm::translate(leftLegObj, glm::vec3(x, y + 0.27f, z + 0.05f));
    leftLegObj = glm::scale(leftLegObj, glm::vec3(0.15f, 0.35f, 0.15f));

    // Right leg
    rightLegObj = glm::translate(rightLegObj, glm::vec3(x + 0.25f, y + 0.27f, z + 0.05f));
    rightLegObj = glm::scale(rightLegObj, glm::vec3(0.15f, 0.35f, 0.15f));

    // TORSO --------------------------------------------------------------------
    // Base torso
    torsoObj = glm::translate(torsoObj, glm::vec3(x + 0.125f, y + 0.67f, z + 0.05f));
    torsoObj = glm::scale(torsoObj, glm::vec3(0.4f, 0.45f, 0.15f));

    // Back torso
    backTorsoObj = glm::translate(backTorsoObj, glm::vec3(x+ 0.125f, y + 0.67f, z + 0.125f));
    backTorsoObj = glm::rotate(backTorsoObj, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
    backTorsoObj = glm::rotate(backTorsoObj, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
    backTorsoObj = glm::scale(backTorsoObj, glm::vec3(0.4f, 0.45f, 0.01f));

    // ANIMATED PARTS -----------------------------------------------------------
    // For arm animations
    float scaleAmount = sin(glfwGetTime() * 8.0f);

    if(playAnimation)
    {
        // Left arm
        leftArmObj = glm::translate(leftArmObj, glm::vec3(x - 0.13f, y + 0.8f, z + 0.05f));
        leftArmObj = glm::rotate(leftArmObj, glm::radians(-10.0f), glm::vec3(0.0, 1.0, 0.0));
        leftArmObj = glm::rotate(leftArmObj, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));
        leftArmObj = glm::scale(leftArmObj, glm::vec3(0.1f, 0.15f, 0.1f));

        // Left hand
        leftHandObj = glm::rotate(leftHandObj, glm::radians(scaleAmount), glm::vec3(1.0, 0.0, 0.0));
        leftHandObj = glm::translate(leftHandObj, glm::vec3(x - 0.07f, y + 0.70f, z - 0.09f));
        leftHandObj = glm::rotate(leftHandObj, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
        leftHandObj = glm::scale(leftHandObj, glm::vec3(0.1f, 0.1f, 0.35f));

        // Right arm
        rightArmObj = glm::translate(rightArmObj, glm::vec3(x + 0.38f, y + 0.8f, z + 0.05f));
        rightArmObj = glm::rotate(rightArmObj, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
        rightArmObj = glm::rotate(rightArmObj, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));
        rightArmObj = glm::scale(rightArmObj, glm::vec3(0.1f, 0.15f, 0.1f));

        // Right hand
        rightHandObj = glm::rotate(rightHandObj, glm::radians(scaleAmount), glm::vec3(1.0, 0.0, 0.0));
        rightHandObj = glm::translate(rightHandObj, glm::vec3(x + 0.32f, y + 0.70f, z - 0.09f));
        rightHandObj = glm::rotate(rightHandObj, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
        rightHandObj = glm::scale(rightHandObj, glm::vec3(0.1f, 0.1f, 0.35f));

        // Head box
        headObj = glm::translate(headObj, glm::vec3(x + 0.125f, y + 1.07f, z + 0.05f));
        headObj = glm::rotate(headObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        headObj = glm::rotate(headObj, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
        headObj = glm::scale(headObj, glm::vec3(0.25f, 0.25f , 0.25f));
        
        // Happy face applied
        applyTexture(shader, headObj, manFaceDiff, noSpec);

        // Chin
        chinObj = glm::translate(chinObj, glm::vec3(x + 0.125f, y + 0.945f, z + 0.05f));
        chinObj = glm::scale(chinObj, glm::vec3(0.24f, 0.01f, 0.24f));

        // Hair
        hairObj = glm::translate(hairObj, glm::vec3(x + 0.125f, y + 1.2f, z + 0.05f));
        hairObj = glm::scale(hairObj, glm::vec3(0.25f, 0.01f, 0.25f));

        // Back of head
        backHeadObj = glm::translate(backHeadObj, glm::vec3(x + 0.125f, y + 1.07f, z + 0.18f));
        backHeadObj = glm::rotate(backHeadObj, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
        backHeadObj = glm::scale(backHeadObj, glm::vec3(0.25f, 0.25f, 0.01f));

        // Left of head
        leftHeadObj = glm::translate(leftHeadObj, glm::vec3(x, y + 1.07f, z + 0.05f));
        leftHeadObj = glm::rotate(leftHeadObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        leftHeadObj = glm::rotate(leftHeadObj, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        leftHeadObj = glm::scale(leftHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));

        // Right of head
        rightHeadObj = glm::translate(rightHeadObj, glm::vec3(x + 0.25f, y + 1.07f, z + 0.05f));
        rightHeadObj = glm::rotate(rightHeadObj, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        rightHeadObj = glm::scale(rightHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));
    }
    else
    {
        // Left arm
        leftArmObj = glm::translate(leftArmObj, glm::vec3(x - 0.13f, y + 0.8f, z + 0.02f));
        leftArmObj = glm::rotate(leftArmObj, glm::radians(-15.0f), glm::vec3(0.0, 1.0, 0.0));
        leftArmObj = glm::rotate(leftArmObj, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));
        leftArmObj = glm::scale(leftArmObj, glm::vec3(0.1f, 0.15f, 0.1f));

        // Left hand
        leftHandObj = glm::translate(leftHandObj, glm::vec3(x - 0.05f, y + 0.675f, z - 0.09f));
        leftHandObj = glm::rotate(leftHandObj, glm::radians(-40.0f), glm::vec3(0.0, 1.0, 0.0));
        leftHandObj = glm::rotate(leftHandObj, glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
        leftHandObj = glm::scale(leftHandObj, glm::vec3(0.1f, 0.1f, 0.35f));

        // Right arm
        rightArmObj = glm::translate(rightArmObj, glm::vec3(x + 0.38f, y + 0.8f, z + 0.02f));
        rightArmObj = glm::rotate(rightArmObj, glm::radians(-15.0f), glm::vec3(0.0, 1.0, 0.0));
        rightArmObj = glm::rotate(rightArmObj, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));
        rightArmObj = glm::scale(rightArmObj, glm::vec3(0.1f, 0.15f, 0.1f));

        // Right hand
        rightHandObj = glm::translate(rightHandObj, glm::vec3(x + 0.45f, y + 0.675f, z - 0.09f));
        rightHandObj = glm::rotate(rightHandObj, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
        rightHandObj = glm::rotate(rightHandObj, glm::radians(-10.0f), glm::vec3(1.0, 0.0, 0.0));
        rightHandObj = glm::scale(rightHandObj, glm::vec3(0.1f, 0.1f, 0.35f));

        // Head box
        headObj = glm::translate(headObj, glm::vec3(x + 0.125f, y + 1.07f, z + 0.05f));
        headObj = glm::rotate(headObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        headObj = glm::rotate(headObj, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
        headObj = glm::rotate(headObj, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
        headObj = glm::scale(headObj, glm::vec3(0.25f, 0.25f , 0.25f));
        
        // Sad face applied
        applyTexture(shader, headObj, manFace2Diff, noSpec);

        // Chin
        chinObj = glm::translate(chinObj, glm::vec3(x + 0.125f, y + 0.945f, z + 0.05f));
        chinObj = glm::rotate(chinObj, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
        chinObj = glm::scale(chinObj, glm::vec3(0.24f, 0.01f, 0.24f));

        // Hair
        hairObj = glm::translate(hairObj, glm::vec3(x + 0.125f, y + 1.2f, z + 0.05f));
        hairObj = glm::rotate(hairObj, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
        hairObj = glm::scale(hairObj, glm::vec3(0.25f, 0.01f, 0.25f));

        // Back of head
        backHeadObj = glm::translate(backHeadObj, glm::vec3(x + 0.06f, y + 1.07f, z + 0.16f));
        backHeadObj = glm::rotate(backHeadObj, glm::radians(180.0f), glm::vec3(0.0, 0.0, 1.0));
        backHeadObj = glm::rotate(backHeadObj, glm::radians(30.0f), glm::vec3(0.0, 1.0, 0.0));
        backHeadObj = glm::scale(backHeadObj, glm::vec3(0.25f, 0.25f, 0.01f));

        // Left of head
        leftHeadObj = glm::translate(leftHeadObj, glm::vec3(x + 0.01, y + 1.07f, z - 0.01f));
        leftHeadObj = glm::rotate(leftHeadObj, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        leftHeadObj = glm::rotate(leftHeadObj, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        leftHeadObj = glm::rotate(leftHeadObj, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
        leftHeadObj = glm::scale(leftHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));

        // Right of head
        rightHeadObj = glm::translate(rightHeadObj, glm::vec3(x + 0.23f, y + 1.07f, z + 0.115f));
        rightHeadObj = glm::rotate(rightHeadObj, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        rightHeadObj = glm::rotate(rightHeadObj, glm::radians(-30.0f), glm::vec3(0.0, 0.0, 1.0));
        rightHeadObj = glm::scale(rightHeadObj, glm::vec3(0.01f, 0.25f, 0.25f));
    }

    // NECK ---------------------------------------------------------------------
    neckObj = glm::translate(neckObj, glm::vec3(x + 0.125f, y + 0.92f, z + 0.05f));
    neckObj = glm::scale(neckObj, glm::vec3(0.1f, 0.05f, 0.1f));

    applyTexture(shader, leftShoeObj, manShoeDiff, noSpec);
    applyTexture(shader, rightShoeObj, manShoeDiff, noSpec);
    applyTexture(shader, leftLegObj, manLegsDiff, noSpec);
    applyTexture(shader, rightLegObj, manLegsDiff, noSpec);
    applyTexture(shader, torsoObj, manTopDiff, noSpec);
    applyTexture(shader, backTorsoObj, manTopBackDiff, noSpec);
    applyTexture(shader, leftArmObj, manTopDiff, noSpec);
    applyTexture(shader, leftHandObj, manNeckDiff, noSpec);
    applyTexture(shader, rightArmObj, manTopDiff, noSpec);
    applyTexture(shader, rightHandObj, manNeckDiff, noSpec);
    applyTexture(shader, neckObj, manNeckDiff, noSpec);
    applyTexture(shader, chinObj, manNeckDiff, noSpec);
    applyTexture(shader, hairObj, manHeadTopDiff, noSpec);
    applyTexture(shader, backHeadObj, manHeadBackDiff, noSpec);
    applyTexture(shader, leftHeadObj, manHeadLeftDiff, noSpec);
    applyTexture(shader, rightHeadObj, manHeadRightDiff, noSpec);
}

void bballDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int bballDiff, unsigned int mildSpec)
{
    float scaleAmount;
    glBindVertexArray(VAO);

    glm::mat4 bballObj = glm::mat4();

    if(playAnimation)
    {
        ballDistance = 1.0;
        scaleAmount = sin(glfwGetTime() * 8.0f);
        bballObj = glm::translate(bballObj, glm::vec3(x, y, z));
        bballObj = glm::scale(bballObj, glm::vec3(0.15f, 0.15f, 0.15f));
        bballObj = glm::translate(bballObj, glm::vec3(x, scaleAmount * 1.7f, z));
    }
    else
    {
        ballDistance++;

        if(ballDistance == 150.0)
        {
            ballDistance--;
        }

        scaleAmount = ballDistance * 0.2f;

        bballObj = glm::translate(bballObj, glm::vec3(x, 0.08f, z - 0.15f));
        bballObj = glm::rotate(bballObj, glm::radians(-30.0f), glm::vec3(0.0, 1.0, 0.0));
        bballObj = glm::scale(bballObj, glm::vec3(0.15f, 0.15f, 0.15f));
        bballObj = glm::translate(bballObj, glm::vec3(x, 0.08f, -scaleAmount * 1.5f));
    }

    glTranslatef(1.0, 2.0, 0.0);

    applyTexture(shader, bballObj, bballDiff, mildSpec);
}

void dogDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int dogHeadDiff, unsigned int dogBodyDiff, unsigned int noSpec)
{
    float headScaleZ;
    float bodyScaleZ;
    float legScaleZ;

    glBindVertexArray(VAO);

    glm::mat4 dogHeadObj = glm::mat4();
    glm::mat4 dogBodyObj = glm::mat4();
    glm::mat4 dogLegs1Obj = glm::mat4();
    glm::mat4 dogLegs2Obj = glm::mat4();

    if(playAnimation)
    {
        dogBodyDistance = 1.0;

        // Original position of the dog
        dogHeadObj = glm::translate(dogHeadObj, glm::vec3(x, y, z));
        dogHeadObj = glm::scale(dogHeadObj, glm::vec3(0.15f, 0.15f, 0.25f));
        dogHeadObj = glm::rotate(dogHeadObj, glm::radians(-25.0f), glm::vec3(1.0, 0.0, 0.0));

        dogBodyObj = glm::translate(dogBodyObj, glm::vec3(x, y, z + 0.25f));
        dogBodyObj = glm::scale(dogBodyObj, glm::vec3(0.25f, 0.20f, 0.35f));

        dogLegs1Obj = glm::translate(dogLegs1Obj, glm::vec3(x, 0.0f, z + 0.1f));
        dogLegs1Obj = glm::scale(dogLegs1Obj, glm::vec3(0.20f, 0.2f, 0.05f));

        dogLegs2Obj = glm::translate(dogLegs2Obj, glm::vec3(x, 0.0f, z + 0.4f));
        dogLegs2Obj = glm::scale(dogLegs2Obj, glm::vec3(0.20f, 0.2f, 0.05f));
    }
    else
    {
        dogBodyDistance++;

        // End position animation of the dog jumping
        if(dogBodyDistance == 75.0)
        {
            dogBodyDistance--;

            headScaleZ = sin(glfwGetTime() * 4.0f);
            bodyScaleZ = sin(glfwGetTime() * 4.0f);

            dogHeadObj = glm::translate(dogHeadObj, glm::vec3(x - 3.0f, headScaleZ * 0.1f, z + 3.5f));
            dogBodyObj = glm::translate(dogBodyObj, glm::vec3(x - 3.0f, bodyScaleZ * 0.1f, z + 3.5f));
        }

        headScaleZ = dogBodyDistance * 0.35f;
        bodyScaleZ = dogBodyDistance * 0.25f;
        legScaleZ = dogBodyDistance * 1.45f;

        // Translation path of the dog
        dogHeadObj = glm::translate(dogHeadObj, glm::vec3(x - 0.45f, y + 0.05f, z));
        dogHeadObj = glm::scale(dogHeadObj, glm::vec3(0.15f, 0.15f, 0.25f));
        dogHeadObj = glm::translate(dogHeadObj, glm::vec3(x, y, -headScaleZ));
        dogHeadObj = glm::rotate(dogHeadObj, glm::radians(25.0f), glm::vec3(1.0, 0.0, 0.0));

        dogBodyObj = glm::translate(dogBodyObj, glm::vec3(x - 0.45f, y, z + 0.25f));
        dogBodyObj = glm::scale(dogBodyObj, glm::vec3(0.25f, 0.20f, 0.35f));
        dogBodyObj = glm::translate(dogBodyObj, glm::vec3(x - 1.2f, y, -bodyScaleZ));

        dogLegs1Obj = glm::translate(dogLegs1Obj, glm::vec3(x - 0.45f, y, z + 0.25f));
        dogLegs1Obj = glm::scale(dogLegs1Obj, glm::vec3(0.20f, 0.2f, 0.05f));
        dogLegs1Obj = glm::translate(dogLegs1Obj, glm::vec3(x - 0.75f, y - 1.0f, -legScaleZ * 1.25f));
    }

    applyTexture(shader, dogHeadObj, dogHeadDiff, noSpec);
    applyTexture(shader, dogBodyObj, dogBodyDiff, noSpec);
    // applyTexture(shader, dogLegs1Obj, dogHeadDiff, noSpec);
    // applyTexture(shader, dogLegs2Obj, dogHeadDiff, noSpec);
}

void birdDraw(float x, float y, float z, unsigned int VAO, Shader shader, unsigned int birdDiff,unsigned int noSpec)
{
    float scaleX, scaleY, scaleZ;

    glBindVertexArray(VAO);

    glm::mat4 birdObj = glm::mat4();

    if(playAnimation)
    {
        birdDistance = 1.0;
        scaleY = sin(glfwGetTime() * 4.0f);

        // Original position of bird flying up and down 
        birdObj = glm::translate(birdObj, glm::vec3(x, y, z));
        birdObj = glm::scale(birdObj, glm::vec3(0.1f, 0.1f, 0.15f)); // Scaling
        birdObj = glm::translate(birdObj, glm::vec3(1.0, scaleY, 1.0)); // Up and down animation
        
    }
    else
    {
        birdDistance++;

        // End position of bird flying side to side
        if(birdDistance == 75.0)
        {
            birdDistance--;

            scaleX = sin(glfwGetTime() * 1.5f);
            scaleZ = cos(glfwGetTime() * 1.5f);

            birdObj = glm::translate(birdObj, glm::vec3(-scaleX, y - 1.0f, scaleZ + 1.5f));
        }

        scaleZ = birdDistance * 0.65f;

        // Translation path of the bird
        birdObj = glm::translate(birdObj, glm::vec3(x - 0.18f, y, z));
        birdObj = glm::scale(birdObj, glm::vec3(0.1f, 0.1f, 0.15f));
        birdObj = glm::translate(birdObj, glm::vec3(x, y, -scaleZ));
    }

    applyTexture(shader, birdObj, birdDiff, noSpec);
}

void playFloorDraw(unsigned int VAO, Shader shader, unsigned int playFloorDiff, unsigned int noSpec)
{
    float x = 7.0f;
    float y = -0.05f;
    float z = 7.0f;

    glBindVertexArray(VAO);

    glm::mat4 floorObj = glm::mat4();

    floorObj = glm::translate(floorObj, glm::vec3(x, y, z));
    floorObj = glm::scale(floorObj, glm::vec3(5.0f, -0.1f, -5.0f));

    applyTexture(shader, floorObj, playFloorDiff, noSpec);
}

void swingDraw(unsigned int VAO, Shader shader, unsigned int swingFrameDiff, unsigned int swingRopeDiff, unsigned int swingSeatDiff, unsigned int noSpec, unsigned int mildSpec)
{
    float x = 7.0f;
    float y = -0.05f;
    float z = 7.0f;

    glBindVertexArray(VAO);

    // Base frame objects
    glm::mat4 bf1Obj = glm::mat4();
    glm::mat4 bf2Obj = glm::mat4();
    glm::mat4 bf3Obj = glm::mat4();
    glm::mat4 bf4Obj = glm::mat4();
    glm::mat4 bf5Obj = glm::mat4();
    glm::mat4 bf6Obj = glm::mat4();

    // Cross bar frame object (Bar connecting left and right 'A' frames)
    glm::mat4 barFrameObj = glm::mat4();

    // Seat objects
    glm::mat4 rope1Obj = glm::mat4();
    glm::mat4 rope2Obj = glm::mat4();
    glm::mat4 seatObj = glm::mat4();

    // Base frame transformations
    bf1Obj = glm::translate(bf1Obj, glm::vec3(x, y, z - 2.0f));
    bf1Obj = glm::rotate(bf1Obj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf1Obj = glm::rotate(bf1Obj, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
    bf1Obj = glm::rotate(bf1Obj, glm::radians(5.0f), glm::vec3(0.0, 0.0, 1.0));
    bf1Obj = glm::scale(bf1Obj, glm::vec3(0.1f, 6.0f, 0.1f));

    bf2Obj = glm::translate(bf2Obj, glm::vec3(x + 2.0f, y, z));
    bf2Obj = glm::rotate(bf2Obj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf2Obj = glm::rotate(bf2Obj, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
    bf2Obj = glm::rotate(bf2Obj, glm::radians(5.0f), glm::vec3(0.0, 0.0, 1.0));
    bf2Obj = glm::scale(bf2Obj, glm::vec3(0.1f, 6.0f, 0.1f));

    bf3Obj = glm::translate(bf3Obj, glm::vec3(x + 0.97f, y + 0.5f, z - 0.97f));
    bf3Obj = glm::rotate(bf3Obj, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf3Obj = glm::rotate(bf3Obj, glm::radians(5.0f), glm::vec3(1.0, 0.0, 0.0));
    bf3Obj = glm::scale(bf3Obj, glm::vec3(2.25f, 0.1f, 0.1f));

    bf4Obj = glm::translate(bf4Obj, glm::vec3(x, y, z + 2.0f));
    bf4Obj = glm::rotate(bf4Obj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf4Obj = glm::rotate(bf4Obj, glm::radians(-30.0f), glm::vec3(1.0, 0.0, 0.0));
    bf4Obj = glm::rotate(bf4Obj, glm::radians(-5.0f), glm::vec3(0.0, 0.0, 1.0));
    bf4Obj = glm::scale(bf4Obj, glm::vec3(0.1f, 6.0f, 0.1f));

    bf5Obj = glm::translate(bf5Obj, glm::vec3(x - 2.0f, y, z));
    bf5Obj = glm::rotate(bf5Obj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf5Obj = glm::rotate(bf5Obj, glm::radians(30.0f), glm::vec3(1.0, 0.0, 0.0));
    bf5Obj = glm::rotate(bf5Obj, glm::radians(-5.0f), glm::vec3(0.0, 0.0, 1.0));
    bf5Obj = glm::scale(bf5Obj, glm::vec3(0.1f, 6.0f, 0.1f));

    bf6Obj = glm::translate(bf6Obj, glm::vec3(x - 0.97f, y + 0.5f, z + 0.97f));
    bf6Obj = glm::rotate(bf6Obj, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
    bf6Obj = glm::rotate(bf6Obj, glm::radians(-5.0f), glm::vec3(1.0, 0.0, 0.0));
    bf6Obj = glm::scale(bf6Obj, glm::vec3(2.25f, 0.1f, 0.1f));

    // Cross bar frame transformations
    barFrameObj = glm::translate(barFrameObj, glm::vec3(x, y + 2.5f, z));
    barFrameObj = glm::rotate(barFrameObj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    barFrameObj = glm::rotate(barFrameObj, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    barFrameObj = glm::scale(barFrameObj, glm::vec3(0.1f, 2.5f, 0.1f));

    // Seat transformations
    rope1Obj = glm::translate(rope1Obj, glm::vec3(x, y + 3.0f, z));
    rope1Obj = glm::rotate(rope1Obj, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
    rope1Obj = glm::scale(rope1Obj, glm::vec3(0.1f, 0.1f, 0.1f));

    applyTexture(shader, bf1Obj, swingFrameDiff, noSpec);
    applyTexture(shader, bf2Obj, swingFrameDiff, noSpec);
    applyTexture(shader, bf3Obj, swingFrameDiff, noSpec);
    applyTexture(shader, bf4Obj, swingFrameDiff, noSpec);
    applyTexture(shader, bf5Obj, swingFrameDiff, noSpec);
    applyTexture(shader, bf6Obj, swingFrameDiff, noSpec);
    applyTexture(shader, barFrameObj, swingFrameDiff, noSpec);
    applyTexture(shader, rope1Obj, swingRopeDiff, noSpec);
}