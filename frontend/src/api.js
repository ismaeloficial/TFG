import axios from "axios";

const api = axios.create({
  baseURL: "http://localhost:8001",
});

// El backend devuelve como mucho 50 filas si no se le pide lo contrario (más
// recientes primero) — con el volumen real de pruebas ya por encima de eso,
// se quedaba fuera todo lo anterior al mes en curso, y por eso desaparecía de
// las gráficas de "Año". 200 es el máximo que el propio backend permite.
export const getHistorial = () => api.get("/historial?limit=200").then((res) => res.data);

export const getFichasCaja = () => api.get("/fichas-caja").then((res) => res.data);

export const createFichaCaja = (ficha) => api.post("/fichas-caja", ficha).then((res) => res.data);

export const updateFichaCaja = (id, ficha) =>
  api.put(`/fichas-caja/${id}`, ficha).then((res) => res.data);

export default api;
