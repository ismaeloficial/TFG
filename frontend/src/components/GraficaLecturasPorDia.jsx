import { useMemo, useState } from "react";
import { ResponsiveContainer, BarChart, Bar, XAxis, YAxis, CartesianGrid, Tooltip } from "recharts";

const VISTAS = [
  { id: "dia", label: "Día" },
  { id: "mes", label: "Mes" },
  { id: "anio", label: "Año" },
];

const MESES = ["Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"];

function datosPorHora(historial, ahora) {
  const conteos = Array(24).fill(0);
  for (const entry of historial) {
    const fecha = new Date(entry.read_at);
    if (
      fecha.getFullYear() === ahora.getFullYear() &&
      fecha.getMonth() === ahora.getMonth() &&
      fecha.getDate() === ahora.getDate()
    ) {
      conteos[fecha.getHours()]++;
    }
  }
  return conteos.map((count, hora) => ({ etiqueta: `${hora}h`, count }));
}

function datosPorDiaDelMes(historial, ahora) {
  const diasEnMes = new Date(ahora.getFullYear(), ahora.getMonth() + 1, 0).getDate();
  const conteos = Array(diasEnMes).fill(0);
  for (const entry of historial) {
    const fecha = new Date(entry.read_at);
    if (fecha.getFullYear() === ahora.getFullYear() && fecha.getMonth() === ahora.getMonth()) {
      conteos[fecha.getDate() - 1]++;
    }
  }
  return conteos.map((count, i) => ({ etiqueta: `${i + 1}`, count }));
}

function datosPorMes(historial, ahora) {
  const conteos = Array(12).fill(0);
  for (const entry of historial) {
    const fecha = new Date(entry.read_at);
    if (fecha.getFullYear() === ahora.getFullYear()) {
      conteos[fecha.getMonth()]++;
    }
  }
  return conteos.map((count, i) => ({ etiqueta: MESES[i], count }));
}

// Actividad de lectura (RFID+QR) agrupada por hora/día/mes según la vista elegida.
// Siempre sobre el periodo "actual" (hoy, este mes, este año) — sin navegación a
// periodos anteriores por ahora.
export default function GraficaLecturasPorDia({ historial }) {
  const [vista, setVista] = useState("mes");

  const datos = useMemo(() => {
    const ahora = new Date();
    if (vista === "dia") return datosPorHora(historial, ahora);
    if (vista === "anio") return datosPorMes(historial, ahora);
    return datosPorDiaDelMes(historial, ahora);
  }, [historial, vista]);

  return (
    <div className="grafica-con-controles">
      <div className="segmented-control">
        {VISTAS.map((v) => (
          <button
            key={v.id}
            className={vista === v.id ? "activo" : ""}
            onClick={() => setVista(v.id)}
          >
            {v.label}
          </button>
        ))}
      </div>
      <div className="grafica-contenido">
        <ResponsiveContainer width="100%" height="100%">
          <BarChart data={datos} margin={{ top: 5, right: 10, left: -10, bottom: 0 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="#eee" />
            <XAxis dataKey="etiqueta" tick={{ fontSize: 11 }} interval={vista === "mes" ? 2 : 0} />
            <YAxis allowDecimals={false} tick={{ fontSize: 11 }} width={28} />
            <Tooltip />
            <Bar dataKey="count" fill="#2f8f4e" radius={[3, 3, 0, 0]} />
          </BarChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}
