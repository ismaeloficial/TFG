import axios from "axios";

const api = axios.create({
  baseURL: "http://localhost:8001",
});

export const getHistorial = () => api.get("/historial").then((res) => res.data);

export const getFichasCaja = () => api.get("/fichas-caja").then((res) => res.data);

export const createFichaCaja = (ficha) => api.post("/fichas-caja", ficha).then((res) => res.data);

export const updateFichaCaja = (id, ficha) =>
  api.put(`/fichas-caja/${id}`, ficha).then((res) => res.data);

export default api;
