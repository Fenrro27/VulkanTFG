#include "GECamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
// FUNCIÓN: GECamera::GECamera()
//
// PROPÓSITO: Construye una cámara
//
// COMENTARIOS: 
//     La posición inicial es (0,0,0).
//     La orientación incial es el sistema de coordenadas del modelo
//     El tamaño del paso inicial es 0.1
//     El támaño del giro inicial es 1.0 grados
//
GECamera::GECamera()
{
	Pos = glm::vec3(0.0f, 0.0f, 0.0f);
	Dir = glm::vec3(0.0f, 0.0f, 1.0f);
	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	Right = glm::vec3(1.0f, 0.0f, 0.0f);

	moveStep = 0.1f;
	turnStep = 1.0f;
	cosAngle = cos(glm::radians(turnStep));
	sinAngle = sin(glm::radians(turnStep));

	turnLeftPressed = false;
	turnRightPressed = false;
	turnUpPressed = false;
	turnDownPressed = false;
	turnCWPressed = false;
	turnCCWPressed = false;
	moveLeftPressed = false;
	moveRightPressed = false;
	moveUpPressed = false;
	moveDownPressed = false;

	moveFrontPressed = false;
	moveBackPressed = false;
}

//
// FUNCIÓN: GECamera::getViewMatrix()
//
// PROPÓSITO: Obtiene la matriz View para situar la cámara.
//
glm::mat4 GECamera::getViewMatrix()
{
	// Si estamos en modo observador, calculamos la vista de órbita
	if (currentMode == CameraMode::OBSERVING) {
		glm::vec3 targetPos;

		if (observationPoints.empty()) {
			// Comportamiento por defecto: Lista vacía -> Mirar al centro (0,0,0)
			targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else {
			// Comportamiento normal: Mirar al elemento actual de la lista
			targetPos = glm::vec3(observationPoints[currentObservationIndex].location[3]);
		}

		// La cámara se coloca a 8 unidades de distancia del objetivo, en la dirección contraria a donde mira
		glm::vec3 offsetPos = targetPos + (Dir * observationDistance);

		return glm::lookAt(offsetPos, targetPos, Up);
	}

	// Comportamiento para modos FREE y FPS
	return glm::lookAt(Pos, Pos - Dir, Up);
}

//
// FUNCIÓN: GECamera::setPosition(glm::vec3 pos)
//
// PROPÓSITO: Asigna la posición de la cámara con respecto al sistema de coordenadas del modelo.
//
void GECamera::setPosition(glm::vec3 pos)
{
	Pos = glm::vec3(pos);
}

//
// FUNCIÓN: GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
//
// PROPÓSITO: Asigna la orientación de la cámara.
//
void GECamera::setDirection(glm::vec3 dir, glm::vec3 up)
{
	Dir = glm::vec3(dir);
	Up = glm::vec3(up);
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::setMoveStep(float step)
//
// PROPÓSITO: Asigna el avance en cada paso.
//
void GECamera::setMoveStep(float step)
{
	moveStep = step;
}

//
// FUNCIÓN: GECamera::setTurnStep(float step)
//
// PROPÓSITO: Asigna el ángulo de giro en cada paso.
//
void GECamera::setTurnStep(float step)
{
	turnStep = step;
	cosAngle = cos(glm::radians(turnStep));
	sinAngle = sin(glm::radians(turnStep));
}

//
// FUNCIÓN: GECamera::getPosition()
//
// PROPÓSITO: Obtiene la posición de la cámara.
//
glm::vec3 GECamera::getPosition()
{
	return Pos;
}

//
// FUNCIÓN: GECamera::getDirection()
//
// PROPÓSITO: Obtiene la orientación de la cámara (eje Z).
//
glm::vec3 GECamera::getDirection()
{
	return Dir;
}

//
// FUNCIÓN: GECamera::getUpDirection()
//
// PROPÓSITO: Obtiene la orientación cenital de la cámara (eje Y).
//
glm::vec3 GECamera::getUpDirection()
{
	return Up;
}

//
// FUNCIÓN: GECamera::getMoveStep()
//
// PROPÓSITO: Obtiene el avance en cada paso.
//
float GECamera::getMoveStep()
{
	return moveStep;
}

//
// FUNCIÓN: GECamera::getTurnStep()
//
// PROPÓSITO: Obtiene el ángulo de giro en cada paso.
//
float GECamera::getTurnStep()
{
	return turnStep;
}

//
// FUNCIÓN: GECamera::update()
//
// PROPÓSITO: Actualiza la posición y orientación de la cámara 
//
void GECamera::update(float deltaTime)
{
	if (currentMode == CameraMode::FREE) {
		if (turnLeftPressed && !turnRightPressed) turnLeft();
		if (!turnLeftPressed && turnRightPressed) turnRight();
		if (turnUpPressed && !turnDownPressed) turnUp();
		if (!turnUpPressed && turnDownPressed) turnDown();
		if (turnCWPressed && !turnCCWPressed) turnCW();
		if (!turnCWPressed && turnCCWPressed) turnCCW();

		if (moveLeftPressed && !moveRightPressed) moveLeft();
		if (!moveLeftPressed && moveRightPressed) moveRight();
		if (moveUpPressed && !moveDownPressed) moveUp();
		if (!moveUpPressed && moveDownPressed) moveDown();

		moveFront(); // Comportamiento normal (se mueve constantemente hacia adelante)
	}
	else if (currentMode == CameraMode::FPS) {
		// En FPS solo nos movemos si las teclas están siendo pulsadas
		if (moveFrontPressed && !moveBackPressed) moveFront();
		if (!moveFrontPressed && moveBackPressed) moveBack();
		if (moveLeftPressed && !moveRightPressed) moveLeft();
		if (!moveLeftPressed && moveRightPressed) moveRight();
		if (moveUpPressed && !moveDownPressed) moveUp();
		if (!moveUpPressed && moveDownPressed) moveDown();
	}
}

//
// FUNCIÓN: GECamera::moveFront()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) en la dirección -Dir 
//
void GECamera::moveFront()
{
	Pos -= moveStep * Dir;
}

//
// FUNCIÓN: GECamera::moveBack()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) hacia atrás en la dirección Dir 
//
void GECamera::moveBack()
{
	Pos += moveStep * Dir;
}

//
// FUNCIÓN: GECamera::moveLeft()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) hacia la izquierda. 
//
void GECamera::moveLeft()
{
//	Pos -= moveStep * Right;
	Pos -= 0.1f * Right;
}

//
// FUNCIÓN: GECamera::moveRight()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) hacia la derecha. 
//
void GECamera::moveRight()
{
//	Pos += moveStep * Right;
	Pos += 0.1f * Right;
}

//
// FUNCIÓN: GECamera::moveUp()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) hacia arriba. 
//
void GECamera::moveUp()
{
//	Pos += moveStep * Up;
	Pos += 0.1f * Up;
}

//
// FUNCIÓN: GECamera::moveDown()
//
// PROPÓSITO: Mueve el observador un paso (moveStep) hacia abajo. 
//
void GECamera::moveDown()
{
//	Pos -= moveStep * Up;
	Pos -= 0.1f * Up;
}

//
// FUNCIÓN: GECamera::turnRight()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia su derecha.
//
void GECamera::turnRight()
{
	Dir.x = cosAngle * Dir.x - sinAngle * Right.x;
	Dir.y = cosAngle * Dir.y - sinAngle * Right.y;
	Dir.z = cosAngle * Dir.z - sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: CACamera::turnLeft()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia su izquierda.
//
void GECamera::turnLeft()
{
	Dir.x = cosAngle * Dir.x + sinAngle * Right.x;
	Dir.y = cosAngle * Dir.y + sinAngle * Right.y;
	Dir.z = cosAngle * Dir.z + sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::turnUp()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia arriba.
//
void GECamera::turnUp()
{
	Dir.x = cosAngle * Dir.x - sinAngle * Up.x;
	Dir.y = cosAngle * Dir.y - sinAngle * Up.y;
	Dir.z = cosAngle * Dir.z - sinAngle * Up.z;

	// Up = Dir x Right
	Up = glm::cross(Dir, Right);
}

//
// FUNCIÓN: GECamera::turnDown()
//
// PROPÓSITO: Rota el observador un paso (angleStep) hacia abajo.
//
void GECamera::turnDown()
{
	Dir.x = cosAngle * Dir.x + sinAngle * Up.x;
	Dir.y = cosAngle * Dir.y + sinAngle * Up.y;
	Dir.z = cosAngle * Dir.z + sinAngle * Up.z;

	// Up = Dir x Right
	Up = glm::cross(Dir, Right);
}

//
// FUNCIÓN: GECamera::turnCW()
//
// PROPÓSITO: Rota el observador un paso (angleStep) en sentido del reloj.
//
void GECamera::turnCW()
{
	Up.x = cosAngle * Up.x + sinAngle * Right.x;
	Up.y = cosAngle * Up.y + sinAngle * Right.y;
	Up.z = cosAngle * Up.z + sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::turnCCW()
//
// PROPÓSITO: Rota el observador un paso (angleStep) en sentido contrario al reloj.
//
void GECamera::turnCCW()
{
	Up.x = cosAngle * Up.x - sinAngle * Right.x;
	Up.y = cosAngle * Up.y - sinAngle * Right.y;
	Up.z = cosAngle * Up.z - sinAngle * Right.z;

	// Right = Up x Dir
	Right = glm::cross(Up, Dir);
}

//
// FUNCIÓN: GECamera::setTurnLeft(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro a la izquierda
//
void GECamera::setTurnLeft(bool flag)
{
	turnLeftPressed = flag;
}

//
// FUNCIÓN: GECamera::setTurnRight(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro a la derecha
//
void GECamera::setTurnRight(bool flag)
{
	turnRightPressed = flag;
}

//
// FUNCIÓN: GECamera::setTurnUp(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro hacia arriba
//
void GECamera::setTurnUp(bool flag)
{
	turnUpPressed = flag;
}

//
// FUNCIÓN: GECamera::setTurnDown(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro hacia abajo
//
void GECamera::setTurnDown(bool flag)
{
	turnDownPressed = flag;
}

//
// FUNCIÓN: GECamera::setTurnCW(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro horario
//
void GECamera::setTurnCW(bool flag)
{
	turnCWPressed = flag;
}

//
// FUNCIÓN: GECamera::setTurnCCW(bool flag)
//
// PROPÓSITO: Activa o desactiva el giro antihorario
//
void GECamera::setTurnCCW(bool flag)
{
	turnCCWPressed = flag;
}

//
// FUNCIÓN: GECamera::setMoveLeft(bool flag)
//
// PROPÓSITO: Activa o desactiva el desplazamiento a la izquierda
//
void GECamera::setMoveLeft(bool flag)
{
	moveLeftPressed = flag;
}

//
// FUNCIÓN: GECamera::setMoveRight(bool flag)
//
// PROPÓSITO: Activa o desactiva el desplazamiento a la derecha
//
void GECamera::setMoveRight(bool flag)
{
	moveRightPressed = flag;
}

//
// FUNCIÓN: GECamera::setMoveUp(bool flag)
//
// PROPÓSITO: Activa o desactiva el desplazamiento hacia arriba
//
void GECamera::setMoveUp(bool flag)
{
	moveUpPressed = flag;
}

//
// FUNCIÓN: GECamera::setMoveDown(bool flag)
//
// PROPÓSITO: Activa o desactiva el desplazamiento hacia abajo
//
void GECamera::setMoveDown(bool flag)
{
	moveDownPressed = flag;
}

//
// FUNCIÓN: GECamera::setNextMode()
//
void GECamera::setNextMode()
{
	// Guardar la posición orbital al salir de OBSERVING
	if (currentMode == CameraMode::OBSERVING) {
		glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);

		if (!observationPoints.empty()) {
			targetPos = glm::vec3(observationPoints[currentObservationIndex].location[3]);
		}

		// Sobrescribimos 'Pos' con la posición real en la que estaba la cámara.
		// Así, al entrar a modo FREE, continuará el vuelo desde aquí sin saltos bruscos.
		Pos = targetPos + (Dir * observationDistance);
	}

	// Ciclo entre FREE -> FPS -> OBSERVING -> FREE
	if (currentMode == CameraMode::FREE) {
		currentMode = CameraMode::FPS;
	}
	else if (currentMode == CameraMode::FPS) {
		currentMode = CameraMode::OBSERVING;
	}
	else {
		currentMode = CameraMode::FREE;
	}

	// Si entramos en un modo basado en Euler, sincronizamos ángulos con la dirección actual
	if (currentMode == CameraMode::FPS || currentMode == CameraMode::OBSERVING)
	{
		glm::vec3 front = glm::normalize(Dir);
		pitch = glm::degrees(asin(front.y));
		yaw = glm::degrees(atan2(front.z, front.x));

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;
	}
}

void GECamera::processMouse(float xoffset, float yoffset)
{
	// Ahora el ratón afecta tanto a FPS como a Tercera Persona
	if (currentMode == CameraMode::FREE) return;

	float sensitivity = 0.1f;
	yaw += xoffset * sensitivity;
	pitch += yoffset * sensitivity;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	updateCameraVectorsFromEuler();
}

void GECamera::updateCameraVectorsFromEuler()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	Dir = glm::normalize(front);
	Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
	Up = glm::normalize(glm::cross(Right, Dir));
}

// Añade una nueva matriz de localización a la lista
void GECamera::addObservationPoint(glm::mat4 location, std::string s)
{
	observationPoints.push_back({ location,s });
}

// Cambia al siguiente punto a observar
void GECamera::nextObservationPoint()
{
	if (!observationPoints.empty()) {
		currentObservationIndex = (currentObservationIndex + 1) % observationPoints.size();
	}
}

// Cambia al punto anterior a observar
void GECamera::prevObservationPoint()
{
	if (!observationPoints.empty()) {
		currentObservationIndex--;
		if (currentObservationIndex < 0) {
			currentObservationIndex = (int)observationPoints.size() - 1;
		}
	}
}


//
// FUNCIÓN: GECamera::getCurrentObservationName()
//
std::string GECamera::getCurrentObservationName()
{
	// Si no hay nada añadido, devolvemos "null" explícitamente
	if (observationPoints.empty()) {
		return "null";
	}

	// Si hay elementos, devolvemos el nombre del elemento actual
	return observationPoints[currentObservationIndex].name;
}


void GECamera::setMoveFront(bool flag)
{
	moveFrontPressed = flag;
}

void GECamera::setMoveBack(bool flag)
{
	moveBackPressed = flag;
}

void GECamera::stopAllMovement()
{
	// Reseteamos la velocidad de avance continuo
	moveStep = 0.0f;

	// Reseteamos todas las teclas de movimiento
	moveFrontPressed = false;
	moveBackPressed = false;
	moveLeftPressed = false;
	moveRightPressed = false;
	moveUpPressed = false;
	moveDownPressed = false;

	// Reseteamos también las de rotación por si acaso
	turnLeftPressed = false;
	turnRightPressed = false;
	turnUpPressed = false;
	turnDownPressed = false;
	turnCWPressed = false;
	turnCCWPressed = false;
}