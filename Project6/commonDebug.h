#pragma once
#include <iostream>

// Definiciones estándar ANSI usando constexpr (C++11 en adelante)
constexpr const char* RESET = "\033[0m";
constexpr const char* RED = "\033[31m";      // Para Errores
constexpr const char* GREEN = "\033[32m";      // Para Éxito
constexpr const char* YELLOW = "\033[33m";      // Para Advertencias
constexpr const char* BLUE = "\033[34m";      // Para Información

// Macros de Info (estas se quedan igual, ya que son funciones/macros de preprocesador)
#ifdef _DEBUG
#define GE_DEBUG_ERROR(x) std::cout << RED << "[DEBUG_ERROR] " << x << RESET << std::endl
#define GE_DEBUG_WARN(x)  std::cout << YELLOW << "[DEBUG_WARN]  " << x << RESET << std::endl
#define GE_DEBUG_INFO(x)  std::cout << GREEN << "[DEBUG_INFO]  " << x << RESET << std::endl
#else
#define GE_DEBUG_ERROR(x)
#define GE_DEBUG_WARN(x)
#define GE_DEBUG_INFO(x)
#endif