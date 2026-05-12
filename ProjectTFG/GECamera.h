/**
 * @file GECamera.h
 * @brief Archivo GECamera.h
 */
#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

/**
 * @struct ObservationPoint
 * @brief Struct ObservationPoint
 */
struct ObservationPoint {
	glm::mat4 location;
	std::string name;
};

/**
 * @class GECamera
 * @brief Class GECamera
 */
class GECamera {
public:

	enum class CameraMode { FREE, FPS, OBSERVING };

	GECamera();
	/**
	 * @brief Función getViewMatrix
	 */
	glm::mat4 getViewMatrix();

	/**
	 * @brief Función setPosition
	 */
	void setPosition(glm::vec3 pos);
	/**
	 * @brief Función setDirection
	 */
	void setDirection(glm::vec3 dir, glm::vec3 up);
	/**
	 * @brief Función setMoveStep
	 */
	void setMoveStep(float step);
	/**
	 * @brief Función setTurnStep
	 */
	void setTurnStep(float step);

	/**
	 * @brief Función getPosition
	 */
	glm::vec3 getPosition();
	/**
	 * @brief Función getDirection
	 */
	glm::vec3 getDirection();
	/**
	 * @brief Función getUpDirection
	 */
	glm::vec3 getUpDirection();
	/**
	 * @brief Función getMoveStep
	 */
	float getMoveStep();
	/**
	 * @brief Función getTurnStep
	 */
	float getTurnStep();

	/**
	 * @brief Función update
	 */
	void update(float deltaTime);

	/**
	 * @brief Función setTurnLeft
	 */
	void setTurnLeft(bool flag);
	/**
	 * @brief Función setTurnRight
	 */
	void setTurnRight(bool flag);
	/**
	 * @brief Función setTurnUp
	 */
	void setTurnUp(bool flag);
	/**
	 * @brief Función setTurnDown
	 */
	void setTurnDown(bool flag);
	/**
	 * @brief Función setTurnCW
	 */
	void setTurnCW(bool flag);
	/**
	 * @brief Función setTurnCCW
	 */
	void setTurnCCW(bool flag);
	/**
	 * @brief Función setMoveLeft
	 */
	void setMoveLeft(bool flag);
	/**
	 * @brief Función setMoveRight
	 */
	void setMoveRight(bool flag);
	/**
	 * @brief Función setMoveUp
	 */
	void setMoveUp(bool flag);
	/**
	 * @brief Función setMoveDown
	 */
	void setMoveDown(bool flag);

	/**
	 * @brief Función setNextMode
	 */
	void setNextMode();
	/**
	 * @brief Función getCurrentMode
	 */
	CameraMode getCurrentMode() { return currentMode; }

	/**
	 * @brief Función processMouse
	 */
	void processMouse(float xoffset, float yoffset);
	/**
	 * @brief Función addObservationPoint
	 */
	void addObservationPoint(glm::mat4 location, std::string s);
	/**
	 * @brief Función nextObservationPoint
	 */
	void nextObservationPoint();
	/**
	 * @brief Función prevObservationPoint
	 */
	void prevObservationPoint();
	/**
	 * @brief Función getCurrentObservationName
	 */
	std::string getCurrentObservationName();

	/**
	 * @brief Función setMoveFront
	 */
	void setMoveFront(bool flag);
	/**
	 * @brief Función setMoveBack
	 */
	void setMoveBack(bool flag);

	void setObservationDistance(float dist) { observationDistance = dist; }
	/**
	 * @brief Función getObservationDistance
	 */
	float getObservationDistance() const { return observationDistance; }
	// Función para detener cualquier movimiento residual
	/**
	 * @brief Función stopAllMovement
	 */
	void stopAllMovement();

private:

	std::vector<ObservationPoint> observationPoints;
	int currentObservationIndex = 0;

	CameraMode currentMode = CameraMode::FREE;

	glm::vec3 Pos;
	glm::vec3 Dir;
	glm::vec3 Up;
	glm::vec3 Right;

	float moveStep;
	float turnStep;
	float cosAngle;
	float sinAngle;

	bool turnLeftPressed;
	bool turnRightPressed;
	bool turnUpPressed;
	bool turnDownPressed;
	bool turnCWPressed;
	bool turnCCWPressed;
	bool moveLeftPressed;
	bool moveRightPressed;
	bool moveUpPressed;
	bool moveDownPressed;

	bool moveFrontPressed;
	bool moveBackPressed;

	float observationDistance = 8.0f; // Distancia inicial por defecto

	/**
	 * @brief Función turnRight
	 */
	void turnRight();
	/**
	 * @brief Función turnLeft
	 */
	void turnLeft();
	/**
	 * @brief Función turnUp
	 */
	void turnUp();
	/**
	 * @brief Función turnDown
	 */
	void turnDown();
	/**
	 * @brief Función turnCW
	 */
	void turnCW();
	/**
	 * @brief Función turnCCW
	 */
	void turnCCW();

	/**
	 * @brief Función moveLeft
	 */
	void moveLeft();
	/**
	 * @brief Función moveRight
	 */
	void moveRight();
	/**
	 * @brief Función moveUp
	 */
	void moveUp();
	/**
	 * @brief Función moveDown
	 */
	void moveDown();
	/**
	 * @brief Función moveFront
	 */
	void moveFront();
	/**
	 * @brief Función moveBack
	 */
	void moveBack();

	float yaw = -90.0f; // -90 para mirar hacia -Z por defecto
	float pitch = 0.0f;
	/**
	 * @brief Función updateCameraVectorsFromEuler
	 */
	void updateCameraVectorsFromEuler();
};