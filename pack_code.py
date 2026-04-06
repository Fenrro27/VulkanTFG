import os
import re

# --- CONFIGURACIÓN ---
EXTENSIONES = ('.h', '.cpp', '.hpp', '.c', '.comp', '.vert', '.frag')

# Carpetas que se saltará por completo
CARPETAS_IGNORAR = {
    '.git', '.vs', 'x64', 'x86', 'Debug', 'Release', 
    'bin', 'obj', 'packages', 'external', 'libs', 'glm', 'glfw'
}

# Nombres exactos de archivos que quieres excluir
ARCHIVOS_IGNORAR = {
	'F22_Raptor_pieces.cpp',
	'F22_Raptor.cpp'
}

ARCHIVO_SALIDA = 'codigo_limpio.txt'

def clean_code(text):
    # 1. Eliminar comentarios de bloque /* ... */
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    
    # 2. Eliminar comentarios de línea // ...
    text = re.sub(r'(?<!:)//.*', '', text)
    
    # 3. Limpiar líneas en blanco múltiples
    lines = text.splitlines()
    cleaned_lines = []
    last_was_blank = False
    
    for line in lines:
        stripped = line.strip()
        if stripped == "":
            if not last_was_blank:
                cleaned_lines.append("")
                last_was_blank = True
        else:
            cleaned_lines.append(line)
            last_was_blank = False
            
    return "\n".join(cleaned_lines)

def main():
    cwd = os.getcwd()
    print(f"Procesando código en: {cwd}")
    contador = 0
    
    try:
        with open(ARCHIVO_SALIDA, 'w', encoding='utf-8') as outfile:
            for root, dirs, files in os.walk(cwd):
                # Filtrar carpetas ignoradas
                dirs[:] = [d for d in dirs if d not in CARPETAS_IGNORAR]
                
                for file in files:
                    # VALIDACIÓN: Extensiones, no ser el propio script y NO estar en la lista negra
                    es_extension_valida = file.lower().endswith(EXTENSIONES)
                    es_script_actual = (file == os.path.basename(__file__))
                    esta_ignorado = (file in ARCHIVOS_IGNORAR)

                    if es_extension_valida and not es_script_actual and not esta_ignorado:
                        ruta_completa = os.path.join(root, file)
                        ruta_relativa = os.path.relpath(ruta_completa, cwd)
                        
                        try:
                            with open(ruta_completa, 'r', encoding='utf-8', errors='replace') as infile:
                                contenido_original = infile.read()
                                contenido_limpio = clean_code(contenido_original)
                                
                                if contenido_limpio.strip():
                                    outfile.write(f"\n{'='*40}\nFILE: {ruta_relativa}\n{'='*40}\n")
                                    outfile.write(contenido_limpio)
                                    outfile.write("\n")
                                    print(f"[OK] Limpiado y agregado: {ruta_relativa}")
                                    contador += 1
                        except Exception as e:
                            print(f"[ERROR] No se pudo leer {ruta_relativa}: {e}")
                    
                    elif esta_ignorado:
                        print(f"[SALTADO] Archivo excluido por lista: {file}")

        print(f"\nListo. {contador} archivos procesados en '{ARCHIVO_SALIDA}'.")

    except Exception as e:
        print(f"Error fatal: {e}")

if __name__ == "__main__":
    main()