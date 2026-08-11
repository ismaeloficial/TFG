#!/bin/bash

# Arranca todo el entorno local del TFG: Mosquitto (Docker), backend (FastAPI)
# y frontend (Vite/React), y los para todos juntos con Ctrl+C.

GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'
BOLD='\033[1m'

BASE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKEND_DIR="$BASE_DIR/backend"
FRONTEND_DIR="$BASE_DIR/frontend"
VENV_DIR="$BACKEND_DIR/.venv"
BACKEND_PORT=8001
FRONTEND_PORT=5175

cd "$BASE_DIR" || exit 1

echo -e "${CYAN}${BOLD}====================================================${NC}"
echo -e "${CYAN}${BOLD}   ARRANCANDO TRAZABILIDAD DE CAJAS - ENTORNO LOCAL   ${NC}"
echo -e "${CYAN}${BOLD}====================================================${NC}"

# Limpiar procesos huérfanos de arranques anteriores en nuestros puertos
echo -e "\n${CYAN}[i] Limpiando procesos anteriores...${NC}"
lsof -ti:$BACKEND_PORT | xargs kill -9 2>/dev/null
lsof -ti:$FRONTEND_PORT | xargs kill -9 2>/dev/null

cleanup() {
    echo -e "\n${RED}${BOLD}[!] Deteniendo servidores locales...${NC}"
    kill "$BACKEND_PID" 2>/dev/null
    kill "$FRONTEND_PID" 2>/dev/null
    echo -e "${GREEN}[✔] Servidores apagados. (Mosquitto se queda corriendo en Docker; 'docker compose down' si quieres pararlo también.)${NC}"
    exit 0
}
trap cleanup SIGINT SIGTERM

# --- 1. MOSQUITTO (Docker) ---
echo -e "\n${CYAN}[1/3] Levantando Mosquitto (MQTT) en Docker...${NC}"
if ! docker info >/dev/null 2>&1; then
    echo -e "${RED}[✘] Docker no está arrancado. Abre Docker Desktop y vuelve a intentarlo.${NC}"
    exit 1
fi
docker compose up -d mosquitto
if [ $? -ne 0 ]; then
    echo -e "${RED}[✘] Error al iniciar Mosquitto.${NC}"
    exit 1
fi
echo -e "${GREEN}[✔] Mosquitto arriba (puerto 1883).${NC}"

# --- 2. BACKEND ---
echo -e "\n${CYAN}[2/3] Arrancando el backend (FastAPI)...${NC}"
if [ ! -d "$VENV_DIR" ]; then
    echo -e "${YELLOW}[!] No se encontró el entorno virtual del backend. Creándolo...${NC}"
    python3 -m venv "$VENV_DIR"
    "$VENV_DIR/bin/pip" install -r "$BACKEND_DIR/requirements.txt"
fi

cd "$BACKEND_DIR"
"$VENV_DIR/bin/uvicorn" app.main:app --host 0.0.0.0 --port $BACKEND_PORT --reload &
BACKEND_PID=$!
cd "$BASE_DIR"
echo -e "${GREEN}[✔] Backend en http://localhost:$BACKEND_PORT (docs en /docs)${NC}"

sleep 1

# --- 3. FRONTEND ---
echo -e "\n${CYAN}[3/3] Arrancando el frontend (Vite/React)...${NC}"
if [ ! -d "$FRONTEND_DIR/node_modules" ]; then
    echo -e "${YELLOW}[!] No se encontraron dependencias de Node. Instalando...${NC}"
    (cd "$FRONTEND_DIR" && npm install)
fi

(cd "$FRONTEND_DIR" && npm run dev) &
FRONTEND_PID=$!
echo -e "${GREEN}[✔] Frontend en http://localhost:$FRONTEND_PORT${NC}"

echo -e "\n${GREEN}${BOLD}[✔] Todo arrancado. Abre http://localhost:$FRONTEND_PORT en el navegador.${NC}"
echo -e "${YELLOW}Presiona CTRL+C para detener backend y frontend juntos.${NC}"
echo -e "${CYAN}----------------------------------------------------${NC}"

wait
