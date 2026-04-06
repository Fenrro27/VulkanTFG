#include "F22_Raptor.h"
#include "F22_Raptor_pieces.h"

#include "GEObject.h"
#include "GEPiece.h"

F22_Raptor::F22_Raptor(GEGraphicsContext* gc, GERenderingContext* rc)
{
	model = glm::mat4(1.0f);

	materials.resize(3);
	textures.resize(3);
	pieces.resize(22);

	materials[0] = {};
	materials[0].Ka = glm::vec3(0.117647f, 0.117647f, 0.117647f);
	materials[0].Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[0].Ks = glm::vec3(0.752941f, 0.752941f, 0.752941f);
	materials[0].Shininess = 8.0f;

	textures[0] = new GETexture(gc, "textures/F22_Raptor_P01.png");

	materials[1] = {};
	materials[1].Ka = glm::vec3(0.117647f, 0.117647f, 0.117647f);
	materials[1].Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[1].Ks = glm::vec3(0.752941f, 0.752941f, 0.752941f);
	materials[1].Shininess = 8.0f;

	textures[1] = new GETexture(gc, "textures/Glass_Cockpit.png");

	materials[2] = {};
	materials[2].Ka = glm::vec3(0.117647f, 0.117647f, 0.117647f);
	materials[2].Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[2].Ks = glm::vec3(0.752941f, 0.752941f, 0.752941f);
	materials[2].Shininess = 8.0f;

	textures[2] = new GETexture(gc, "textures/Pilot_usa.png");

	pieces[0] = new F22_Raptor_0();
	pieces[0]->setMaterial(materials[0]);
	pieces[0]->setTexture(textures[0]);
	pieces[0]->initialize(gc, rc);

	pieces[1] = new F22_Raptor_1();
	pieces[1]->setMaterial(materials[0]);
	pieces[1]->setTexture(textures[0]);
	pieces[1]->initialize(gc, rc);

	pieces[2] = new F22_Raptor_21();
	pieces[2]->setMaterial(materials[0]);
	pieces[2]->setTexture(textures[0]);
	pieces[2]->initialize(gc, rc);

	pieces[3] = new F22_Raptor_3();
	pieces[3]->setMaterial(materials[0]);
	pieces[3]->setTexture(textures[0]);
	pieces[3]->initialize(gc, rc);

	pieces[4] = new F22_Raptor_4();
	pieces[4]->setMaterial(materials[0]);
	pieces[4]->setTexture(textures[0]);
	pieces[4]->initialize(gc, rc);

	pieces[5] = new F22_Raptor_5();
	pieces[5]->setMaterial(materials[2]);
	pieces[5]->setTexture(textures[2]);
	pieces[5]->initialize(gc, rc);

	pieces[6] = new F22_Raptor_6();
	pieces[6]->setMaterial(materials[0]);
	pieces[6]->setTexture(textures[0]);
	pieces[6]->initialize(gc, rc);

	pieces[7] = new F22_Raptor_7();
	pieces[7]->setMaterial(materials[0]);
	pieces[7]->setTexture(textures[0]);
	pieces[7]->initialize(gc, rc);

	pieces[8] = new F22_Raptor_8();
	pieces[8]->setMaterial(materials[0]);
	pieces[8]->setTexture(textures[0]);
	pieces[8]->initialize(gc, rc);

	pieces[9] = new F22_Raptor_9();
	pieces[9]->setMaterial(materials[0]);
	pieces[9]->setTexture(textures[0]);
	pieces[9]->initialize(gc, rc);

	pieces[10] = new F22_Raptor_10();
	pieces[10]->setMaterial(materials[0]);
	pieces[10]->setTexture(textures[0]);
	pieces[10]->initialize(gc, rc);

	pieces[11] = new F22_Raptor_11();
	pieces[11]->setMaterial(materials[0]);
	pieces[11]->setTexture(textures[0]);
	pieces[11]->initialize(gc, rc);

	pieces[12] = new F22_Raptor_12();
	pieces[12]->setMaterial(materials[0]);
	pieces[12]->setTexture(textures[0]);
	pieces[12]->initialize(gc, rc);

	pieces[13] = new F22_Raptor_13();
	pieces[13]->setMaterial(materials[0]);
	pieces[13]->setTexture(textures[0]);
	pieces[13]->initialize(gc, rc);

	pieces[14] = new F22_Raptor_14();
	pieces[14]->setMaterial(materials[0]);
	pieces[14]->setTexture(textures[0]);
	pieces[14]->initialize(gc, rc);

	pieces[15] = new F22_Raptor_15();
	pieces[15]->setMaterial(materials[0]);
	pieces[15]->setTexture(textures[0]);
	pieces[15]->initialize(gc, rc);

	pieces[16] = new F22_Raptor_16();
	pieces[16]->setMaterial(materials[0]);
	pieces[16]->setTexture(textures[0]);
	pieces[16]->initialize(gc, rc);

	pieces[17] = new F22_Raptor_17();
	pieces[17]->setMaterial(materials[0]);
	pieces[17]->setTexture(textures[0]);
	pieces[17]->initialize(gc, rc);

	pieces[18] = new F22_Raptor_18();
	pieces[18]->setMaterial(materials[0]);
	pieces[18]->setTexture(textures[0]);
	pieces[18]->initialize(gc, rc);

	pieces[19] = new F22_Raptor_19();
	pieces[19]->setMaterial(materials[0]);
	pieces[19]->setTexture(textures[0]);
	pieces[19]->initialize(gc, rc);

	pieces[20] = new F22_Raptor_20();
	pieces[20]->setMaterial(materials[0]);
	pieces[20]->setTexture(textures[0]);
	pieces[20]->initialize(gc, rc);

	pieces[21] = new F22_Raptor_2();
	pieces[21]->setMaterial(materials[1]);
	pieces[21]->setTexture(textures[1]);
	pieces[21]->initialize(gc, rc);

}

