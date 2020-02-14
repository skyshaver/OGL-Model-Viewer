//// use a uniform to change color over time
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderObject.ID, "ourColor");
		//glUniform4f(vertexColorLocation, 0.f, greenValue, 0.f, 1.f);

		//// use a uniform to change hpos  over time
		//float timeValue = glfwGetTime();
		//float hValue = (sin(timeValue) / 2.0f) + 0.5f;
		//shaderObject.setFloat("hOffset", hValue);

		//// transform location
		//glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		//unsigned int transformLoc = glGetUniformLocation(shaderObject.ID, "transform");
		//glad_glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));



//// set up vertex data and buffers
	//float vertices[] = {
	//	0.5f,  0.5f, 0.0f,  // top right
	//	0.5f, -0.5f, 0.0f,  // bottom right
	//	-0.5f, -0.5f, 0.0f,  // bottom left
	//	-0.5f,  0.5f, 0.0f   // top left 
	//};

	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 3,   // first triangle
	//	1, 2, 3    // second triangle
	//};

	//// three triangles w/ color data
	//float vertices[] = {
	//	// pos				// colors
	//	-1.f,  0.0f, 0.0f,  1.f, 0.f, 0.f,
	//	-0.5f, 1.f, 0.0f,	0.f, 1.f, 0.f,
	//	0.0f, 0.0f, 0.0f,	0.f, 0.f, 1.f,
	//	-0.5f, -1.f, 0.0f,  1.f, 0.f, 0.f,
	//	0.f,  0.0f, 0.0f,	0.f, 1.f, 0.f,
	//	0.5f, -1.f, 0.0f,	0.f, 0.f, 1.f,
	//	1.f,  0.0f, 0.0f,	1.f, 0.f, 0.f,
	//	0.5f, 1.f, 0.0f,	0.f, 1.f, 0.f,
	//	0.0f, 0.0f, 0.0f,	0.f, 0.f, 1.f
	//};

	//unsigned int indices[] = {  // note that we start from 0!
	//	0, 1, 2,   // first triangle
	//	3, 2, 5,
	//	6, 7, 2		
	//};
	//// end

	// two triangles forming a squared w/ texture data
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};


	//// camera
		//glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 3.f);
		//glm::vec3 cameraTarget = glm::vec3(0.f, 0.f, 0.f);
		//glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
		//glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
		//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
		//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

		/*const float radius = 10.f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;*/

	// create transformations
		// glm::mat4 model = glm::mat4(1.f); // init to identity matrix first

	//model = glm::rotate(model, glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));
		// rotate model over time
		// model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.f), glm::vec3(0.5f, 1.f, 1.f));

	// get uniforms from shader
		/*unsigned int modelLoc = glad_glGetUniformLocation(shaderObject.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));*/

	//// mix offset to blend the two textures
		//float mixOffset = 0.2f;
		//shaderObject.setFloat("mixOffset", mixOffset);