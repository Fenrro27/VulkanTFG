# Memoria TFG - Sistema de Partículas en Vulkan

Este documento latex contiene la memoria principal de un Trabajo de Fin de Grado sobre la implementación de un motor de renderizado de partículas en Vulkan.

## Estructura

- `memoria.tex`: archivo principal del documento.
- `Secciones/`: contiene los capítulos y secciones del trabajo.
- `Media/`: imágenes y recursos usados en la portada y el documento.
- `bibliografia.bib`: archivo de bibliografía.

## Compilación

Para compilar el documento con `latexmk`:

```bash
latexmk -pdf memoria.tex
```

Si prefieres usar `pdflatex` y `bibtex` manualmente:

```bash
pdflatex memoria.tex
bibtex memoria
pdflatex memoria.tex
pdflatex memoria.tex
```

## Notas

- `Portada.tex`, `Resumen.tex`, `Abstract.tex` y `Agradecimientos.tex` están en `Secciones/`.
- El archivo `memoria.tex` importa todas las secciones desde `Secciones/`.
