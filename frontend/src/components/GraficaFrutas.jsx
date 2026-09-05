import { useMemo } from "react";
import { ResponsiveContainer, LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend } from "recharts";

const DIAS_VENTANA = 14;
const PALETA = ["#2f8f4e", "#e0631e", "#c0392b", "#1a5fb4", "#8e44ad", "#d4a017"];

function claveDia(date) {
  return date.toISOString().slice(0, 10);
}

function etiquetaDia(claveISO) {
  const [, mes, dia] = claveISO.split("-");
  return `${dia}/${mes}`;
}

// Cuenta lecturas por día y por tipo de fruta, sobre los últimos DIAS_VENTANA días.
// Cada lectura representa una caja de ese producto pasando por el punto de lectura
// (no se deduplica por ficha_caja: dos lecturas del mismo código son dos cajas reales
// del mismo producto, no la misma caja contada dos veces).
export default function GraficaFrutas({ historial, fichasByCode }) {
  const { datos, tipos } = useMemo(() => {
    const hoy = new Date();
    const desde = new Date(hoy);
    desde.setDate(desde.getDate() - (DIAS_VENTANA - 1));
    desde.setHours(0, 0, 0, 0);

    const dias = [];
    for (let i = 0; i < DIAS_VENTANA; i++) {
      const d = new Date(desde);
      d.setDate(d.getDate() + i);
      dias.push(claveDia(d));
    }

    const tiposEncontrados = new Set();
    const conteos = {};
    dias.forEach((d) => (conteos[d] = {}));

    for (const entry of historial) {
      const ficha = fichasByCode?.[entry.raw_value];
      if (!ficha?.tipo_fruta) continue;

      const fecha = new Date(entry.read_at);
      if (fecha < desde) continue;

      const diaISO = claveDia(fecha);
      if (!(diaISO in conteos)) continue;

      tiposEncontrados.add(ficha.tipo_fruta);
      conteos[diaISO][ficha.tipo_fruta] = (conteos[diaISO][ficha.tipo_fruta] || 0) + 1;
    }

    const tipos = [...tiposEncontrados].sort();
    const datos = dias.map((d) => ({
      dia: etiquetaDia(d),
      ...Object.fromEntries(tipos.map((t) => [t, conteos[d][t] || 0])),
    }));

    return { datos, tipos };
  }, [historial, fichasByCode]);

  if (tipos.length === 0) {
    return <div className="grafica-placeholder">Todavía no hay lecturas asociadas a una ficha de caja.</div>;
  }

  return (
    <ResponsiveContainer width="100%" height="100%">
      <LineChart data={datos} margin={{ top: 5, right: 10, left: -10, bottom: 0 }}>
        <CartesianGrid strokeDasharray="3 3" stroke="#eee" />
        <XAxis dataKey="dia" tick={{ fontSize: 11 }} />
        <YAxis allowDecimals={false} tick={{ fontSize: 11 }} width={28} />
        <Tooltip />
        <Legend wrapperStyle={{ fontSize: 12 }} />
        {tipos.map((tipo, i) => (
          <Line
            key={tipo}
            type="monotone"
            dataKey={tipo}
            stroke={PALETA[i % PALETA.length]}
            strokeWidth={2}
            dot={{ r: 2 }}
            activeDot={{ r: 4 }}
          />
        ))}
      </LineChart>
    </ResponsiveContainer>
  );
}
