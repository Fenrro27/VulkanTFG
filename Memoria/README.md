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

## Graphify (mapa de conocimiento)

El repositorio raíz tiene configurado [Graphify](https://github.com/Graphify-Labs/graphify) (paquete PyPI `graphifyy`), que indexa el código del motor en `graphify-out/`. Para regenerarlo desde la raíz:

```bash
python3 -m venv .venv
.venv/bin/pip install graphifyy
.venv/bin/graphify extract . --code-only   # desde la raíz del repo
```

Los diagramas de arquitectura de `Desarrollo.tex` (p. ej. la jerarquía de ejecución en TikZ) pueden consultarse junto con el reporte del grafo (`graphify-out/GRAPH_REPORT.md`).
