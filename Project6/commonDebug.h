#pragma once

// Definiciones estándar ANSI
#define RESET   "\033[0m"
#define RED     "\033[31m"      // Para Errores
#define GREEN   "\033[32m"      // Para Éxito
#define YELLOW  "\033[33m"      // Para Advertencias
#define BLUE    "\033[34m"      // Para Información

// Info
#ifdef _DEBUG
#define GE_DEBUG_ERROR(x) std::cout << RED << "[DEBUG_ERROR] " << x << RESET << std::endl
#define GE_DEBUG_WARN(x)  std::cout << YELLOW << "[DEBUG_WARN]  " << x << RESET << std::endl
#define GE_DEBUG_INFO(x)  std::cout << GREEN << "[DEBUG_INFO]  " << x << RESET << std::endl
#else
#define GE_DEBUG_ERROR(x)
#define GE_DEBUG_WARN(x)
#define GE_DEBUG_INFO(x)
#endif