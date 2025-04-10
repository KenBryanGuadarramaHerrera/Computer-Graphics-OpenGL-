/*
Práctica 8: Iluminación 2
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;

Texture dado8Texture;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

//COFRE
Model cofre;
//LAMPARA
Model lampara;
//---------------------- MODELO DE LA PARED -----------------------------
Model pared;

//---------------------- MODELO DE LA BATISEÑAL--------------------------
Model batiseñal;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
//--------------- ARREGLOS CREADOS ---------------------------------------
SpotLight spotLights2[MAX_SPOT_LIGHTS];
PointLight pointLights2[MAX_POINT_LIGHTS];

//--------------- BANDERAS PARA EL APAGADO Y ENCENDIDO DE LUCES POINTLIGHT ---------------------------------------
bool lampara_apagada = false;     
bool batiseñal_apagada = false;

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//función de calculo de normales por promedio de vértices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	//---------------------- DIBUJADO DEL ROMBO -----------------------------
	
		unsigned int rombo_indices[] = {

			0, 1, 2,
			3, 4, 5,
			6, 7, 8,
			9, 10, 11,
			12, 13, 14,
			15, 16, 17,
			18, 19, 20,
			21, 22, 23

		};
		// average normals
		GLfloat rombo_vertices[] = {

			//FRONT (UP) // NUMERO 1
			//x		y		z		S		T			NX		NY		NZ
			0.5f, -0.5f,  0.5f,	    0.01f,  0.68f,		-1.0f,	0.0f,	-1.0f,	//0
			0.5f, -0.5f,  -0.5f,    0.32f,  0.68f,		-1.0f,	0.0f,	-1.0f,	//0
			0.0f,  0.5f,  0.0f,	    0.165f,  1.0f,		-1.0f,	0.0f,	-1.0f,	//0

			//LEFT (UP) //NUMERO 2
			//x		y		z		S		T			NX		NY		NZ
			0.5f, -0.5f,  -0.5f,	 0.34f,  0.68f,		1.0f,	0.0f,	1.0f,//0
			-0.5f, -0.5f,  -0.5f,    0.65f,  0.68f,		1.0f,	0.0f,	1.0f,	//0
			0.0f,  0.5f,  0.0f,	     0.495f, 1.0f,		1.0f,	0.0f,	1.0f,	//0

			//BACK(UP) //NUMERO 3
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  -0.5f,	0.67f,  0.68f,		1.0f,	0.0f,	1.0f,//0
			-0.5f, -0.5f,  0.5f,    0.98f,  0.68f,		1.0f,	0.0f,	1.0f,	//0
			0.0f,  0.5f,  0.0f,	    0.825f,  1.0f,		1.0f,	0.0f,	1.0f,	//0

			//RIGHT(UP) // NUMERO 4
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  0.5f,	0.01f,  0.36f,		-1.0f,	0.0f,	-1.0f,//0
			0.5f, -0.5f,  0.5f,     0.32f,  0.36f,		-1.0f,	0.0f,	-1.0f,	//0
			0.0f,  0.5f,  0.0f,	    0.165f,  0.66f,		-1.0f,	0.0f,	-1.0f,	//0

			//-----------------------------------------------------------------------

			//FRONT (DOWN) // NUMERO 5
			//x		y		z		S		T			NX		NY		NZ
			0.5f, -0.5f,  0.5f,	    0.65f,  0.36f,		-1.0f,	0.0f,	-1.0f,	//0
			0.5f, -0.5f,  -0.5f,    0.34f,  0.36f,		-1.0f,	0.0f,	-1.0f,	//0
			0.0f,  -1.5f,  0.0f,	0.495f, 0.66f,		-1.0f,	0.0f,	-1.0f,	//0

			//LEFT (DOWN) // NUMERO 6
			//x		y		z		S		T			NX		NY		NZ
			0.5f, -0.5f,  -0.5f,	 0.99f,  0.34f,		1.0f,	0.0f,	1.0f,//0
			-0.5f, -0.5f,  -0.5f,    0.67f,  0.34f,		1.0f,	0.0f,	1.0f,	//0
			0.0f,  -1.5f,  0.0f,	 0.825f,  0.66f,	1.0f,	0.0f,	1.0f,	//0

			//BACK(DOWN) // NUMERO 7
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  -0.5f,	0.32f,  0.02f,		1.0f,	0.0f,	1.0f,//0
			-0.5f, -0.5f,  0.5f,    0.01f,  0.02f,		1.0f,	0.0f,	1.0f,	//0
			0.0f,  -1.5f,  0.0f,	0.165f,  0.33f,		1.0f,	0.0f,	1.0f,	//0

			//RIGHT(DOWN) // NUMERO 8
			//x		y		z		S		T			NX		NY		NZ
			-0.5f, -0.5f,  0.5f,	0.65f,  0.02f,		0.0f,	0.0f,	-1.0f,//0
			0.5f, -0.5f,  0.5f,     0.34f,  0.02f,		0.0f,	0.0f,	-1.0f,	//0
			0.0f,  -1.5f,  0.0f,	0.495f,  0.33f,		0.0f,	0.0f,	-1.0f,	//0

		};
	

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* rombo = new Mesh();
	rombo->CreateMesh(rombo_vertices, rombo_indices, 192, 24);
	meshList.push_back(rombo); // 5 EN EL MESHLIST

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}





int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();



	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	//------------------------------------------------------------

	dado8Texture = Texture("Textures/Dadode8caras.jpg");
	dado8Texture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/Bati-Carro.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/Bati-LlantasDer.obj");
	cofre = Model();
	cofre.LoadModel("Models/Bati-Cofre.obj");

	lampara = Model();
	lampara.LoadModel("Models/lamparaTexturizada.obj");

	//----------------------  CARGA DEL MODELO DE LA PARED -----------------------------
	pared = Model();
	pared.LoadModel("Models/paredTexturizadaobj.obj");
	//----------------------------------------------------------------------------------

	batiseñal = Model();
	batiseñal.LoadModel("Models/BatiseñalTexturizada.obj");

	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	//---------------------------------------------------------------------------
	//         LUZ DEL PRINCIPAL
	//---------------------------------------------------------------------------
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f, //intensidad ambientak y difusa(saturacion)
		0.0f, -1.0f, 0.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	
	
	
	/*
	//Declaración de primer luz puntual
	//---------------------------------------------------------------------------
	//         PRIMER POINTLIGHT
	//---------------------------------------------------------------------------
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, //intensidad ambientak(intensidad) y difusa(saturacion)
		-6.0f, 1.5f, 10.5f, //VECTOR DE POSICION 
		0.3f, 0.2f, 0.1f); //ATENUACION (RESTRICCION QUE LOS 3 VALORES NO PUEDEN SER 0)
	pointLightCount++; //AL CREACERSE OTRA SE AUMENTA


	*/

	unsigned int spotLightCount = 0;

	//---------------------------------------------------------------------------
	//         LINTERNA
	//---------------------------------------------------------------------------
	
	/*
	
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, //DIR
		1.0f, 0.0f, 0.0f,
		5.0f); //ANGULO DE APERTURA
	spotLightCount++;

	*/
	
	//---------------------------------------------------------------------------
	//         LUZ DEL COFRE
	//---------------------------------------------------------------------------

	spotLights[0] = SpotLight(1.0f, 1.0f, 0.0f,
		5.0f, 10.0f,
		-0.4f, 0.32f, -0.02f, //POS -0.4f, 0.32f, -0.02f
		-1.0f, 0.0f, 0.0f,//DIRECCION 
		1.0f, 0.0f, 0.005f, //EC DE SEGUNDO GRADO PARA ATENUAR (LIMITAR) LA ILUMNIACION
		5.0f); //ANGULO DE APERTURA
	spotLightCount++;
	//---------------------------------------------------------------------------
	//                              LUZ DE HELICOPTERO ----- SPOTLIGHT
	//---------------------------------------------------------------------------

	glm::vec3 chappaPosition = glm::vec3(mainWindow.getmuevex2(), 0.0f, 0.0f); // Posición del coche
	spotLights[1] = SpotLight(1.0f, 1.0f, 0.0f,
		2.0f, 1.0f,//intensidad ambientak(intensidad) y difusa(saturacion)
		-1.2f, 5.0f, 6.0f, //POS
		0.0f, -1.0f, 0.0f,//DIRECCION LE PUSO Y POSUTICA
		0.3f, 0.05f, 0.01f, //EC DE SEGUNDO GRADO PARA ATENUAR (LIMITAR) LA ILUMNIACION
		15.0f); //ANGULO DE APERTURA
	spotLightCount++;
	//---------------------------------------------------------------------------
	//         LUCES DEL FARO
	//---------------------------------------------------------------------------
	glm::vec3 carPosition = glm::vec3( mainWindow.getmuevex(), 0.0f, 0.0f); // Posición del coche
	 // Actualizando la posición de la luz para que siga al coche
	//float valoracomparar = mainWindow.getmuevex();
	spotLights[2] = SpotLight(0.0f, 0.0f, 1.0f,
		5.0f, 10.0f,//intensidad ambientak(intensidad) y difusa(saturacion)
		0.0f,-0.6f, 0.0f, //POS
		-1.0f, 0.0f, 0.0f,//DIRECCION DE LA LUZ
		1.0f, 0.0f, 0.01f, //EC DE SEGUNDO GRADO PARA ATENUAR (LIMITAR) LA ILUMNIACION,,,,,, 0.001f, 0.05f, 0.01f
		15.0f); //ANGULO DE APERTURA
	spotLightCount++;

	spotLights[3] = SpotLight(0.0f, 0.0f, 1.0f,
		5.0f, 10.0f,//intensidad ambientak(intensidad) y difusa(saturacion)
		0.0f, -0.6f, 0.0f, //POS
		1.0f, 0.0f, 0.0f,//DIRECCION DE LA LUZ
		1.0f, 0.0f, 0.01f, //EC DE SEGUNDO GRADO PARA ATENUAR (LIMITAR) LA ILUMNIACION,,,,,, 0.001f, 0.05f, 0.01f
		15.0f); //ANGULO DE APERTURA
	spotLightCount++;

	

	unsigned int spotLightCount2 = 0;
	spotLights2[0] = spotLights[0];
	spotLightCount2++;
	spotLights2[1] = spotLights[1];
	spotLightCount2++;
	spotLights2[2] = spotLights[3];
	spotLightCount2++;
	spotLights2[3] = spotLights[2];
	spotLightCount2++;


	//--------------------------------------------------------------------------
	//                             LUZ DE LA LAMPARA ---- POINTLIGHT
	//---------------------------------------------------------------------------

	
	
	
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 2.0f, //intensidad ambientak(intensidad) y difusa(saturacion)
		0.0f, -1.0f, 15.0f, //VECTOR DE POSICION 
		0.3f, 0.5f, 0.0f); //ATENUACION (RESTRICCION QUE LOS 3 VALORES NO PUEDEN SER 0)
	pointLightCount++;

	//Declaración luz puntual 
	pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
		1.0f, 2.0f, //intensidad ambientak(intensidad) y difusa(saturacion)
		5.0f, 5.0f, -4.0f, //VECTOR DE POSICION 
		0.3f, 0.5f, 0.0f); //ATENUACION (RESTRICCION QUE LOS 3 VALORES NO PUEDEN SER 0)
	pointLightCount++;

	//ARREGLO 1: , LA BATISEÑAL Y LA LAMPARA 

	//ARREGLO 2:, LA LAMPARA Y LA BATISEÑAL

	
	unsigned int pointLightCount2 = 0; 

	pointLights2[0] = pointLights[1];
	pointLightCount2++;
	pointLights2[1] = pointLights[0];
	pointLightCount2++;



	
	

	
	




	
	//se crean mas luces puntuales y spotlight 

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		
		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		/*
		
		// luz ligada a la cámara de tipo flash
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
			glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection()); //LINTERNA

		*/

		
		
		//LUZ HELICOPTERO
		chappaPosition = glm::vec3( mainWindow.getmuevex2(), 5.0f, 6.0f);  // Actualiza la posición del coche
		spotLights[1].SetPos(chappaPosition);
		spotLights2[1].SetPos(chappaPosition);

		//LUZ COFRE SOLUCION ALTERNATIVA ( NO LA USE PORQUE NO ES DIRECTA COMO LAS OTRAS LUCES PERO SI ESTA CONTENIDA EN EL COFRE)
		// ----------------------------------------------------------------------------------------------------
		/*
		
		// Calcular la dirección de la luz basada en el ángulo -0.4f, 0.32f, -0.02f
		carPosition2 = glm::vec3(-0.5f + mainWindow.getmuevex(), 0.32f, -0.02f);
		//spotLights[1].SetPos(carPosition2);
		angle = mainWindow.getarticulacion1();
	
		// Calcular solo la dirección en el eje X en función del ángulo, manteniendo Y y Z fijos
		glm::vec3 lightDirection = glm::vec3(angle, 0.0f, 0.0f);
		spotLights[0].SetFlash(carPosition2,lightDirection);
		spotLights2[0].SetFlash(carPosition2, lightDirection);

		*/
	
		// ----------------------------------------------------------------------------------------------------

		
		/*//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		if (mainWindow.getcambio()) {
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}
		else {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}*/
		shaderList[0].SetDirectionalLight(&mainLight);
		
		//----------------------------- LOGICA PARA LA LAMPARA Y LA BATISEÑAL ---------------------------------------------
		lampara_apagada = mainWindow.getcambio();
		batiseñal_apagada = mainWindow.getcambio2();

		std::cout << std::boolalpha;
		std::cout << "TEST: lampara =  " << lampara_apagada << " | batiseñal: " << batiseñal_apagada << std::endl;

		if (lampara_apagada == true) {
			if (batiseñal_apagada == true) { //AMBAS APAGADAS
				std::cout << std::boolalpha;  // Para que se imprima "true"/"false"
				std::cout << "AMBAS APAGADAS: lampara =  " << lampara_apagada << " | batiseñal: " << batiseñal_apagada << std::endl;
				shaderList[0].SetPointLights(pointLights, pointLightCount-2);
			}
			else { //solo BATISEÑAL
				std::cout << std::boolalpha;
				std::cout << "SOLO BATISEÑAL: lampara =  " << lampara_apagada << " | batiseñal: " << batiseñal_apagada << std::endl;
				shaderList[0].SetPointLights(pointLights, pointLightCount-1);
			}
		}
		else {//solo LAMPARA
			if (batiseñal_apagada == true) {
				std::cout << std::boolalpha;
				std::cout << "SOLO LAMPARA: lampara =  " << lampara_apagada << " | batiseñal: " << batiseñal_apagada << std::endl;
				shaderList[0].SetPointLights(pointLights2, pointLightCount-1);
			}
			else { //AMBAS ENCEDIDAS
				std::cout << std::boolalpha;
				std::cout << "AMBAS ENCEDIDAS: lampara =  " << lampara_apagada << " | batiseñal: " << batiseñal_apagada << std::endl;
				shaderList[0].SetPointLights(pointLights2, pointLightCount);
			}
		}
		
		
	

		
		
		// ----------------------------------------------------------------------------------------------------
		

		//-------------------------------------------------------------------------------------------------------
		
		
		if (mainWindow.getavanzar()) { //AVANZAR
			shaderList[0].SetSpotLights(spotLights, spotLightCount - 1);
			carPosition = glm::vec3(mainWindow.getmuevex(), -0.1f, 0.0f);  // Actualiza la posición del coche
			spotLights[2].SetPos(carPosition);
			spotLights2[2].SetPos(carPosition);
			
		}
		else if (mainWindow.getretroceder()) { //RETROCEDER
			shaderList[0].SetSpotLights(spotLights2, spotLightCount-1);
			carPosition = glm::vec3( mainWindow.getmuevex(), -0.1f, 0.0f);  // Actualiza la posición del coche
			spotLights2[2].SetPos(carPosition);
			spotLights[2].SetPos(carPosition);
		}
		else {
			shaderList[0].SetSpotLights(spotLights, spotLightCount-2);
			
		}
		
		//shaderList[0].SetSpotLights(spotLights, spotLightCount);



		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//Instancia del coche 
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex(), -0.2f, 0.0f));
		modelaux = model;
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();

		//DELANTERA DERECHA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.95f, -0.25f, 0.972f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//DELANTERA IZQUIERDA
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.95f, -0.25f, -0.730f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//TRASERA DERECHA
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.62f, -0.25f, 0.972f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//TRASERA IZQUIERDA
		model = modelaux;
		model = glm::translate(model, glm::vec3(1.62f, -0.25f, -0.730f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Llanta_M.RenderModel();

		//Cofre
		model = modelaux;
		model = glm::translate(model, glm::vec3(-0.4f, 0.32f, -0.02f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(mainWindow.getarticulacion1()), glm::vec3(1.0f, 0.0f, 0.0f));
		spotLights[0].SetFlash(glm::vec3(model * glm::vec4(-0.4f, 0.32f,-0.02f, 1.0f)), glm::normalize(glm::vec3(model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))));
		spotLights2[0].SetFlash(glm::vec3(model * glm::vec4(-0.4f,0.32f,-0.02f,1.0f)), glm::normalize(glm::vec3(model * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f))));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cofre.RenderModel();

	
		// HELICOPTERO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f + mainWindow.getmuevex2(), 5.0f, 6.0));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Blackhawk_M.RenderModel();

		// LAMPARA
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, -4.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara.RenderModel();

		
		
		//PARED DE LADRILLO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-15.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 5.0f, 3.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pared.RenderModel();

		//BATISEÑAL
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 15.0f));
		//model = glm::scale(model, glm::vec3(3.0f, 5.0f, 3.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		batiseñal.RenderModel();

		

		

		//-------------------------------------------------------------------------
		//DADO
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 2.0f, 10.0f));
		modelaux = model;
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dado8Texture.UseTexture();
		meshList[4]->RenderMesh();
		
		//Agave ¿qué sucede si lo renderizan antes del coche y el helicóptero?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
