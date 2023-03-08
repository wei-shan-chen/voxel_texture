#include "Drawmodel.h"


MatrixStack model;
MatrixStack view;
MatrixStack projection;

Shader ourShader;


void OurShader_Create();
void Shader_init(glm::vec3 position, glm::vec3 front);
void Shader_use();
void ViewProjection_Create(glm::mat4 viewMatrix, float zoom, int width, int height);
void Model_Floor_Create(unsigned int VAO);
void Model_Create(unsigned int VAO, int numVoxelFace, float x, float z);
void Model_lattice_Create(unsigned int VAO, int size);
void OurShader_Create()
{
    ourShader = Shader("shader/lightShader.vs", "shader/lightShader.fs");
}
void Shader_init(glm::vec3 position, glm::vec3 front){

		ourShader.use();
		ourShader.setVec3("viewPos", position);
        ourShader.setVec3("lightPos", glm::vec3(-150.0f, 200.0f, 600.0f));
	    // directional light
        // ourShader.setVec3("dirLight.direction", front);
        // ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        // ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        // ourShader.setVec3("dirLight.specular", 0.4f, 0.4f, 0.4f);	
}   

void ViewProjection_Create(glm::mat4 viewMatrix, float zoom, int width, int height){
    view.Save(viewMatrix);
    projection.Save(glm::perspective(glm::radians(zoom), (float)width / (float)height, 0.1f, 1000.0f));
    ourShader.setMat4("view", view.Top());
    ourShader.setMat4("projection", projection.Top());
}

void Model_Create(unsigned int VAO, int numVoxelFace, float x, float z){
    model.Push();
    // model.Save(glm::scale(model.Top(), glm::vec3( 0.1, 0.1, 0.1)));
    // model.Save(glm::translate(model.Top(), glm::vec3(2*x,0,0)));
    // model.Save(glm::rotate(model.Top(),glm::radians(90.0f),glm::vec3(0.0f,0.0f,1.0f)));
    // ourShader.setVec3("objectColor", 1.0f, 0.0f, 0.0f);
    ourShader.setMat4("model", model.Top());
    ourShader.setBool("shader",true);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * numVoxelFace);
    model.Pop();
}
void Model_Floor_Create(unsigned int VAO){
    model.Push();
    // model.Save(glm::scale(model.Top(), glm::vec3( 20.0f, 1.0f, 20.0f)));
    model.Save(glm::scale(model.Top(), glm::vec3( 2000.0f, 1.0f, 2000.0f)));
    // model.Save(glm::translate(model.Top(), glm::vec3(-0.5f, 10.0f, -0.5)));
    model.Save(glm::translate(model.Top(), glm::vec3(-0.5f, 0.0f, -0.5)));
    // ourShader.setVec3("objectColor", 0.0f, 1.0f, 1.0f);
    ourShader.setMat4("model", model.Top());
    ourShader.setBool("shader",true);
    // ourShader.setBool("shader",false);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    model.Pop();

}
void Model_latticeTri_Create(unsigned int VAO, int size){
    model.Push();
    // model.Save(glm::translate(model.Top(),camera.Position));
    ourShader.setMat4("model", model.Top());
    ourShader.setBool("shader",false);
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, size);
    model.Pop();
}
void Model_lattice_Create(unsigned int VAO, int size){
    model.Push();
    // model.Save(glm::translate(model.Top(),camera.Position));
    ourShader.setMat4("model", model.Top());
    ourShader.setBool("shader",false);
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, size);
    model.Pop();
    
}