import axios from "axios";

const api = axios.create({
  baseURL: "http://localhost:8001",
});

export const getEvents = () => api.get("/events").then((res) => res.data);

export const getBoxes = () => api.get("/boxes").then((res) => res.data);

export const createBox = (box) => api.post("/boxes", box).then((res) => res.data);

export const updateBox = (id, box) => api.put(`/boxes/${id}`, box).then((res) => res.data);

export default api;
